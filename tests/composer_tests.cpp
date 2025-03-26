#include <gtest/gtest.h>
#include "../src/composer/composer.h"
#include <memory>

class ComposerTest : public ::testing::Test {
protected:
    void SetUp() override {
        fields = {
            {"id", "int", {{"min", "1"}, {"max", "100"}}},
            {"name", "string", {{"length", "10"}}},
            {"price", "double", {{"min", "0.0"}, {"max", "1000.0"}}}
        };
    }

    std::vector<Field> fields;
    std::string table_name = "test_table";
};

TEST_F(ComposerTest, ConstructorWithGenerator) {
    auto gen = std::make_shared<Generator>();
    Composer composer(table_name, fields, gen);
    EXPECT_EQ(composer.getTableName(), table_name);
    EXPECT_EQ(composer.getFields().size(), fields.size());
}

TEST_F(ComposerTest, ComposeRowsCount) {
    Composer composer(table_name, fields);
    int num_rows = 5;
    composer.compose(num_rows);
    EXPECT_EQ(composer.getRows().size(), num_rows);
}

TEST_F(ComposerTest, ComposeRowsFieldsCount) {
    Composer composer(table_name, fields);
    composer.compose(3);
    const auto& rows = composer.getRows();
    for (const auto& row : rows) {
        EXPECT_EQ(row.size(), fields.size());
    }
}