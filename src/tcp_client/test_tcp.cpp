#include "net_tcp.h"
#include <iostream>
#include <unistd.h>

const uint16_t TcpPort = 7666;


using namespace std;


int main(int argc, char** argv)
{
    

    //net::Address server("117.50.44.92", 7666);
    NetTcpClient tcp;
    tcp.connect_server("117.50.44.92", 7666);

    std::string msg = "{\"cmd\":\"init_parkid\",\"park_id\":\"0531100015\"}";
    std::string recv_msg;
    ssize_t n = tcp.send_only(msg);
    cout << "sent:\t" << n << endl;
    cout << msg << endl;
    while(1)
    {
        tcp.get_message(recv_msg);
        cout << recv_msg << endl;
        usleep(50000);
    }
    
    return 0;
}
