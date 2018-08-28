#include "tcp_client.h"

#include <scy/application.h>
#include <scy/logger.h>


using std::endl;
using namespace scy;


const uint16_t TcpPort = 7666;


TcpClient tcp_cli;


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Debug));
    Logger::instance().setWriter(new AsyncLogWriter);
    {
        tcp_cli.start_work("117.50.44.92", TcpPort);
        
        std::cout << "TCP Client " << std::endl;
        
        waitForShutdown([&](void*) {
            tcp_cli.shutdown();
        });
    }
    return 0;
}
