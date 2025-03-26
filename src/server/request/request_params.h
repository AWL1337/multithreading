#pragma once

#include <string>
#include <vector>
#include "../../composer/composer.h"

Field parse_field(const std::string& field_str);

struct RequestParams {
    std::string table_name;
    std::vector<Field> fields;
    int rows;
    std::string output_file;
};

RequestParams parse_json_body(const std::string& body);