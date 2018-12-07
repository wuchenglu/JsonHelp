// AegisTcpClientDll.cpp : 定义 DLL 应用程序的导出函数。
//
#include "AegisTcpClientDll.h"
bool IsConnected = false;
HANDLE hEvent;
HANDLE hSendEvent;
// 发送缓冲区
char *sendBuff;
// 接收缓冲区
char *recvBuff;
//客户端
SOCKET clientSocket;
// 是否可以获取数据
bool isCanGetData = false;

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 AegisTcpClientDll.h
CAegisTcpClientDll::CAegisTcpClientDll()
{
	//文件读写
	ofstream ofresult1;
	ofresult1.open("./Log.txt ");
	ofresult1.close();
	receiveData = (char*)malloc(5);		  //free(recvBuff);
	memset(receiveData, 0, 5);
	ip = 0;
	port = 0;
	return;
}

// 开始来链接
bool CAegisTcpClientDll::Start()
{

	//不清除原先的内容
	ofstream ofresult("./Log.txt ", ios::app);
	if (ReadIpTxt())
	{
		ofresult << "读取ip值OK" << "\n\t";
	}
	else
	{
		ofresult << "读取ip值FALSE" << "\n\t";
		return false;
	}

	if (InitMember())
	{
		ofresult << "初始化存储区OK" << "\n\t";
	}
	else
	{
		ofresult << "初始化存储区FALSE" << "\n\t";
		return false;
	}

	if (InitSocket())
	{
		ofresult << "建立连接OK" << "\n\t";
	}
	else
	{
		ofresult << "建立连接FALSE" << "\n\t";
		return false;
	}

	if (startConnect())
	{
		ofresult << "开启线程OK" << "\n\t";
	}
	else
	{
		ofresult << "开启线程FALSE" << "\n\t";
		return false;
	}
	SetEvent(hEvent);
	return true;
}


// 初始化套接字
bool CAegisTcpClientDll::InitSocket()
{
	//不清除原先的内容
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
			else if (r == WSAEISCONN)//套接字原来已经连接！！  
			{
				break;
			}
			else
			{
				//std::cout << "发生错误" << std::endl;
				ofresult << "建立连接FALSE" << "\n\t";
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


// 初始数据
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
// 创建接收和发送数据线程
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

//接收数据线程入口函数
DWORD WINAPI  recvThread(void* param)
{
	//不清除原先的内容
	ofstream ofresult("./Log.txt ", ios::app);
	//std::cout << "接收数据线程！" << std::endl;
	ofresult << "接收数据线程！" << "\n\t";
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
				ofresult3 << "数据接收失败" << "\n\t";
				return false;
			}
		}
		else if (ret == 0)
		{
			isCanGetData = false;
			IsConnected = false;
			ofstream ofresult3("./Log.txt ", ios::app);
			ofresult3 << "目标服务器关闭" << "\n\t";
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
// 发送数据线程入口函数
DWORD WINAPI  sendThread(void* param)
{
	//不清除原先的内容
	ofstream ofresult("./Log.txt ", ios::app);
	ofresult << "发送数据线程" << "\n\t";
	//std::cout << "发送数据线程！" << std::endl;
	while (IsConnected)//是否与服务器连接  
	{
		WaitForSingleObject(hEvent, INFINITE);//等待接收数据线程通知。  
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
				ofresult2 << "发送数据false" << "\n\t";
				break;
			}
		}
		else if (ret == 0)
		{
			isCanGetData = false;
			IsConnected = false;
			ofstream ofresult3("./Log.txt ", ios::app);
			ofresult3 << "目标服务器关闭" << "\n\t";
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
	//不清除原先的内容
	ofstream ofresult("./Log.txt ", ios::app);
	fstream cfgFile;
	cfgFile.open("./ipink.txt");//打开文件      
	if (!cfgFile.is_open())
	{
		//cout << "can not open cfg file!" << endl;
		ofresult << "can not open cfg file=ipink.txt!" << "\n\t";
		return false;
	}
	char tmp[1000];
	while (!cfgFile.eof())//循环读取每一行  
	{
		cfgFile.getline(tmp, 1000);//每行读取前1000个字符，1000个应该足够了  
		string line(tmp);
		size_t pos = line.find('=');//找到每行的“=”号位置，之前是key之后是value  
		if (pos == string::npos) return false;
		string tmpKey = line.substr(0, pos);//取=号之前  
		string value = line.substr(pos + 1);//取=号之后 
		const char *expr = tmpKey.c_str();
		ip = new char[strlen(expr) + 1];
		strcpy_s(ip, strlen(expr) + 1, expr);
		port = (atoi)(value.c_str());
		//cout << port << endl;
		return true;
	}
	return false;

}

// 接收日志
void WriteLog()
{
	ofstream  ofresult("./Log.txt ", ios::app);
	ofresult << "接收:" << recvBuff << "\n\t";
}

//接收日志 fasong 
void WriteLog1()
{
	ofstream  ofresult("./Log.txt ", ios::app);
	ofresult << "发送:" << sendBuff << "\n\t";
}

// 发送
bool CAegisTcpClientDll::SendAegisData(const char * sbuf, const unsigned int len)
{
	ofstream  ofresult("./Log.txt ", ios::app);

	if (IsConnected == false)
	{
		ofresult << "断开连接!=" << sbuf << "\n\t";
		return false;
	}
	if ((sbuf == NULL) || (len > SBUF_LEN))
	{
		ofresult << "输入格式不对!=" << sbuf << "\n\t";
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


// 获取本地ip
bool CAegisTcpClientDll::GetLocalIP()
{
	//1.初始化wsa  
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
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip  
	ip = inet_ntoa(addr);
	WSACleanup();
	return true;
}


// //释放Dll中的内存
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


// 是否可以获取数据
bool CAegisTcpClientDll::IsCanReceiveData()
{
	return isCanGetData;
}
//获取数据
char* CAegisTcpClientDll::GetReviceData()
{
	if (isCanGetData)
	{
		isCanGetData = false;
		memcpy(receiveData, recvBuff, 5);
		return receiveData;
	}
	else
	{
		memset(receiveData, 0, 5);
		return receiveData;
	}
}