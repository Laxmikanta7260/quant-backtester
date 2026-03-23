#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <string>
#include <vector>
#include "tuner.h"

class CSVWriter {
public:
    static void writeTuningResults(
        const std::string& filename,
        const std::vector<TuningResult>& results
    );
};

#endif