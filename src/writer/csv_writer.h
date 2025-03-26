#pragma once

#include "../composer/composer.h"
#include <string>
#include <sstream>

class CsvWriter {
public:
    CsvWriter(char separator = ',');

    void write(const Composer& composer);

    std::string str() const;

    std::istringstream get_stream() const;

private:
    std::stringstream buffer_;
    char separator_;
};