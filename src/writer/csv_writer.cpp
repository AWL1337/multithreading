#include "csv_writer.h"
#include <fstream>
#include <stdexcept>


CsvWriter::CsvWriter(std::string  file_path) : file_path(std::move(file_path)), separator(',') {
}

CsvWriter::CsvWriter(std::string file_path, const char separator) : file_path(std::move(file_path)), separator(separator) {
}

void CsvWriter::write(const Composer& composer) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + file_path);
    }

    const auto& fields = composer.getFields();
    for (size_t i = 0; i < fields.size(); ++i) {
        file << fields[i].name;
        if (i < fields.size() - 1) {
            file << separator;
        }
    }
    file << "\n";

    const auto& rows = composer.getRows();
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) {
                file << separator;
            }
        }
        file << "\n";
    }

    file.close();
}
