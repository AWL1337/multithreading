#pragma once

#include <boost/beast.hpp>
#include "request_params.h"

namespace http = boost::beast::http;

http::response<http::string_body> handle_request(const http::request<http::string_body>& req);
