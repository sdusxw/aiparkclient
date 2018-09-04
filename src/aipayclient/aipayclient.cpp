/*
 * 文件：aipayclient.cpp
 * 作者：Xiwei Sun
 * 日期：2018年9月4日
 * 描述：重构的人工智能支付客户端程序
 */
#include "aipayclient.h"


int main()
{
    char log_chars[1024];
    std::string log_str;
    //初始化全局变量
    ai_online = false;
    //日志初始化
    log_init();
    //打印系统启动消息
    log_str = "AiPayClient系统启动...版本号V3.14_20180904";
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
    pthread_join(tid_heartbeat, NULL);
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
            }
        }else{
            usleep(10000);
        }
    }
    return NULL;
}
