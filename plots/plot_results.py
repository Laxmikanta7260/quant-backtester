import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

equity = pd.read_csv("output/equity_curve.csv")
trades = pd.read_csv("output/trades.csv")
price = pd.read_csv("data/sample_prices.csv")

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
plt.show()