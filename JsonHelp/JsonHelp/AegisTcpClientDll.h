// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� AEGISTCPCLIENTDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// AEGISTCPCLIENTDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once
#include<iostream>
#include"windows.h"  
#include <iostream>
#include <fstream>
#include <string>
#define SBUF_LEN 1024
#pragma  comment (lib,"wsock32.lib")  
using namespace std;
//���������߳���ں���
DWORD WINAPI  recvThread(void* param);
// ���������߳���ں���
DWORD WINAPI sendThread(void* param);
// ������־
void WriteLog();
void WriteLog1();
// �����Ǵ� AegisTcpClientDll.dll ������
class  CAegisTcpClientDll {
private:
	// �����߳��ź���
	HANDLE hRecvThread;
	// �����߳��ź���
	HANDLE hSendThread; 
	// �˿ں�
	int port=0;
	//ip
    char* ip=0;
	//���յ�����
	char* receiveData;
private:
	// �������պͷ��������߳�
	bool startConnect();
	// ��ʼ���׽���
	bool InitSocket();
	// ��ʼ����
	bool InitMember();
	bool ReadIpTxt();
	// ��ȡ����ip
	bool GetLocalIP();
public:
	CAegisTcpClientDll();
	// TODO:  �ڴ�������ķ�����
	// ��ʼ������
	bool Start();
	//��ȡ���յ�������
    char* GetReviceData();
	// ����
	bool SendAegisData(const char * sbuf, const unsigned int len);
	
	// //�ͷ�Dll�е��ڴ�
	int FreeDllMemory();
	// �Ƿ���Ի�ȡ����
	bool IsCanReceiveData();

};
