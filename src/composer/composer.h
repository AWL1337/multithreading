#pragma once

#include "../generator/generator.h"
#include <string>
#include <vector>
#include <map>

struct Field {
    std::string name;
    std::string type;
    std::map<std::string, std::string> params;
};

class Composer {
public:
    Composer(std::string  table_name, const std::vector<Field>& fields);
    Composer(std::string  table_name, const std::vector<Field>& fields, std::shared_ptr<Generator> gen);

    void compose(int num_rows);

    const std::vector<std::vector<std::string>>& getRows() const { return rows; }
    const std::string& getTableName() const { return table_name; }
    const std::vector<Field>& getFields() const { return fields; }

private:
    std::string table_name;
    std::vector<Field> fields;
    std::vector<std::vector<std::string>> rows;
    std::shared_ptr<Generator> generator;

    std::string generateFieldValue(const Field& field);
};