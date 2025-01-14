#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

// 定义文件路径常量
extern const QString USER_INFO_FILE;
extern const QString HISTORY_FILE;
extern const QString CHAT_HISTORY_FILE;

struct User
{
	string userName;
	string passWord;
};

struct History
{
	string ip;
	string connTime;
};

vector<User> loadUserInfo();//加载所有用户信息
void addUser();//添加用户
void updateUser();//修改用户
void deleteUser();//删除用户
void WriteToFile(vector<User> allUser);//将用户信息写入文件
void logChatMessage(const string& userName, const string& message, const string& timestamp);
bool AppendUser(string userName, string passWord);
void showChatHistory(); // 查看聊天记录
void showHistory();//客户端连接历史
void WirteToHistory(string ip, string connTime);
void SplitString(const string& s, vector<string>& v, const string& c);

string encryptMessage(const string& message); // 加密消息
string decryptMessage(const string& encryptedMessage); // 解密消息
