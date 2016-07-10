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

bool Args::parse(int argc, char * argv[])
{
    return true;
}
