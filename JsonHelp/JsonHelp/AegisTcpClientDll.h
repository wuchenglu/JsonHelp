// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 AEGISTCPCLIENTDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// AEGISTCPCLIENTDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifdef AEGISTCPCLIENTDLL_EXPORTS
#define AEGISTCPCLIENTDLL_API __declspec(dllexport)
#else
#define AEGISTCPCLIENTDLL_API __declspec(dllimport)
#endif
#include<iostream>
#include"windows.h"  
#include <iostream>
#include <fstream>
#include <string>
#define SBUF_LEN 1024
#pragma  comment (lib,"wsock32.lib")  
using namespace std;
//接收数据线程入口函数
DWORD WINAPI  recvThread(void* param);
// 接收数据线程入口函数
DWORD WINAPI sendThread(void* param);
// 接收日志
void WriteLog();
void WriteLog1();
// 此类是从 AegisTcpClientDll.dll 导出的
class AEGISTCPCLIENTDLL_API  CAegisTcpClientDll {
private:
	// 接收线程信号量
	HANDLE hRecvThread;
	// 发送线程信号量
	HANDLE hSendThread; 
	// 端口号
	int port=0;
	//ip
    char* ip=0;
	//接收的数据
	char* receiveData;
private:
	// 创建接收和发送数据线程
	bool startConnect();
	// 初始化套接字
	bool InitSocket();
	// 初始数据
	bool InitMember();
	bool ReadIpTxt();
	// 获取本地ip
	bool GetLocalIP();
public:
	CAegisTcpClientDll();
	// TODO:  在此添加您的方法。
	// 开始来链接
	bool Start();
	//获取接收到的数据
    char* GetReviceData();
	// 发送
	bool SendAegisData(const char * sbuf, const unsigned int len);
	
	// //释放Dll中的内存
	int FreeDllMemory();
	// 是否可以获取数据
	bool IsCanReceiveData();

};
extern "C" AEGISTCPCLIENTDLL_API CAegisTcpClientDll* AegisTcpClientInterface();
