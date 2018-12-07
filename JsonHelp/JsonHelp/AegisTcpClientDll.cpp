// AegisTcpClientDll.cpp : ���� DLL Ӧ�ó���ĵ���������
#include "stdafx.h"
#include "AegisTcpClientDll.h"
bool IsConnected = false;
HANDLE hEvent;
HANDLE hSendEvent;
// ���ͻ�����
char *sendBuff;
// ���ջ�����
char *recvBuff;
//�ͻ���
SOCKET clientSocket;
// �Ƿ���Ի�ȡ����
bool isCanGetData = false;

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� AegisTcpClientDll.h
CAegisTcpClientDll::CAegisTcpClientDll()
{
	//�ļ���д
	ofstream ofresult1;
	ofresult1.open("./Log.txt ");
	ofresult1.close();
	receiveData = (char*)malloc(1024);		  //free(recvBuff);
	memset(receiveData, 0, 1024);
	ip = 0;
	port = 0;
	return;
}

// ��ʼ������
bool CAegisTcpClientDll::Start()
{

	//�����ԭ�ȵ�����
	ofstream ofresult("./Log.txt ", ios::app);
	if (ReadIpTxt())
	{
		ofresult << "��ȡipֵOK" << "\n\t";
	}
	else
	{
		ofresult << "��ȡipֵFALSE" << "\n\t";
		return false;
	}

	if (InitMember())
	{
		ofresult << "��ʼ���洢��OK" << "\n\t";
	}
	else
	{
		ofresult << "��ʼ���洢��FALSE" << "\n\t";
		return false;
	}

	if (InitSocket())
	{
		ofresult << "��������OK" << "\n\t";
	}
	else
	{
		ofresult << "��������FALSE" << "\n\t";
		return false;
	}

	if (startConnect())
	{
		ofresult << "�����߳�OK" << "\n\t";
	}
	else
	{
		ofresult << "�����߳�FALSE" << "\n\t";
		return false;
	}
	SetEvent(hEvent);
	return true;
}


// ��ʼ���׽���
bool CAegisTcpClientDll::InitSocket()
{
	//�����ԭ�ȵ�����
	ofstream ofresult("./Log.txt ", ios::app);
	WSAData wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		return false;
	}
	unsigned long ul = 1;
	ioctlsocket(clientSocket, FIONBIO, &ul);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	int linkCount = 0;
	while (true)
	{
		if (linkCount > 10)
		{
			return false;
		}
		linkCount++;
		//cout << "StartInk" << endl;
		int ret = connect(clientSocket, (sockaddr*)&addr, sizeof(addr));
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK || r == WSAEINVAL)
			{
				Sleep(20);
				continue;
			}
			else if (r == WSAEISCONN)//�׽���ԭ���Ѿ����ӣ���  
			{
				break;
			}
			else
			{
				//std::cout << "��������" << std::endl;
				ofresult << "��������FALSE" << "\n\t";
				return false;
			}
		}
		if (ret == 0)
		{
			break;
		}
	}
	IsConnected = true;
	return true;
}


// ��ʼ����
bool CAegisTcpClientDll::InitMember()
{
	hSendThread = NULL;
	hRecvThread = NULL;
	hEvent = CreateEvent(NULL, true, false, NULL);
	hSendEvent = CreateEvent(NULL, true, false, NULL);
	IsConnected = false;
	recvBuff = (char*)malloc(1024);		  //free(recvBuff);
	memset(recvBuff, 0, 1024);
	sendBuff = (char*)malloc(1024);
	memset(sendBuff, 0, 1024);
	return true;
}
// �������պͷ��������߳�
bool CAegisTcpClientDll::startConnect()
{
	hRecvThread = CreateThread(NULL, 0, recvThread, NULL, 0, NULL);
	if (hRecvThread == NULL)
	{
		return false;
	}
	hSendThread = CreateThread(NULL, 0, sendThread, NULL, 0, NULL);
	if (hSendThread == NULL)
	{
		return false;
	}
	return true;
}

//���������߳���ں���
DWORD WINAPI  recvThread(void* param)
{
	//�����ԭ�ȵ�����
	ofstream ofresult("./Log.txt ", ios::app);
	//std::cout << "���������̣߳�" << std::endl;
	ofresult << "���������̣߳�" << "\n\t";
	while (IsConnected)
	{
		WaitForSingleObject(hSendEvent, INFINITE);
		//ResetEvent(hSendEvent);
		int ret = recv(clientSocket, recvBuff, 1024, 0);
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				Sleep(10);
				continue;
			}
			else if (r == WSAENETDOWN)
			{
				ofstream ofresult3("./Log.txt ", ios::app);
				ofresult3 << "���ݽ���ʧ��" << "\n\t";
				return false;
			}
		}
		else if (ret == 0)
		{
			isCanGetData = false;
			IsConnected = false;
			ofstream ofresult3("./Log.txt ", ios::app);
			ofresult3 << "Ŀ��������ر�" << "\n\t";
			clientSocket = NULL;
			free(recvBuff);
			free(sendBuff);
			return false;
		}
		else
		{
			isCanGetData = true;
			WriteLog();
		}
	}
	return true;
}
// ���������߳���ں���
DWORD WINAPI  sendThread(void* param)
{
	//�����ԭ�ȵ�����
	ofstream ofresult("./Log.txt ", ios::app);
	ofresult << "���������߳�" << "\n\t";
	//std::cout << "���������̣߳�" << std::endl;
	while (IsConnected)//�Ƿ������������  
	{
		WaitForSingleObject(hEvent, INFINITE);//�ȴ����������߳�֪ͨ��  
		ResetEvent(hEvent);
		int ret = send(clientSocket, sendBuff, 256, 0);
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				Sleep(20);
				continue;
			}
			else
			{
				ofstream ofresult2("./Log.txt ", ios::app);
				ofresult2 << "��������false" << "\n\t";
				break;
			}
		}
		else if (ret == 0)
		{
			isCanGetData = false;
			IsConnected = false;
			ofstream ofresult3("./Log.txt ", ios::app);
			ofresult3 << "Ŀ��������ر�" << "\n\t";
			clientSocket = NULL;
			free(recvBuff);
			free(sendBuff);
			return false;
		}
		else
		{
			SetEvent(hSendEvent);
			WriteLog1();
		}
	}
	return true;
}


bool CAegisTcpClientDll::ReadIpTxt()
{
	//�����ԭ�ȵ�����
	ofstream ofresult("./Log.txt ", ios::app);
	fstream cfgFile;
	cfgFile.open("./ipink.txt");//���ļ�      
	if (!cfgFile.is_open())
	{
		//cout << "can not open cfg file!" << endl;
		ofresult << "can not open cfg file=ipink.txt!" << "\n\t";
		return false;
	}
	char tmp[1000];
	while (!cfgFile.eof())//ѭ����ȡÿһ��  
	{
		cfgFile.getline(tmp, 1000);//ÿ�ж�ȡǰ1000���ַ���1000��Ӧ���㹻��  
		string line(tmp);
		size_t pos = line.find('=');//�ҵ�ÿ�еġ�=����λ�ã�֮ǰ��key֮����value  
		if (pos == string::npos) return false;
		string tmpKey = line.substr(0, pos);//ȡ=��֮ǰ  
		string value = line.substr(pos + 1);//ȡ=��֮�� 
		const char *expr = tmpKey.c_str();
		ip = new char[strlen(expr) + 1];
		strcpy_s(ip, strlen(expr) + 1, expr);
		port = (atoi)(value.c_str());
		//cout << port << endl;
		return true;
	}
	return false;

}

// ������־
void WriteLog()
{
	ofstream  ofresult("./Log.txt ", ios::app);
	ofresult << "����:" << recvBuff << "\n\t";
}

//������־ fasong 
void WriteLog1()
{
	ofstream  ofresult("./Log.txt ", ios::app);
	ofresult << "����:" << sendBuff << "\n\t";
}

// ����
bool CAegisTcpClientDll::SendAegisData(const char * sbuf, const unsigned int len)
{
	ofstream  ofresult("./Log.txt ", ios::app);

	if (IsConnected == false)
	{
		ofresult << "�Ͽ�����!=" << sbuf << "\n\t";
		return false;
	}
	if ((sbuf == NULL) || (len > SBUF_LEN))
	{
		ofresult << "�����ʽ����!=" << sbuf << "\n\t";
		return false;
	}
	memset(sendBuff, 0, 1024);
	memcpy(sendBuff, sbuf, len);
	SetEvent(hEvent);
	return true;
}


CAegisTcpClientDll* AegisTcpClientInterface(void)
{
	return new CAegisTcpClientDll();
}


// ��ȡ����ip
bool CAegisTcpClientDll::GetLocalIP()
{
	//1.��ʼ��wsa  
	WORD wVersionRequested = MAKEWORD(2, 2);

	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return "";

	char local[255] = { 0 };
	gethostname(local, sizeof(local));
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return "";

	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // �������ȡ��һ��ip  
	ip = inet_ntoa(addr);
	WSACleanup();
	return true;
}


// //�ͷ�Dll�е��ڴ�
int CAegisTcpClientDll::FreeDllMemory()
{
	if (IsConnected == true)
	{
		clientSocket = NULL;
		free(recvBuff);
		free(sendBuff);
		free(receiveData);
		return 0;
	}
	return -1;
}


// �Ƿ���Ի�ȡ����
bool CAegisTcpClientDll::IsCanReceiveData()
{
	return isCanGetData;
}
//��ȡ����
char* CAegisTcpClientDll::GetReviceData()
{
	if (isCanGetData)
	{
		isCanGetData = false;
		memcpy(receiveData, recvBuff, 1024);
		return receiveData;
	}
	else
	{
		memset(receiveData, 0, 1024);
		return receiveData;
	}
}