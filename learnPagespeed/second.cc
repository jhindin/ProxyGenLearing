#include <iostream>
#include <cstring>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "net/instaweb/rewriter/public/process_context.h"
#include "net/instaweb/rewriter/public/rewrite_driver.h"
#include "net/instaweb/rewriter/public/rewrite_stats.h"
#include "net/instaweb/rewriter/public/url_namer.h"
#include "net/instaweb/rewriter/public/rewrite_stats.h"
#include "pagespeed/kernel/html/html_writer_filter.h"
#include "pagespeed/kernel/base/cache_interface.h"
#include "pagespeed/kernel/base/posix_timer.h"
#include "pagespeed/kernel/base/writer.h"
#include "pagespeed/kernel/base/null_message_handler.h"
#include "pagespeed/kernel/base/null_mutex.h"

#include "pagespeed/system/system_server_context.h"
#include "pagespeed/system/system_rewrite_driver_factory.h"
#include "pagespeed/system/system_rewrite_options.h"

#include "pagespeed/system/system_thread_system.h"

#include "pagespeed/opt/http/request_context.h"

#include "pagespeed/kernel/http/http_options.h"
#include "pagespeed/kernel/http/user_agent_matcher.h"

using namespace std;

class MyWriter : public net_instaweb::Writer {
public:
    bool Write(const StringPiece& str, net_instaweb::MessageHandler* handler) {
        cout << str;
        return true;
    }
    bool Flush(net_instaweb::MessageHandler* message_handler) { return true; }
    bool Dump(net_instaweb::Writer* writer, net_instaweb::MessageHandler* message_handler) { return false; }
};

class ArmeronServerContext;

class ArmeronRewriteDriverFactory : public net_instaweb::RewriteDriverFactory
{
public:
    explicit ArmeronRewriteDriverFactory(const net_instaweb::ProcessContext &processContext, net_instaweb::SystemThreadSystem *threadSystem, net_instaweb::MessageHandler *messageHandler) :
        RewriteDriverFactory(processContext, threadSystem),
        m_messageHandler(messageHandler),
        m_threadSystem(threadSystem)
    {
        InitializeDefaultOptions();
    }

    net_instaweb::MessageHandler *DefaultHtmlParseMessageHandler()  { return m_messageHandler; }
    net_instaweb::MessageHandler *DefaultMessageHandler() { return m_messageHandler; }

    void NonStaticInitStats(net_instaweb::Statistics *statistics)  {
        InitStats(statistics);
    }

    static void InitStats(net_instaweb::Statistics *statistics) {
        net_instaweb::RewriteDriverFactory::InitStats(statistics);
    }

    net_instaweb::ServerContext *NewDecodingServerContext();
    virtual net_instaweb::RewriteOptions *NewRewriteOptions() override;

    void SetupCaches(net_instaweb::ServerContext* server_context) override {}
    bool UseBeaconResultsInFilters() const { return false; }
    net_instaweb::UrlAsyncFetcher* DefaultAsyncUrlFetcher() { return nullptr; }
    net_instaweb::FileSystem* DefaultFileSystem() { return nullptr; }
    net_instaweb::Hasher* NewHasher() { return nullptr; }
    net_instaweb::ServerContext* NewServerContext();
protected:
    net_instaweb::MessageHandler *m_messageHandler;
    net_instaweb::ServerContext *m_serverContext;
    net_instaweb::SystemThreadSystem *m_threadSystem;
};

class ArmeronServerContext : public net_instaweb::ServerContext {
public:
    explicit ArmeronServerContext(ArmeronRewriteDriverFactory *factory) :
        net_instaweb::ServerContext(factory) {}
    bool  ProxiesHtml() const  { return false; }
};

net_instaweb::ServerContext* ArmeronRewriteDriverFactory::NewServerContext()
{
    return new ArmeronServerContext(this);
}

class ArmeronRewriteOptions : public net_instaweb::RewriteOptions
{
public:
    ArmeronRewriteOptions(net_instaweb::ThreadSystem *threadSystem) : RewriteOptions(threadSystem) {
        InitializeOptions(s_armeronProperties);
    }

    static void Initialize();
    static void Terminate();

    static net_instaweb::RewriteOptions::Properties *s_armeronProperties;
};

net_instaweb::RewriteOptions::Properties *ArmeronRewriteOptions::s_armeronProperties = NULL;

void ArmeronRewriteOptions::Initialize() {
    Properties::Initialize(&s_armeronProperties);
    RewriteOptions::Initialize();
}

void ArmeronRewriteOptions::Terminate() {
    RewriteOptions::Terminate();
    Properties::Terminate(&s_armeronProperties);
}

net_instaweb::ServerContext *ArmeronRewriteDriverFactory::NewDecodingServerContext()
{
    return new ArmeronServerContext(this);
}

net_instaweb::RewriteOptions *ArmeronRewriteDriverFactory::NewRewriteOptions()
{
    ArmeronRewriteOptions *options = new ArmeronRewriteOptions(m_threadSystem);
    //    options->Initialize();
    return options;
}


class NullCache : public net_instaweb::CacheInterface {
public:
    void Get(const GoogleString& key, Callback* callback) override {
        ValidateAndReportResult(key, kNotFound, callback);
    }
    void Put(const GoogleString& key, net_instaweb::SharedString* value) override {}
    void Delete(const GoogleString& key) override {}

    GoogleString Name() const override  {
        return "NullCache";
    }

    bool IsBlocking() const override { return false; }
    bool IsHealthy() const override { return true; }
    void ShutDown() override {}
};


int main(int argc, char **argv)
{
    net_instaweb::ProcessContext processContext;

    net_instaweb::NullMessageHandler nullMessageHandler;


    ArmeronRewriteOptions::Initialize();

    net_instaweb::SystemThreadSystem *threadSystem = new net_instaweb::SystemThreadSystem();
    ArmeronRewriteDriverFactory factory(processContext, threadSystem, &nullMessageHandler);
    ArmeronServerContext serverContext(&factory);
    net_instaweb::PosixTimer timer;
    NullCache nullCache;
    net_instaweb::UrlNamer urlNamer;
    net_instaweb::UserAgentMatcher userAgentMatcher;

    serverContext.set_url_namer(&urlNamer);
    serverContext.set_timer(&timer);
    serverContext.set_metadata_cache(&nullCache);
    serverContext.set_user_agent_matcher(&userAgentMatcher);

    net_instaweb::Statistics *globalStatistics = new net_instaweb::NullStatistics();
    net_instaweb::RewriteStats::InitStats(globalStatistics);

    ArmeronRewriteDriverFactory::InitStats(globalStatistics);
    factory.SetStatistics(globalStatistics);
    serverContext.set_statistics(globalStatistics);
    serverContext.InitWorkers();

    net_instaweb::RewriteStats rewriteStats(globalStatistics, threadSystem, &timer);
    serverContext.set_rewrite_stats(&rewriteStats);


    net_instaweb::HttpOptions options;
    net_instaweb::RequestContextPtr req(new net_instaweb::RequestContext(options, new net_instaweb::NullMutex(), nullptr));

    net_instaweb::RewriteDriver *rewriteDriver  = serverContext.NewCustomRewriteDriver(new ArmeronRewriteOptions(threadSystem), req);

    net_instaweb::HtmlWriterFilter writerFilter(rewriteDriver);

    MyWriter myWriter;
    writerFilter.set_writer(&myWriter);
    rewriteDriver->AddFilter(&writerFilter);

    threadSystem->PermitThreadStarting();

    // Read and parse
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

    if (!rewriteDriver->StartParse("http://foo.com/bar.html")) {
        cerr << "StartParse failed" << endl;
        exit(-1);
    }

    rewriteDriver->ParseText(content, argStat.st_size);
    rewriteDriver->FinishParse();

    delete []content;

    ArmeronRewriteOptions::Terminate();
    delete globalStatistics;

    return 0;
}
