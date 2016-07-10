#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <poll.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

struct Args {
    bool parse(int  argc, char *argv[]);

    string m_extDevice;
    string m_host;
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
    string addrString;

    while ((c=getopt(argc, argv, "s:h")) != -1) {
        switch (c) {
        case 's':
            m_extDevice = optarg;
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

    switch (argc - optind) {
    case 0:
        cerr << "No args" << endl;
        usage(argv[0]);
        return false;
    case 1:
        addrString = argv[optind];
        break;
    default:
        cerr << "Extra args" << endl;
        usage(argv[0]);
        return false;
    }

    cout << "arg is " << addrString << endl;

    string::size_type semicolonPos = addrString.find(':');
    if (semicolonPos == string::npos) {
        m_host = addrString;
        m_port = 80;
    } else {
        m_host = addrString.substr(0, semicolonPos);

        string portString = addrString.substr(semicolonPos + 1);

        string::size_type portParseEnd;

        try {
            m_port = stoi(portString,  &portParseEnd);
        } catch (invalid_argument &ex) {
            cerr << "Invalid integer in port field of " << addrString << endl;
            usage(argv[0]);
            return -1;
        }

        if (portParseEnd != portString.size()) {
            cerr << "Invalid integer in port field of " << addrString << endl;
            usage(argv[0]);
            return -1;
        }
    }


    return true;
}
