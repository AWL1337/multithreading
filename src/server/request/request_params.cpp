#include "request_params.h"
#include <boost/json.hpp>
#include <sstream>

namespace json = boost::json;

Field parse_field(const std::string& field_str) {
    Field field;
    std::istringstream iss(field_str);
    std::string token;

    if (!std::getline(iss, field.name, ':')) {
        throw std::runtime_error("Invalid field format: " + field_str + " (missing name)");
    }

    if (!std::getline(iss, field.type, ':')) {
        throw std::runtime_error("Invalid field format: " + field_str + " (missing type)");
    }

    while (std::getline(iss, token, ':')) {
        size_t eq_pos = token.find('=');
        if (eq_pos == std::string::npos) {
            throw std::runtime_error("Invalid parameter format in field: " + field_str);
        }
        std::string key = token.substr(0, eq_pos);
        std::string value = token.substr(eq_pos + 1);
        field.params[key] = value;
    }

    return field;
}

RequestParams parse_json_body(const std::string& body) {
    RequestParams params;

    try {
        json::value jv = json::parse(body);
        json::object obj = jv.as_object();

        params.table_name = json::value_to<std::string>(obj.at("table_name"));
        params.rows = json::value_to<int>(obj.at("rows"));
        params.output_file = json::value_to<std::string>(obj.at("output_file"));

        json::array fields_arr = obj.at("fields").as_array();
        for (const auto& field_val : fields_arr) {
            json::object field_obj = field_val.as_object();
            std::string field_str = json::value_to<std::string>(field_obj.at("name")) + ":" +
                                  json::value_to<std::string>(field_obj.at("type"));

            if (field_obj.contains("params")) {
                json::object params_obj = field_obj.at("params").as_object();
                for (const auto& [key, val] : params_obj) {
                    field_str += ":" + std::string(key) + "=" + json::value_to<std::string>(val);
                }
            }
            params.fields.push_back(parse_field(field_str));
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid JSON body: " + std::string(e.what()));
    }

    if (params.table_name.empty()) params.table_name = "default_table";
    if (params.output_file.empty()) params.output_file = "output.csv";
    if (params.rows <= 0) params.rows = 10;
    if (params.fields.empty()) {
        params.fields.push_back(parse_field("id:int:min=1:max=1000"));
    }

    return params;
}