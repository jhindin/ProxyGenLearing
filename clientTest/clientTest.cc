#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <poll.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>

using namespace std;

struct Args {
    bool parse(int  argc, char *argv[]);

    const char *m_extDevice;
    const char *m_host;
    uint16_t m_port;
};


int main(int argc, char *argv[])
{
    Args args;

    if (!args.parse(argc, argv))
        return -1;

    cout << "Hello, world!" << endl;
}

void usage(const char *prog)
{
    cerr << "Usage" << endl;
}

bool Args::parse(int argc, char * argv[])
{
    int c;
    char *secondary = nullptr;

    while ((c=getopt(argc, argv, "s:h")) != -1) {
        switch (c) {
        case 's':
            secondary = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return false;
        case '?':
            usage(argv[0]);
            return false;
        default:
            cerr << "Unexpected getopt state " << static_cast<char>(c) << endl;
            usage(argv[0]);
            break;
        }
    }

    cout << "Secondary set to " << (secondary == nullptr ? "nullptr" : secondary) << endl;
    return true;
}
