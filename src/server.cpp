#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unordered_map>
#include <chrono>
#include <arpa/inet.h>

#include "json.hpp"
using json = nlohmann::json;


   //Rate limiting structures

struct RateLimit {
    int tokens;
    std::chrono::steady_clock::time_point last_refill;
};

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Atlas listening on port 8080...\n";

    static int next_user_id = 3;

   
      // Rate limit state
   
    std::unordered_map<std::string, RateLimit> rate_limits;
    const int MAX_TOKENS = 5;
    const int REFILL_SECONDS = 10;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        std::string client_ip = inet_ntoa(client_addr.sin_addr);

        
          // Read request
        
        char buffer[4096] = {0};
        read(client_fd, buffer, sizeof(buffer));
        std::string request(buffer);

        
          // Parse request line
       
        size_t line_end = request.find("\r\n");
        if (line_end == std::string::npos) {
            close(client_fd);
            continue;
        }

        std::string request_line = request.substr(0, line_end);
        std::istringstream iss(request_line);

        std::string method, path, version;
        iss >> method >> path >> version;

        std::cout << client_ip << " → " << method << " " << path << "\n";

        
          // Parse headers
       
        std::unordered_map<std::string, std::string> headers;
        std::istringstream header_stream(request);
        std::string line;

        std::getline(header_stream, line);

        while (std::getline(header_stream, line) && line != "\r") {
            size_t colon = line.find(":");
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                if (!value.empty() && value[0] == ' ') {
                    value.erase(0, 1);
                }
                headers[key] = value;
            }
        }

        
           //Extract body
        
        std::string body_data;
        size_t body_pos = request.find("\r\n\r\n");
        if (body_pos != std::string::npos) {
            body_data = request.substr(body_pos + 4);
        }

       
          // Initialize response
       
        std::string response_body;
        std::string status = "200 OK";
        std::string content_type = "text/plain";

        
          // Authentication gate
       
        const std::string VALID_TOKEN = "secret-token-123";
        bool authenticated = false;

        if (headers.count("Authorization")) {
            if (headers["Authorization"] == "Bearer " + VALID_TOKEN) {
                authenticated = true;
            }
        }

        bool needs_auth = (path == "/users/me");

        if (needs_auth && !authenticated) {
            status = "401 Unauthorized";
            response_body = R"({"error":"unauthorized"})";
            content_type = "application/json";
        }

        
          // Rate limiting gate
       
        if (status == "200 OK") {
            auto now = std::chrono::steady_clock::now();
            auto& rl = rate_limits[client_ip];

            if (rl.tokens == 0) {
                rl.tokens = MAX_TOKENS;
                rl.last_refill = now;
            }

            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - rl.last_refill).count();

            if (elapsed >= REFILL_SECONDS) {
                rl.tokens = MAX_TOKENS;
                rl.last_refill = now;
            }

            if (rl.tokens <= 0) {
                status = "429 Too Many Requests";
                response_body = R"({"error":"rate limit exceeded"})";
                content_type = "application/json";
            } else {
                rl.tokens--;
            }
        }

        
          // Routing + API logic
        
        if (status == "200 OK") {

            if (method == "GET" && path == "/health") {
                response_body = "OK";
            }

            else if (method == "GET" && path == "/users") {
                json users = json::array({
                    { {"id", 1}, {"name", "Kailash"} },
                    { {"id", 2}, {"name", "Alex"} }
                });

                response_body = users.dump();
                content_type = "application/json";
            }

            else if (method == "GET" && path == "/users/me") {
                json me = {
                    {"id", 1},
                    {"name", "Kailash"}
                };

                response_body = me.dump();
                content_type = "application/json";
            }

            else if (method == "POST" && path == "/users") {
                try {
                    auto parsed = json::parse(body_data);

                    if (!parsed.contains("name") || !parsed["name"].is_string()) {
                        status = "422 Unprocessable Entity";
                        response_body = R"({"error":"name is required"})";
                        content_type = "application/json";
                    } else {
                        json user = {
                            {"id", next_user_id++},
                            {"name", parsed["name"]}
                        };

                        response_body = user.dump();
                        status = "201 Created";
                        content_type = "application/json";
                    }
                }
                catch (...) {
                    status = "400 Bad Request";
                    response_body = R"({"error":"invalid JSON"})";
                    content_type = "application/json";
                }
            }

            else {
                status = "404 Not Found";
                response_body = "Not Found";
            }
        }

        
         //  Build HTTP response
       
        std::string response =
            "HTTP/1.1 " + status + "\r\n"
            "Content-Type: " + content_type + "\r\n"
            "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            response_body;

        write(client_fd, response.c_str(), response.size());
        close(client_fd);
    }
}
