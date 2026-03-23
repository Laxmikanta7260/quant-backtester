#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <string>
#include <vector>

struct Bar {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double volume;
};

class DataLoader {
public:
    static std::vector<Bar> loadCSV(const std::string& filename);
};

#endif