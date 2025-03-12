#pragma once

#include "../composer/composer.h"
#include <string>

class CsvWriter {
public:
    CsvWriter(std::string file_path);
    CsvWriter(std::string file_path, char separator);

    void write(const Composer& composer) const;

private:
    std::string file_path;
    char separator;
};