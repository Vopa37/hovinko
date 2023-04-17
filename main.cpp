#include <iostream>
#include <cstdlib>
// unistd doesn't provide getopt on windows
#ifdef _WIN32
#include "getopt.h"
#else
#include <unistd.h>
#endif
#include <csignal>
#include "client.h"
#include "utils.h"

// has to be global to be able to be accessed in signal handler
Client* client;

void signalHandler(int signum) {
    // close connection and exit on ctrl+c
    client->closeConnection();
    std::exit(0);
}

int main(int argc, char* argv[]) {
    std::string host;
    std::string port;
    std::string mode;

    if (optind < argc && std::string(argv[optind]) == "--help") {
        printHelp();
        exit(0);
    }

    int option;
    // load command line options
    while ((option = getopt(argc, argv, "h:p:m:")) != -1) {
        switch (option) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'm':
                mode = optarg;
                break;
            case '?':
                error(InvalidArguments);
        }
    }

    if (host.empty() || port.empty() || mode.empty()) {
        error(InvalidArguments);
    }

    // initialize connection
    client = new Client(host, stoi(port), mode);
    client->setupConnection();

    signal(SIGINT, signalHandler);

    // read messages from stdin
    std::string message;
    while (std::getline(std::cin, message)) {
        std::string response = client->sendMessage(message);
        // send message and print response
        std::cout << response << std::endl;

        if (response == "BYE") {
            break;
        }
    }

    // close connection in case program was not terminated
    client->closeConnection();

    return 0;
}
