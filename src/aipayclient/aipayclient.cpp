/*
 * 文件：aipayclient.cpp
 * 作者：Xiwei Sun
 * 日期：2018年9月4日
 * 描述：重构的人工智能支付客户端程序
 */
#include "aipayclient.h"

typedef struct
{
    int msg_len;
    char msg[1024];
}sem_msg, *p_sem_msg;

int main()
{
    char log_chars[1024];
    std::string log_str;
    //初始化全局变量
    ai_online = false;
    configure.server_ip="117.50.44.92";
    configure.server_port=7666;
    configure.park_id="0531000008";
    configure.box_ip="127.0.0.1";
    //处理PIPE信号，防止断网后程序退出
    set_pipe();
    //日志初始化
    log_init();
    //检测是否有进程存在，防止重复启动
    if(is_have_instance()){
        log_str = "检测到AiPayClinet进程存在，本程序退出";
        log_output(log_str);
        return -1;
    }
    //打印系统启动消息
    log_str = "AiPayClient系统启动...版本号V3.141_20190317";
    log_output(log_str);
    //读取配置文件
    if(read_config(configure))
    {
        log_str = "读取配置成功";
        log_output(log_str);
    }else{
        log_str = "读取配置失败";
        log_output(log_str);
    }
    //输出系统参数
    sprintf(log_chars, "系统配置：\nServer IP:\t%s\nServer Port:\t%d\nPark ID:\t%s", configure.server_ip.c_str(), configure.server_port, configure.park_id.c_str());
    log_str = log_chars;
    log_output(log_str);
    //启动重连线程
    pthread_t tid_connect;
    pthread_create(&tid_connect,NULL,thread_connect,NULL);
    //启动心跳线程
    pthread_t tid_heartbeat;
    pthread_create(&tid_heartbeat,NULL,thread_heartbeat,NULL);
    //启动工作线程
    pthread_t tid_work;
    pthread_create(&tid_work,NULL,thread_work,NULL);
    //join以上线程
    pthread_join(tid_connect, NULL);
    pthread_join(tid_heartbeat, NULL);
    pthread_join(tid_work, NULL);
}

//重连线程
void * thread_connect(void *)
{
    //char log_chars[1024];
    std::string log_str;
    while(true)
    {
        if (ai_online) {
            sleep(10);
        }else{
            tcp_client.dis_connect();
            if(tcp_client.connect_server(configure.server_ip, configure.server_port))
            {
                ai_online = true;
                log_str = "连接服务器成功";
                log_output(log_str);
                //发送初始化命令
                Json::Value json_init_msg;
                json_init_msg["cmd"] = Json::Value("init_parkid");
                json_init_msg["park_id"] = Json::Value(configure.park_id);
                std::string msg_init = json_init_msg.toStyledString();
                ssize_t n = tcp_client.send_only(msg_init);
                log_str = "发送初始化消息：";
                log_str += msg_init;
                log_output(log_str);
            }else{
                ai_online = false;
                log_str = "连接服务器失败";
                log_output(log_str);
                //延时10秒后重新连接
                sleep(10);
            }
        }
    }
    return NULL;
}
//心跳线程
void * thread_heartbeat(void *)
{
    std::string log_str;
    while (true) {
        if (ai_online) {    //在线才发送心跳
            Json::Value json_hb_msg;
            json_hb_msg["cmd"] = Json::Value("heartbeat");
            json_hb_msg["park_id"] = Json::Value(configure.park_id);
            std::string msg_hb = json_hb_msg.toStyledString();
            ssize_t n = tcp_client.send_only(msg_hb);
            log_str = "发送心跳消息：";
            log_str += msg_hb;
            log_output(log_str);
            if(n<0)         //发送心跳失败表示服务器离线
            {
                ai_online = false;
                log_str = "发送心跳失败";
                log_output(log_str);
            }
        }
        sleep(10);
    }
    return NULL;
}
//工作线程
void * thread_work(void *)
{
    std::string log_str;
    while (true) {
        if (ai_online) {    //在线才接收消息
            std::string msg;
            if (tcp_client.get_message(msg)>0) {    //收到消息长度大于0
                log_str = "收到服务器消息: ";
                log_str += msg;
                log_output(log_str);
                
                p_sem_msg p_msg = (p_sem_msg)malloc(sizeof(sem_msg));
                memcpy(p_msg->msg, msg.c_str(), msg.length());
                p_msg->msg_len = msg.length();
                
                //创建新线程，处理消息
                pthread_t pid_msg;
                pthread_create(&pid_msg,NULL,thread_msg,(void*)p_msg);
                pthread_detach(pid_msg);

                
            }
        }else{
            usleep(10000);
        }
    }
    return NULL;
}

//消息处理线程
void * thread_msg(void * para)
{
    std::string log_str;
    p_sem_msg p_msg = (p_sem_msg)para;
    Json::Reader reader;
    Json::Value json_object;
    
    std::string msg = std::string(p_msg->msg, p_msg->msg_len);
    
    if (!reader.parse(msg, json_object))
    {
        //JSON格式错误导致解析失败
        log_str = "[json]解析失败";
        log_output(log_str);
    }
    else
    {
        std::string str_cmd = json_object["cmd"].asString();
        if (str_cmd == "heartbeat") {
            return NULL;
        }
         std::string str_box_ip = json_object["box_ip"].asString();
         if (str_box_ip.length()==0) //发送到指定的岗亭IP
         {
             str_box_ip = configure.box_ip;
         }
        std::cout << "box_ip:\t" << str_box_ip << std::endl;
        NetTcpClient tcp_cli;
        if(tcp_cli.connect_server(str_box_ip, 6000))
        {
            //连接指定岗亭的tcp端口6000成功，则发送消息
            std::string recv_msg;
            tcp_cli.send_data(msg, recv_msg);
            if(recv_msg.length()>0)
            {
                //有返回消息
                ssize_t n = tcp_client.send_only(recv_msg);
                std::cout << "recv_msg  " << recv_msg << std::endl;
            }
            tcp_cli.dis_connect();
        }
    }
    if(p_msg) {free(p_msg); p_msg=NULL;}
    return NULL;
}

//设置pipe信号，保证断网发送不退出
void set_pipe()
{
    struct sigaction action;
    
    action.sa_handler = handle_pipe;
    
    sigemptyset(&action.sa_mask);
    
    action.sa_flags = 0;
    
    sigaction(SIGPIPE, &action, NULL);
}
//pipe处理函数
void handle_pipe(int sig)
{
    ai_online = false;
    std::string log_str = "服务器断线，开启重连线程";
    log_output(log_str);
}
