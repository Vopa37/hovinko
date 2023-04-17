#include <cstring>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#endif

#include "client.h"
#include "utils.h"

#ifdef _WIN32
#define close_socket closesocket
#else
#define close_socket close
#endif

Client::Client(std::string host, int port, std::string mode) {
    if (isPort(port) && isMode(mode)) {
        this->host = host;
        this->port = port;
        this->mode = mode;
        this->clientSocket = -1;
        this->sentBye = false;
        std::memset(&this->serverAddress, 0, sizeof(this->serverAddress));
    } else {
        error(InvalidArguments);
    }
}

void Client::setupConnection() {
    // get address using DNS
    if ((this->server = gethostbyname(this->host.c_str())) == NULL) {
        error(NoSuchHostName);
    }

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(this->port);
    if (this->mode == "udp") {
        std::memcpy(&this->serverAddress.sin_addr.s_addr, this->server->h_addr, this->server->h_length);
    }
    else {
        bcopy((char*)this->server->h_addr, (char*)&this->serverAddress.sin_addr.s_addr, this->server->h_length);
    }

    // try to create socket
    if ((this->clientSocket = socket(AF_INET, this->mode == "udp" ? SOCK_DGRAM : SOCK_STREAM, 0)) <= 0) {
        error(SocketError);
    }

    // setup connection for TCP
    if (this->mode == "tcp") {
        if (connect(this->clientSocket, (const struct sockaddr* )&this->serverAddress, sizeof(this->serverAddress)) != 0) {
            exit(ConnectError);
        }
    }
}

// transforms string message to correct shape for UDP communication
std::vector<uint8_t> Client::constructMessage(const std::string& message) {
    if (message.length() > 255) {
        error(MessageTooLong);
    }

    std::vector<uint8_t> messageBytes;
    // first byte is 0
    messageBytes.push_back(0x00);
    
    std::stringstream ss;
    ss << std::hex << message.length();

    std::string lengthString = ss.str();
    // prepend 0 if length is 1
    if (lengthString.length() == 1) {
        lengthString = "0" + lengthString;
    }

    uint8_t messageLengthByte = (uint8_t)std::stoi(lengthString, 0, 16);
    // second byte is payload length
    messageBytes.push_back(messageLengthByte);

    // rest of bytes are payload
    for (int i = 0; i < message.length(); i++) {
        messageBytes.push_back((uint8_t)message[i]);
    }

    return messageBytes;
}

// converts UDP response to string
std::string Client::parseResponse(char response[BUFSIZE]) {
    std::vector<uint8_t> payload;
    // get response info
    uint8_t opcode = response[0];
    uint8_t statusCode = response[1];
    uint8_t payloadLength = response[2];

    // opcode 0 would mean request, which triggers an error
    if (opcode == 1) {
        std::string payloadString = statusCode == 0 ? "OK:" : "ERR:";
        for (int i = 3; i < payloadLength + 3; i++) {
            payloadString += response[i];
        }

        // return payload converted to string
        return payloadString;
    }
    else {
        return "ERR:Invalid response.";
    }
}

std::string Client::sendMessage(const std::string& message) {
    this->serverLen = sizeof(this->serverAddress);

    if (this->mode == "udp") {
        auto data = constructMessage(message);

        if (sendto(
            this->clientSocket,
            data.data(),
            data.size(),
            0,
            (struct sockaddr *)&this->serverAddress,
            serverLen
        ) < 0) {
            error(SendError);
        }

        char response[BUFSIZE];

        if (recvfrom(
            this->clientSocket,
            response,
            BUFSIZE,
            0,
            (struct sockaddr *)&this->serverAddress,
            &serverLen
        ) < 0) {
            error(RecieveError);
        }

        return parseResponse(response);
    } else {
        if (send(
            this->clientSocket,
            (message + "\n").c_str(),
            message.length() + 1,
            0
        ) < 0) {
            error(SendError);
        }

        char response[BUFSIZE];
        bzero(response, BUFSIZE);
        if (recv(
            this->clientSocket,
            response,
            BUFSIZE,
            0
        ) < 0) {
            error(RecieveError);
        }

        std::string stringResponse = cropToNewLine(std::string(response));
        if (stringResponse == "BYE") {
            // if response is bye, it means we don't have to send bye anymore
            this->sentBye = true;
            this->closeConnection();
        }

        return stringResponse;
    }
}

void Client::closeConnection() {
    // in case the program was forcefully closed and the communication mode was TCP, send BYE
    if (this->mode == "tcp" && !this->sentBye) {
        std::cout << this->sendMessage("BYE") << std::endl;
    } 
    // check if there is an active connection
    if (this->clientSocket != -1) {
        close_socket(this->clientSocket);
        this->clientSocket = -1;
    }
}
