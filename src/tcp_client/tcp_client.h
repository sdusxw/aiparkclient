#include <unistd.h>
#include <scy/net/sslsocket.h>
#include <scy/net/tcpsocket.h>
#include <jsoncpp/json/json.h>

using namespace scy;
using namespace scy::net;

using namespace std;

class TcpClient : public SocketAdapter
{
public:
    TCPSocket::Ptr client;

    TcpClient()
        : client(std::make_shared<TCPSocket>())
    {
    }

    ~TcpClient()
    {
        shutdown();
    }

    void start(const std::string& host, uint16_t port)
    {

        client->connect(host, port);
        //client->setKeepAlive(true, 30);
        //client->addReceiver(this);
        sleep(3);
        std::string msg = "{\"cmd\":\"init_parkid\",\"park_id\":\"0531100015\"}";
        ssize_t n = client->send(msg.c_str(), msg.length());
        cout << "sent:\t" << n << endl;
    }

    void shutdown()
    {
        client->close();
    }



    void onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
    {
        cout << "On recv: " << &socket << ": " << buffer.str() << endl;
        cout << "On recv: " << socket.address().host() <<"\t" << socket.address().port() << endl;
        cout << "On recv: peerAddress " << socket.peerAddress().host() <<"\t" << socket.peerAddress().port() << endl;
    }

    void onSocketError(Socket& socket, const Error& error)
    {
        cout << "On error: " << error.message << endl;
    }
};



