#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

// �����ļ�·������
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

vector<User> loadUserInfo();//���������û���Ϣ
void addUser();//����û�
void updateUser();//�޸��û�
void deleteUser();//ɾ���û�
void WriteToFile(vector<User> allUser);//���û���Ϣд���ļ�
void logChatMessage(const string& userName, const string& message, const string& timestamp);
bool AppendUser(string userName, string passWord);
void showChatHistory(); // �鿴�����¼
void showHistory();//�ͻ���������ʷ
void WirteToHistory(string ip, string connTime);
void SplitString(const string& s, vector<string>& v, const string& c);

string encryptMessage(const string& message); // ������Ϣ
string decryptMessage(const string& encryptedMessage); // ������Ϣ
