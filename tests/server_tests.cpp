#include <gtest/gtest.h>
#include "../src/server/request/request_handler.h"
#include "../src/writer/csv_writer.h"
#include "../src/composer/composer.h"
#include <boost/beast.hpp>
#include <memory>

namespace http = boost::beast::http;

class ServerTest : public testing::Test {
protected:
    http::request<http::string_body> create_request(
        http::verb method,
        std::string target,
        std::string body,
        std::string content_type = "application/json") {
        http::request<http::string_body> req{method, target, 11};
        req.body() = body;
        req.set(http::field::content_type, content_type);
        req.prepare_payload();
        return req;
    }
};

TEST_F(ServerTest, CsvWriter_WritesCorrectCsv) {
    CsvWriter writer;
    std::vector<Field> fields = {{"id", "int"}, {"name", "string"}};
    auto gen = std::make_shared<Generator>();
    Composer composer("test_table", fields, gen);
    composer.compose(2);

    writer.write(composer);
    std::string result = writer.str();

    EXPECT_NE(result, "");
}

TEST_F(ServerTest, HandleRequest_PostGenerate_Success) {
    std::string json_body = R"({"table_name": "test", "fields": [{"name": "id", "type": "int"}, {"name": "name", "type": "string"}], "rows": 2, "output_file": "test.csv"})";
    auto req = create_request(http::verb::post, "/generate", json_body);

    auto resp = handle_request(req);

    EXPECT_EQ(resp.result(), http::status::ok);
    EXPECT_EQ(resp[http::field::content_type], "text/csv");
    EXPECT_EQ(resp[http::field::content_disposition], "attachment; filename=\"test.csv\"");
    EXPECT_NE(resp.body(), "");
}

TEST_F(ServerTest, HandleRequest_InvalidContentType) {
    std::string json_body = R"({"table_name": "test", "fields": [{"name": "id", "type": "int"}, {"name": "name", "type": "string"}], "rows": 2, "output_file": "test.csv"})";
    auto req = create_request(http::verb::post, "/generate", json_body, "text/plain");

    auto resp = handle_request(req);

    EXPECT_EQ(resp.result(), http::status::bad_request);
    EXPECT_EQ(resp[http::field::content_type], "application/json");
    EXPECT_EQ(resp.body(), R"({"error": "Content-Type must be application/json"})");
}

TEST_F(ServerTest, HandleRequest_NotFound) {
    auto req = create_request(http::verb::get, "/wrong_endpoint", "");

    auto resp = handle_request(req);

    EXPECT_EQ(resp.result(), http::status::not_found);
    EXPECT_EQ(resp[http::field::content_type], "application/json");
    EXPECT_EQ(resp.body(), R"({"error": "Endpoint not found"})");
}
