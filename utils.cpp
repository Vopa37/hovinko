#include <iostream>
#include "utils.h"

void error(ErrorType type) {
    switch (type) {
        case InvalidArguments:
            std::cerr << "Usage: ipkcpc -h <host> -p <port> -m <mode>" << std::endl;
            break;
        case NoSuchHostName:
            std::cerr << "ERR:No such host." << std::endl;
            break;
        case SocketError:
            std::cerr << "ERR:Socket error." << std::endl;
            break;
        case SendError:
            std::cerr << "ERR:Failed to send data." << std::endl;
            break;
        case RecieveError:
            std::cerr << "ERR:Failed to recieve data." << std::endl;
            break;
        case MessageTooLong:
            std::cerr << "ERR:Message too long." << std::endl;
            return;
        case ConnectError:
            std::cerr << "ERR:Connect error." << std::endl;
            break;
        default:
            break;
    }
    std::exit(type);
}

bool isPort(const int num) {
    return num >= 0 && num <= 9999;
}

bool isMode(const std::string& str) {
    return str == "udp" || str == "tcp";
}

std::string cropToNewLine(std::string str) {
    return str.substr(0, str.find('\n'));
}

void printHelp() {
    std::cout << "NAME:" << std::endl <<
            "\tipkcpc - Client for a calculator over TCP/UDP" << std::endl << std::endl;
        std::cout << "USAGE:" << std::endl << "\tipkcpc [options]" << std::endl << std::endl;
        std::cout << "OPTIONS:" << std::endl << "\t--help\t\tdisplay this help and exit" <<
            std::endl << "\t-h <VALUE>\thost IPv4 address" <<
            std::endl << "\t-m <VARIANT>\tapplication mode to use [choices: tcp, udp]" <<
            std::endl << "\t-p <INT>\tport number on which the calculator is running" <<
            std::endl << std::endl;
        std::cout << "AUTHORS:" << std::endl << "\tMarian Ščupák <xscupa00@stud.fit.vutbr.cz>" << std::endl;
}
