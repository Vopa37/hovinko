#ifndef UTILS_H
#define UTILS_H

enum ErrorType {
    // error codes
    InvalidArguments = 1,
    NoSuchHostName,
    SocketError,
    SendError,
    RecieveError,
    MessageTooLong,
    ConnectError,
};

// prints error according to error type and exits with given code
void error(ErrorType type);
// validates that a number is a port
bool isPort(int num);
// validates that a string is a connection mode (udp/tcp)
bool isMode(const std::string& str);
// crops string to \n
std::string cropToNewLine(std::string str);
// prints help
void printHelp();

#endif
