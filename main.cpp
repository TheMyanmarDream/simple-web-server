#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 8080;
const std::string HTML_FILE = "index.html";

std::string readHtmlFile()
{
    std::ifstream file(HTML_FILE);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main()
{
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        std::cerr << "Failed to set socket options\n";
        return 1;
    }

    // Configure address structure
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Failed to bind socket\n";
        return 1;
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Failed to listen\n";
        return 1;
    }

    std::cout << "Server running on port " << PORT << std::endl;

    while (true)
    {
        // Accept incoming connection
        int socket = accept(server_fd, nullptr, nullptr);
        if (socket < 0)
        {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        // Read request
        char buffer[1024] = {0};
        read(socket, buffer, 1024);

        // Prepare response
        std::string html = readHtmlFile();
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Content-Length: " + std::to_string(html.length()) + "\r\n";
        response += "\r\n";
        response += html;

        // Send response
        send(socket, response.c_str(), response.length(), 0);
        close(socket);
    }

    return 0;
}
