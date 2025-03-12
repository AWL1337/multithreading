#include <gtest/gtest.h>
#include "../src/writer/csv_writer.h"
#include "../src/composer/composer.h"
#include <fstream>

TEST(CsvWriterTest, WriteFileExists) {
    std::vector<Field> fields = {
        {"id", "int", {{"min", "1"}, {"max", "10"}}},
        {"name", "string", {{"length", "5"}}}
    };
    Composer composer("test_table", fields);
    composer.compose(2);
    
    std::string file_path = "test_output.csv";
    CsvWriter writer(file_path);
    writer.write(composer);
    
    std::ifstream file(file_path);
    EXPECT_TRUE(file.good());
    file.close();
    
    std::remove(file_path.c_str());
}

TEST(CsvWriterTest, CustomSeparator) {
    std::vector<Field> fields = {
        {"id", "int", {{"min", "1"}, {"max", "10"}}},
        {"name", "string", {{"length", "5"}}}
    };
    Composer composer("test_table", fields);
    composer.compose(1);
    
    std::string file_path = "test_output.csv";
    char separator = '|';
    CsvWriter writer(file_path, separator);
    writer.write(composer);
    
    std::ifstream file(file_path);
    std::string line;
    std::getline(file, line);
    std::getline(file, line);
    EXPECT_NE(line.find(separator), std::string::npos);
    file.close();
    
    std::remove(file_path.c_str());
}