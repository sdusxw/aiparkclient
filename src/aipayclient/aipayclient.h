/*
 * 文件：aipayclient.h
 * 作者：Xiwei Sun
 * 日期：2018年9月4日
 * 描述：重构的人工智能支付客户端程序
 */
#ifndef AI_PAY_CLIENT
#define AI_PAY_CLIENT

#include "common.h"
#include "net_tcp.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <atomic>
#include <jsoncpp/json/json.h>
#include <signal.h>

#define PAY_SVR_IP      ("117.50.44.92")
#define PAY_TCP_PORT    (7666)
#define PAY_HTTP_PORT   (7667)

//全局变量声明
NetTcpClient tcp_client;        //tcp客户端连接，保持唯一，断线重连
Configure configure;            //系统配置

std::atomic_bool ai_online;     //服务器是否联通

//重连线程
void * thread_connect(void *);
//心跳线程
void * thread_heartbeat(void *);
//工作线程
void * thread_work(void *);

//设置pipe信号，保证断网发送不退出
void set_pipe();
//pipe处理函数
void handle_pipe(int sig);
#endif  //AI_PAY_CLIENT
