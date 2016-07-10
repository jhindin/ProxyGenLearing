#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <poll.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <string>

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
    string progString(prog);
    string::size_type slashPos = progString.rfind('/');
    if (slashPos != string::npos)
        progString = progString.substr(slashPos + 1);

    cerr << "Usage: " << progString << " [-s <secondary terminal>] {-h] <host>[:<port]" << endl;
    cerr << "\t-s <secondary terminal> : opens secondary terminal to control the flow in the main one" << endl;
    cerr << "\t-h : print this message and exit" << endl;
    cerr << "\t<host>[:<port>] server address to connect, port 80 by default" << endl;
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
