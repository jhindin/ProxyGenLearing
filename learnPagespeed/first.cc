#include <iostream>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "net/instaweb/rewriter/public/process_context.h"
#include "pagespeed/kernel/html/html_parse.h"
#include "pagespeed/kernel/html/html_writer_filter.h"
#include "pagespeed/kernel/base/writer.h"
#include "pagespeed/kernel/base/null_message_handler.h"

using namespace std;

class MyWriter : public net_instaweb::Writer {
public:
    virtual bool Write(const StringPiece& str, net_instaweb::MessageHandler* handler) {
        cout << str;
        return true;
    }
    virtual bool Flush(net_instaweb::MessageHandler* message_handler) { return true; }
    virtual bool Dump(net_instaweb::Writer* writer, net_instaweb::MessageHandler* message_handler) { return false; }
};


int main(int argc, char **argv)
{
    net_instaweb::ProcessContext process_context;

    net_instaweb::HtmlParse parser(new net_instaweb::NullMessageHandler());
    net_instaweb::HtmlWriterFilter writerFilter(&parser);
    MyWriter myWriter;
    writerFilter.set_writer(&myWriter);
    parser.AddFilter(&writerFilter);

    if (argc == 0) {
        cerr << "No args" << endl;
        exit(-1);
    }

    struct stat argStat;
    if (stat(argv[1], &argStat)) {
        cerr << argv[1] << ": " << strerror(errno) << endl;
        exit(-1);
    }

    char *content = new char[argStat.st_size];

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        cerr << argv[1] << ": " << strerror(errno) << endl;
        exit(-1);
    }

    int rc = read(fd, content, argStat.st_size);
    if (rc != argStat.st_size) {
        if (rc < 0)
            cerr << argv[1] << ": " << strerror(errno) << endl;
        else
            cerr << "Failed to read all bytes in " << argv[1] << endl;
        exit(-1);
    }

    close(fd);

    if (!parser.StartParse("http://foo.com/bar.html")) {
        cerr << "StartParse failed" << endl;
        exit(-1);
    }

    parser.ParseText(content, argStat.st_size);
    parser.FinishParse();

    return 0;
}
