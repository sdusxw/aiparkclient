#include <scy/application.h>
#include <scy/net/sslsocket.h>
#include <scy/net/tcpsocket.h>


using std::endl;
using namespace scy;


const uint16_t TcpPort = 7666;


using namespace std;


int main(int argc, char** argv)
{
    
    TCPSocket tcp;
    
    tcp.connect("117.50.44.92", 7666);
    
    std::string msg = "{\"cmd\":\"init_parkid\",\"park_id\":\"0531100015\"}";
    ssize_t n = tcp.send(msg.c_str(), msg.length());
    cout << "sent:\t" << n << endl;
    
    return 0;
}
