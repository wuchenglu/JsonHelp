// JsonHelp.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "json.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <tchar.h>
#include "AegisTcpClientDll.h";
using namespace std;

int ParseJsonFromFile(const char* filename)
{

	// 解析json用Json::Reader     
	Json::Reader reader;
	// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
	Json::Value root;

	std::ifstream is;
	is.open(filename, std::ios::binary);
	string strOne;
	stringstream ss;

	if (is.is_open())
	{
		std::cout << "true" << std::endl;


		while (getline(is, strOne))
		{
			ss << strOne;
		}

	}
	else
	{

		cout << "打开文件失败" << endl;
	}

	if (reader.parse(ss, root))
	{
		std::string code;
		if (!root["files"].isNull())  // 访问节点，Access an object value by name, create a null member if it does not exist. 
		{
			code = root["uploadid"].asString();
			// 访问节点，Return the member named key if it exist, defaultValue otherwise.     
			code = root.get("uploadid", "null").asString();

			// 得到"files"的数组个数     
			int file_size = root["files"].size();
			cout << file_size << endl;
			for (int i = 0; i < file_size; ++i)
			{
				Json::Value val_image = root["files"][i]["images"];
				int image_size = val_image.size();

				for (int j = 0; j < image_size; ++j)
				{
					std::string type = val_image[j]["type"].asString();
					std::string url = val_image[j]["url"].asString();
					cout << type << endl;
					cout << url << endl;
				}
			}

		}
	}
	else
	{

		std::cout << "fasle" << std::endl;
	}
	return 0;
}

int main()
{

	//是用于读取的，用于将字符串转换为 Json::Value 对象的，
	/*Json::Reader reader;
	Json::Value json_object;
	const char* json_document = "{\"age\" : 26,\"name\" : \"123\"}";
	if (!reader.parse(json_document, json_object))
		return 0;
	std::cout << json_object["name"] << std::endl;
	std::cout << json_object["age"] << std::endl;*/


	//Json::Value json_temp;      // 临时对象，供如下代码使用
	//json_temp["name"] = Json::Value("huchao");
	//json_temp["age"] = Json::Value(26);

	//// 表示整个 json 对象
	//Json::Value root;
	//// 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
	//root["key_string"] = Json::Value("value_string");
	//// 新建一个 Key（名为：key_number），赋予数值：12345。
	//root["key_number"] = Json::Value(12345);
	//// 新建一个 Key（名为：key_boolean），赋予bool值：false。							
	//root["key_boolean"] = Json::Value(false);
	//// 新建一个 Key（名为：key_double），赋予 double 值：12.345。							
	//root["key_double"] = Json::Value(12.345);
	//// 新建一个 Key（名为：key_object），赋予 json::Value 对象值。								
	//root["key_object"] = json_temp;
	//// 新建一个 Key（名为：key_array），类型为数组，对第一个元素赋值为字符串："array_string"。								
	//root["key_array"].append("array_string");
	//// 为数组 key_array 赋值，对第二个元素赋值为：1234。
	//root["key_array"].append(1234);
	//// 获得 root 的类型，此处为 objectValue 类型。
	//Json::ValueType type = root.type();
	//Json::FastWriter fast_writer;
	//std::cout << fast_writer.write(root) << std::endl;

	//Json::StyledWriter styled_writer;
	//std::cout << styled_writer.write(root) << std::endl;

	TCHAR szDir[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szDir);
	
	size_t len = wcslen(szDir) + 1;

	size_t converted = 0;

    char *dicPath;

	dicPath = (char*)malloc(len * sizeof(char));

	wcstombs_s(&converted, dicPath, len, szDir, _TRUNCATE);

	std::cout << dicPath << std::endl;
	/*
	*这里时C++的当前文件的写法，上面可以输出当前路径，可以将文件放到对应的文件中
	*/
	const char* filename = ".\\json.conf";
	ParseJsonFromFile(filename);


    /*
    *也可以用如下办法将当前路径和配置文件名链接到一起
    */
	//const char* filename = "\\json.conf";
	//string filename1 =(string) dicPath;
	//string filename2 = (string)filename;
	//string filename3;
	//filename3.append(filename1);
	//filename3.append(filename2);
	//std::cout << filename3 << std::endl;
	//ParseJsonFromFile(filename3.c_str());
	



	CAegisTcpClientDll *tcpClient;
	tcpClient = new CAegisTcpClientDll();
	if (tcpClient->Start())
	{
		std::cout << "连接成功！ \n";

	}
	else
	{
		return -1;
	}
	Json::Reader reader;
	Json::Value json_object;
	char* json_document;
	while (true)
	{   
		//判断是否可以接收数据
		if (tcpClient->IsCanReceiveData())
		{
			//获取接收到的数据
			json_document = tcpClient->GetReviceData();
			std::cout << json_document << std::endl;
			if (reader.parse(json_document, json_object))
			{
				std::cout << json_object["name"] << std::endl;
				std::cout << json_object["age"] << std::endl;
			}
			else
			{
				std::cout << "json解析失败" << std::endl;
			}
		}
	}
	while (true)
	{

	}
	return 0;
}






