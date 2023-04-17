#include <string>
#include <cstring>
#include "getopt.h"

// This implementation was not written by me. For the sake of windows compatibility,
// I asked an AI language model chatgpt to create it and it is only used in case the
// project was compiled on windows. I did read through the implementation and I understand it.

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

int getopt(int argc, char *argv[], const char *optstring) {
    static int optindex = 0;
    static std::string optarg;
    static const char *optpos = "";

    // If this is the first call to getopt(), initialize the static variables
    if (optindex == 0) {
        optindex = 1;
        optpos = argv[1];
    }

    // If optpos is at the end of the current argument, move to the next one
    if (*optpos == '\0') {
        optindex++;
        optpos = argv[optindex];
    }

    // If optindex is greater than argc, we've processed all the arguments
    if (optindex > argc) {
        return -1;
    }

    // If optpos is not a valid option, return '?'
    if (*optpos != '-') {
        return '?';
    }

    // Move to the next character in the option string
    optpos++;

    // If we've reached the end of the option string, return '-'
    if (*optpos == '\0') {
        return '-';
    }

    // Check if the option is in optstring
    const char *opt = std::strchr(optstring, *optpos);
    if (opt == NULL) {
        return '?';
    }

    // Check if the option takes an argument
    if (*(opt + 1) == ':') {
        // If the argument is attached to the option, use it
        if (*(optpos + 1) != '\0') {
            optarg = optpos + 1;
            optpos += std::strlen(optpos) - 1;
        }
        // Otherwise, the argument is in the next argument
        else if (optindex < argc - 1) {
            optarg = argv[optindex + 1];
            optpos = "";
        }
        // If there is no argument, return '?'
        else {
            return '?';
        }
    }

    // Move to the next argument
    optpos++;

    return *opt;
}
