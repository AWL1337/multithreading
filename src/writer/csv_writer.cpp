#include "csv_writer.h"

CsvWriter::CsvWriter(char separator) : separator_(separator) {
}

void CsvWriter::write(const Composer& composer) {
    buffer_.str("");

    const auto& fields = composer.getFields();
    for (size_t i = 0; i < fields.size(); ++i) {
        buffer_ << fields[i].name;
        if (i < fields.size() - 1) {
            buffer_ << separator_;
        }
    }
    buffer_ << "\n";

    const auto& rows = composer.getRows();
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            buffer_ << row[i];
            if (i < row.size() - 1) {
                buffer_ << separator_;
            }
        }
        buffer_ << "\n";
    }
}

std::string CsvWriter::str() const {
    return buffer_.str();
}

std::istringstream CsvWriter::get_stream() const {
    return std::istringstream(buffer_.str());
}