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
    //处理PIPE信号，防止断网后程序退出
    set_pipe();
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
                //模拟回应
                Json::Reader reader;
                Json::Value json_object;
                
                if (!reader.parse(msg, json_object))
                {
                    //JSON格式错误导致解析失败
                    log_str = "[json]解析失败";
                    log_output(log_str);
                }
                else
                {
                    //根据park_id来确定转发目标
                    std::string str_cmd = json_object["cmd"].asString();
                    if (str_cmd == "query_pay") {
                        Json::Value json_query_msg;
                        json_query_msg["cmd"] = Json::Value("query_pay");
                        json_query_msg["park_id"] = json_object["park_id"];
                        json_query_msg["palte"] = json_object["plate"];
                        json_query_msg["openid"] = json_object["openid"];
                        if(json_query_msg["palte"].asString()=="鲁AB925E")
                        {
                            json_query_msg["money"] = 10;
                            json_query_msg["intime"] = "2017-07-28  08:00:01";
                        }
                        else
                        {
                            json_query_msg["money"] = 6;
                            json_query_msg["intime"] = "2017-07-28  10:30:05";
                        }
                        json_query_msg["ret"] = "ok";
                        std::string ret_msg = json_query_msg.toStyledString();
                        ssize_t n = tcp_client.send_only(ret_msg);
                    }
                }
            }
        }else{
            usleep(10000);
        }
    }
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
