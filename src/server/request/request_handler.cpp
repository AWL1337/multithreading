#include "request_handler.h"
#include "request_params.h"
#include "../../writer/csv_writer.h"
#include <fstream>


namespace {
    struct CacheEntry {
        http::response<http::string_body> response;
    };

    std::unordered_map<std::string, CacheEntry> request_cache;
    std::mutex cache_mutex;
}

http::response<http::string_body> handle_request(const http::request<http::string_body>& req) {
    http::response<http::string_body> resp;
    resp.version(11);
    resp.set(http::field::server, BOOST_BEAST_VERSION_STRING);

    std::string cache_key = std::string(req.method_string()) +
                           std::string(req.target()) +
                           req.body();

    {
        std::lock_guard<std::mutex> lock(cache_mutex);
        auto it = request_cache.find(cache_key);
        if (it != request_cache.end()) {
            resp = it->second.response;
            resp.prepare_payload();
            return resp;
        }
    }

    try {
        if (req.method() == http::verb::post && req.target() == "/generate") {
            if (req[http::field::content_type] != "application/json") {
                resp.result(http::status::bad_request);
                resp.set(http::field::content_type, "application/json");
                resp.body() = R"({"error": "Content-Type must be application/json"})";
                return resp;
            }

            RequestParams params = parse_json_body(req.body());

            auto gen = std::make_shared<Generator>();
            Composer composer(params.table_name, params.fields, gen);
            composer.compose(params.rows);

            CsvWriter writer;
            writer.write(composer);

            std::string file_content = writer.str();

            resp.result(http::status::ok);
            resp.set(http::field::content_type, "text/csv");
            resp.set(http::field::content_disposition,
                    "attachment; filename=\"" + params.output_file + "\"");
            resp.body() = std::move(file_content);
        } else {
            resp.result(http::status::not_found);
            resp.set(http::field::content_type, "application/json");
            resp.body() = R"({"error": "Endpoint not found"})";
        }
    } catch (const std::exception& e) {
        resp.result(http::status::internal_server_error);
        resp.set(http::field::content_type, "application/json");
        resp.body() = R"({"error": ")" + std::string(e.what()) + "\"}";
    }

    resp.prepare_payload();

    {
        std::lock_guard<std::mutex> lock(cache_mutex);
        request_cache[cache_key] = {resp};
    }

    return resp;
}