#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <netdb.h>

#define BUFSIZE 512

class Client {
    // connection config
    std::string host;
    int port;
    std::string mode;
    // connection data
    struct hostent *server;
    struct sockaddr_in serverAddress;
    int clientSocket;
    socklen_t serverLen;
    // indicates whether bye was sent
    bool sentBye;

    static std::vector<uint8_t> constructMessage(const std::string& message);
    static std::string parseResponse(char response[BUFSIZE]);

public:
    Client(std::string host, int port, std::string mode);
    void setupConnection();
    std::string sendMessage(const std::string& message);
    void closeConnection();
};

#endif
