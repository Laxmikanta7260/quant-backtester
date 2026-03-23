from fastapi import FastAPI, Form, Request
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates

import subprocess
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import os

app = FastAPI()

app.mount("/static", StaticFiles(directory="static"), name="static")
templates = Jinja2Templates(directory="templates")


def run_cpp_backtester(strategy: str, short_window: int, long_window: int, rsi_period: int, overbought: float, oversold: float, transaction_cost: float):
    env = os.environ.copy()
    env["STRATEGY_TYPE"] = strategy
    env["SHORT_WINDOW"] = str(short_window)
    env["LONG_WINDOW"] = str(long_window)
    env["RSI_PERIOD"] = str(rsi_period)
    env["RSI_OVERBOUGHT"] = str(overbought)
    env["RSI_OVERSOLD"] = str(oversold)
    env["TRANSACTION_COST"] = str(transaction_cost)

    result = subprocess.run(
        ["quant_backtester.exe"],
        cwd="build",
        capture_output=True,
        text=True,
        env=env,
        shell=True
    )

    return result.stdout, result.stderr


def generate_chart():
    equity_path = os.path.join("output", "equity_curve.csv")
    trades_path = os.path.join("output", "trades.csv")
    price_path = os.path.join("data", "sample_prices.csv")

    if not (os.path.exists(equity_path) and os.path.exists(trades_path) and os.path.exists(price_path)):
        return False

    equity = pd.read_csv(equity_path)
    trades = pd.read_csv(trades_path)
    price = pd.read_csv(price_path)

    equity["Date"] = pd.to_datetime(equity["Date"], dayfirst=True, errors="coerce")
    trades["Date"] = pd.to_datetime(trades["Date"], dayfirst=True, errors="coerce")
    price["Date"] = pd.to_datetime(price["Date"], dayfirst=True, errors="coerce")

    buy_trades = trades[trades["Action"] == "BUY"]
    sell_trades = trades[trades["Action"] == "SELL"]

    buy_price_points = pd.merge(buy_trades, price, on="Date", how="left")
    sell_price_points = pd.merge(sell_trades, price, on="Date", how="left")

    buy_equity_points = pd.merge(buy_trades, equity, on="Date", how="left")
    sell_equity_points = pd.merge(sell_trades, equity, on="Date", how="left")

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)

    ax1.plot(price["Date"], price["Close"], label="Price")
    ax1.scatter(buy_price_points["Date"], buy_price_points["Close"], marker="^", s=30, label="Buy")
    ax1.scatter(sell_price_points["Date"], sell_price_points["Close"], marker="v", s=30, label="Sell")
    ax1.set_title("Price Chart with Buy/Sell Signals")
    ax1.set_ylabel("Price")
    ax1.legend()

    ax2.plot(equity["Date"], equity["Equity"], label="Equity Curve")
    ax2.scatter(buy_equity_points["Date"], buy_equity_points["Equity"], marker="^", s=30, label="Buy")
    ax2.scatter(sell_equity_points["Date"], sell_equity_points["Equity"], marker="v", s=30, label="Sell")
    ax2.set_title("Equity Curve")
    ax2.set_xlabel("Date")
    ax2.set_ylabel("Portfolio Value")
    ax2.legend()

    ax2.xaxis.set_major_locator(mdates.YearLocator(5))
    ax2.xaxis.set_major_formatter(mdates.DateFormatter("%Y"))

    plt.tight_layout()
    plt.savefig(os.path.join("static", "chart.png"))
    plt.close()

    return True


def load_summary():
    equity_path = os.path.join("output", "equity_curve.csv")
    trades_path = os.path.join("output", "trades.csv")

    if not (os.path.exists(equity_path) and os.path.exists(trades_path)):
        return None

    equity = pd.read_csv(equity_path)
    trades = pd.read_csv(trades_path)

    total_return = 0.0
    if len(equity) > 1:
        total_return = (equity["Equity"].iloc[-1] - equity["Equity"].iloc[0]) / equity["Equity"].iloc[0]

    peak = equity["Equity"].cummax()
    drawdown = (peak - equity["Equity"]) / peak
    max_drawdown = drawdown.max() if len(drawdown) > 0 else 0.0

    daily_returns = equity["Equity"].pct_change().dropna()
    sharpe = 0.0
    if len(daily_returns) > 1 and daily_returns.std() != 0:
        sharpe = daily_returns.mean() / daily_returns.std()

    return {
        "num_trades": len(trades),
        "total_return": round(float(total_return), 6),
        "max_drawdown": round(float(max_drawdown), 6),
        "sharpe_ratio": round(float(sharpe), 6),
    }


def load_tuning_results():
    tuning_path = os.path.join("output", "tuning_results.csv")
    if not os.path.exists(tuning_path):
        return []

    df = pd.read_csv(tuning_path)
    df = df.sort_values(by="SharpeRatio", ascending=False)
    return df.to_dict(orient="records")


@app.get("/", response_class=HTMLResponse)
def home(request: Request):
    return templates.TemplateResponse(
        "index.html",
        {
            "request": request,
            "summary": None,
            "stdout": "",
            "stderr": "",
            "chart_exists": False,
            "tuning_results": []
        }
    )


@app.post("/run", response_class=HTMLResponse)
def run_backtest(
    request: Request,
    strategy: str = Form(...),
    short_window: int = Form(20),
    long_window: int = Form(50),
    rsi_period: int = Form(14),
    overbought: float = Form(70.0),
    oversold: float = Form(30.0),
    transaction_cost: float = Form(0.001),
):
    stdout, stderr = run_cpp_backtester(
        strategy, short_window, long_window, rsi_period, overbought, oversold, transaction_cost
    )

    summary = load_summary()
    chart_exists = generate_chart()
    tuning_results = load_tuning_results()

    return templates.TemplateResponse(
        "index.html",
        {
            "request": request,
            "summary": summary,
            "stdout": stdout,
            "stderr": stderr,
            "chart_exists": chart_exists,
            "tuning_results": tuning_results
        }
    )