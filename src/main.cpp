#include <boost/program_options.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "composer/composer.h"
#include "writer/csv_writer.h"

namespace po = boost::program_options;

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

int main(int argc, char* argv[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("table-name,n", po::value<std::string>()->required(), "table name")
            ("output,o", po::value<std::string>()->default_value("output.csv"), "output CSV file")
            ("fields,f", po::value<std::vector<std::string>>()->multitoken(), "field definitions (e.g., id:int:min=1:max=1000)")
            ("rows,r", po::value<int>()->default_value(10), "number of rows to generate");

        po::variables_map vm;
       store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help") || argc == 1) {
            std::cout << desc << "\n";
            return 0;
        }

        notify(vm);

        std::string table_name = vm["table-name"].as<std::string>();
        std::string output_file = vm["output"].as<std::string>();
        int num_rows = vm["rows"].as<int>();

        std::vector<Field> fields;
        if (vm.count("fields")) {
            const auto& field_args = vm["fields"].as<std::vector<std::string>>();
            for (const auto& field_str : field_args) {
                fields.push_back(parse_field(field_str));
            }
        } else {
            throw std::runtime_error("No fields specified. Use -f to define fields.");
        }

        auto gen = std::make_shared<Generator>();
        Composer composer(table_name, fields, gen);

        composer.compose(num_rows);

        CsvWriter writer(output_file);
        writer.write(composer);
        std::cout << "Data written to " << output_file << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}