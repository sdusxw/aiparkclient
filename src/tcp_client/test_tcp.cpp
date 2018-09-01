#include "net_tcp.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

NetTcpClient tcp;

void *heartbeat(void* args)
{
    std::string msg = "{\"cmd\":\"heartbeat\",\"park_id\":\"0531888888\"}";
    while(true)
    {
        sleep(10);
        ssize_t n = tcp.send_only(msg);
        cout << "sent:\t" << n << endl;
    }
    return NULL;
}


int main(int argc, char** argv)
{
    

    //net::Address server("117.50.44.92", 7666);
    tcp.connect_server("117.50.44.92", 7666);

    std::string msg = "{\"cmd\":\"init_parkid\",\"park_id\":\"0531888888\"}";
    std::string recv_msg;
    ssize_t n = tcp.send_only(msg);
    cout << "sent:\t" << n << endl;
    cout << msg << endl;
    
    pthread_t id;
    pthread_create(&id,NULL,heartbeat,NULL);
    pthread_detach(id);
    
    while(1)
    {
        tcp.get_message(recv_msg);
        cout << recv_msg << endl;
        usleep(50000);
    }
    
    return 0;
}
