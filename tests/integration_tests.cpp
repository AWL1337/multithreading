#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <sstream>
#include <thread>
#include <vector>
#include <regex>
#include "../src/server/server.h"

namespace ip = boost::asio::ip;
namespace http = boost::beast::http;
namespace json = boost::json;

std::vector<std::vector<std::string>> parse_csv(const std::string& csv) {
    std::vector<std::vector<std::string>> result;
    std::istringstream iss(csv);
    std::string line;
    while (std::getline(iss, line)) {
        std::vector<std::string> row;
        std::istringstream line_stream(line);
        std::string cell;
        while (std::getline(line_stream, cell, ',')) {
            row.push_back(cell);
        }
        result.push_back(row);
    }
    return result;
}

http::response<http::string_body> send_post_request(const std::string& host, int port, const std::string& body) {
    boost::asio::io_context ioc;
    ip::tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, std::to_string(port));

    ip::tcp::socket socket(ioc);
    boost::asio::connect(socket, results.begin(), results.end());

    http::request<http::string_body> req{http::verb::post, "/generate", 11};
    req.set(http::field::host, host);
    req.set(http::field::content_type, "application/json");
    req.body() = body;
    req.prepare_payload();

    http::write(socket, req);

    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(socket, buffer, res);

    socket.shutdown(ip::tcp::socket::shutdown_both);
    return res;
}

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        server_thread_ = std::thread([]() {
            start_server(8080, 2);
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void TearDown() override {
        server_thread_.detach();
    }

private:
    std::thread server_thread_;
};

TEST_F(IntegrationTest, TestGenerateEndpoint) {
    json::object request_body;
    request_body["table_name"] = "users";
    request_body["rows"] = 10;
    request_body["output_file"] = "users.csv";
    json::array fields;
    json::object field1, field2, field3;

    field1["name"] = "id";
    field1["type"] = "int";
    field1["params"] = json::object{{"min", "1"}, {"max", "1000"}};

    field2["name"] = "name";
    field2["type"] = "string";
    field2["params"] = json::object{{"length", "10"}};

    field3["name"] = "age";
    field3["type"] = "int";
    field3["params"] = json::object{{"min", "18"}, {"max", "60"}};

    fields.push_back(field1);
    fields.push_back(field2);
    fields.push_back(field3);
    request_body["fields"] = fields;

    std::string request_body_str = json::serialize(request_body);

    auto res = send_post_request("localhost", 8080, request_body_str);

    EXPECT_EQ(res.result(), http::status::ok) << "Expected HTTP 200 OK";
    EXPECT_EQ(res[http::field::content_type], "text/csv") << "Expected Content-Type: text/csv";
    EXPECT_EQ(res[http::field::content_disposition], "attachment; filename=\"users.csv\"")
        << "Expected Content-Disposition: attachment; filename=\"users.csv\"";

    auto csv_rows = parse_csv(res.body());

    EXPECT_EQ(csv_rows.size(), 11) << "Expected 11 rows (10 data rows + header)";

    ASSERT_GE(csv_rows.size(), 1) << "CSV should have at least a header row";
    EXPECT_EQ(csv_rows[0].size(), 3) << "Header should have 3 columns";
    EXPECT_EQ(csv_rows[0][0], "id") << "First column should be 'id'";
    EXPECT_EQ(csv_rows[0][1], "name") << "Second column should be 'name'";
    EXPECT_EQ(csv_rows[0][2], "age") << "Third column should be 'age'";

    for (size_t i = 1; i < csv_rows.size(); ++i) {
        auto& row = csv_rows[i];
        EXPECT_EQ(row.size(), 3) << "Row " << i << " should have 3 columns";

        int id;
        try {
            id = std::stoi(row[0]);
        } catch (const std::exception& e) {
            FAIL() << "Row " << i << ": id is not an integer";
        }
        EXPECT_GE(id, 1) << "Row " << i << ": id should be >= 1";
        EXPECT_LE(id, 1000) << "Row " << i << ": id should be <= 1000";

        EXPECT_EQ(row[1].size(), 10) << "Row " << i << ": name should be 10 characters long";
        EXPECT_TRUE(std::regex_match(row[1], std::regex("[a-zA-Z0-9]+")))
            << "Row " << i << ": name should contain only alphanumeric characters";

        int age;
        try {
            age = std::stoi(row[2]);
        } catch (const std::exception& e) {
            FAIL() << "Row " << i << ": age is not an integer";
        }
        EXPECT_GE(age, 18) << "Row " << i << ": age should be >= 18";
        EXPECT_LE(age, 60) << "Row " << i << ": age should be <= 60";
    }
}