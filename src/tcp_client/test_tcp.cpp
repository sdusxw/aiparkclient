#include "common/net_tcp.h"
#include <iostream>


const uint16_t TcpPort = 7666;


using namespace std;


int main(int argc, char** argv)
{
    

    //net::Address server("117.50.44.92", 7666);
    NetTcpClient tcp;
    tcp.connect_server("117.50.44.92", 7666);

    std::string msg = "{\"cmd\":\"init_parkid\",\"park_id\":\"0531100015\"}";
    std::string recv_msg;
    ssize_t n = tcp.send_data(msg, recv_msg);
    cout << "sent:\t" << n << endl;
    cout << msg << endl;
    cout << recv_msg << endl;
    return 0;
}
