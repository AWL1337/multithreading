#include "composer.h"
#include <stdexcept>

Composer::Composer(std::string  table_name, const std::vector<Field>& fields)
    : table_name(std::move(table_name)), fields(fields), generator(std::make_shared<Generator>()) {}

Composer::Composer(std::string  table_name, const std::vector<Field>& fields, std::shared_ptr<Generator> gen)
    : table_name(std::move(table_name)), fields(fields), generator(std::move(gen)) {}

void Composer::compose(int num_rows) {
    rows.clear();
    for (int i = 0; i < num_rows; ++i) {
        std::vector<std::string> row;
        row.reserve(fields.size());
        for (const auto& field : fields) {
            row.push_back(generateFieldValue(field));
        }
        rows.push_back(row);
    }
}

std::string Composer::generateFieldValue(const Field& field) {
    if (field.type == "int") {
        int min_val = std::stoi(field.params.count("min") ? field.params.at("min") : "1");
        int max_val = std::stoi(field.params.count("max") ? field.params.at("max") : "1000");
        return std::to_string(generator->generateInt(min_val, max_val));
    }
    if (field.type == "double") {
        double min_val = std::stod(field.params.count("min") ? field.params.at("min") : "0.0");
        double max_val = std::stod(field.params.count("max") ? field.params.at("max") : "1000.0");
        return std::to_string(generator->generateDouble(min_val, max_val));
    }
    if (field.type == "string") {
        size_t length = std::stoul(field.params.count("length") ? field.params.at("length") : "10");
        return generator->generateString(length);
    }
    if (field.type == "name") {
        return generator->generateName();
    }
    throw std::runtime_error("Unknown field type: " + field.type);
}