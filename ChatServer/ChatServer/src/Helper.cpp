#include "Helper.h"

vector<History> allHistory;

const char key = 'K';

// 定义文件路径常量
const QString USER_INFO_FILE = "userInfo.txt";
const QString HISTORY_FILE = "history.txt";
const QString CHAT_HISTORY_FILE = "chatHistory.txt";

vector<User> loadUserInfo()
{
	vector<User> allUser;
	QString filePath = getDataFilePath(USER_INFO_FILE);
	ifstream file(filePath.toStdString());
	if (!file.is_open())
	{
		cout << "Failed to open file: " << filePath.toStdString() << endl;
		return allUser;
	}

	string line;
	while (getline(file, line))
	{
		vector<string> tokens;
		istringstream iss(line);
		string token;
		while (iss >> token)
		{
			tokens.push_back(token);
		}
		if (tokens.size() == 0)
			continue;
		User user;
		user.userName = tokens[0];
		user.passWord = tokens[1];
		if (user.userName == "" || user.passWord == "")
			continue;
		allUser.push_back(user);
	}
	file.close();
	return allUser;
}

void addUser()
{
	string userName = "";
	string passWord = "";
	string confirmPass = "";
AddFlag:
	cout << " Enter 'exit' to return to the main menu: " << endl;
	cout << " Please input your userName: " << endl;
	cin >> userName;
	if (userName == "exit")
		return;

	printf("\n");
	cout << " Please input your password: " << endl;
	cin >> passWord;

	if (passWord == "exit")
		return;

	cout << " Please confirm  your password: " << endl;
	cin >> confirmPass;

	if (confirmPass == "exit")
		return;

	//两次输入的密码不一致，重新输入
	if (passWord != confirmPass)
	{
		cout << " The passwords entered twice do not match. Please re-enter " << endl;
		goto AddFlag;
	}
	AppendUser(userName, passWord);
	cout << "Add successfully" << endl;
}
//修改用户
void updateUser()
{
UpdateFlag:
	string userName = "";
	cout << " Enter exit to return to the main menu: " << endl;
	cout << " Please enter the username you want to change: " << endl;
	cin >> userName;
	if (userName == "exit")
		return;
	bool find = false;
	vector<User> allUser = loadUserInfo();
	for (int i = 0; i < allUser.size(); i++)
	{
		if (allUser[i].userName == userName)
		{
		Confirm:
			string passWord = "";
			string confirmPassWord = "";

			cout << " Please enter the new password: " << endl;
			cin >> passWord;

			if (passWord == "exit")
				return;

			cout << " Please confirm your password: " << endl;
			cin >> confirmPassWord;

			if (confirmPassWord == "exit")
				return;

			if (passWord != confirmPassWord)
			{
				cout << "The two passwords are inconsistent. Please re-enter them" << endl;
				goto Confirm;
			}
			allUser[i].passWord = passWord;
			find = true;
			break;
		}
	}
	if (!find)
	{
		cout << "This user was not found" << endl;
		goto UpdateFlag;
	}
	//更新用户文件
	WriteToFile(allUser);
}
//删除用户
void deleteUser()
{
DeleteFlag:
	string userName = "";
	cout << " Enter exit to return to the main menu: " << endl;
	cout << " Please enter the user name you want to delete: " << endl;

	cin >> userName;
	if (userName == "exit")
		return;
	bool find = false;
	vector<User> allUser = loadUserInfo();
	//删除用户
	for (vector<User>::iterator it = allUser.begin(); it != allUser.end(); it++)
	{
		if ((*it).userName == userName)
		{
			it = allUser.erase(it);
			find = true;
			break;
		}
	}
	if (!find)
	{
		cout << "This user was not found" << endl;
		goto DeleteFlag;
	}

	//更新用户文件
	WriteToFile(allUser);
	cout << "Successfully Delete" << endl;
}
void WriteToFile(vector<User> allUser)
{
	ofstream outFile;	//定义ofstream对象outFile
	QString filePath = getDataFilePath(USER_INFO_FILE);
	outFile.open(filePath.toStdString(), ios::trunc);	//打开文件 覆盖写入
	if (outFile.is_open() == false)
	{
		printf("open error!\n");
		return;
	}
	for (int i = 0; i < allUser.size(); i++)
	{
		outFile << allUser[i].userName.c_str() << " " << allUser[i].passWord.c_str() << endl;
	}
	outFile.close();
}
bool AppendUser(string userName, string passWord)
{
	ofstream outFile;	//定义ofstream对象outFile
	QString filePath = getDataFilePath(USER_INFO_FILE);
	outFile.open(filePath.toStdString(), ios::app);	//打开文件 追加写
	if (outFile.is_open() == false)
	{
		printf("open error!\n");
		return false;
	}

	outFile << "\n" << userName << " " << passWord << endl;

	outFile.close();
	return true;
}

void showHistory()
{
	QString filePath = getDataFilePath(HISTORY_FILE);
	ifstream file(filePath.toStdString());
	if (!file.is_open())
	{
		cout << "Cannot open history file: " << filePath.toStdString() << endl;
		return;
	}

	string line;
	while (getline(file, line))
	{
		vector<string> tokens;
		istringstream iss(line);
		string token;

		// 将行内容分割为 IP 和时间
		while (iss >> token)
		{
			tokens.push_back(token);
		}

		// 如果 IP 为 "0.0.0.0"，跳过显示
		if (!tokens.empty() && tokens[0] == "0.0.0.0")
		{
			continue;
		}

		// 打印有效的连接记录
		if (tokens.size() >= 2)
		{
			cout << "IP: " << tokens[0] << "  Time: " << tokens[1] << endl;
		}
	}

	file.close();
}


void WirteToHistory(string ip, string connTime)
{
	// 检查是否为无效 IP 或服务器自身的地址
	if (ip == "0.0.0.0" || ip == "127.0.0.1")
	{
		cout << "Skipped writing invalid or server IP: " << ip << endl;
		return;
	}

	ofstream outFile; // 定义 ofstream 对象
	QString filePath = getDataFilePath(HISTORY_FILE);
	outFile.open(filePath.toStdString(), ios::app); // 追加写入文件
	if (!outFile.is_open())
	{
		printf("Failed to open file: %s\n", filePath.toStdString().c_str());
		return;
	}

	outFile << ip << " " << connTime << endl;
	outFile.close();
}

// 加密消息
string encryptMessage(const string& message)
{
	string encrypted = message;
	for (char& c : encrypted)
	{
		c ^= key; // 使用 XOR 加密
	}
	return encrypted;
}

// 解密消息
string decryptMessage(const string& encryptedMessage)
{
	string decrypted = encryptedMessage;
	for (char& c : decrypted)
	{
		c ^= key; // 使用 XOR 解密
	}
	return decrypted;
}

// 记录聊天信息到文件
void logChatMessage(const string& userName, const string& message, const string& timestamp)
{
	QString filePath = getDataFilePath(CHAT_HISTORY_FILE);
	ofstream outFile(filePath.toStdString(), ios::app);
	if (!outFile.is_open())
	{
		cout << "Unable to open chat log file" << endl;
		return;
	}

	// 加密消息
	string encryptedMessage = encryptMessage(message);

	// 记录加密后的消息
	outFile << "[" << timestamp << "] " << userName << ": " << encryptedMessage << endl;
	outFile.close();
}

void showChatHistory()
{
	ifstream file("./chatHistory.txt");
	if (!file.is_open())
	{
		cout << "Unable to open chat log file" << endl;
		return;
	}
	string line;
	while (getline(file, line))
	{
		cout << line << endl;
	}
	file.close();
}


void SplitString(const string& s, vector<string>& v, const string& c)
{
	v.clear();
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

/*
* The following code is used to decrypt the message on the server side:
* 
void viewDecryptedChatHistory()
{
	ifstream file("./chatHistory.txt");
	if (!file.is_open())
	{
		cout << "Unable to open chat log file" << endl;
		return;
	}

	string line;
	while (getline(file, line))
	{
		// Extract username and encrypted content
		size_t pos = line.find(": ");
		if (pos != string::npos)
		{
			string userNameAndTimestamp = line.substr(0, pos);
			string encryptedMessage = line.substr(pos + 2);

			// Decrypt message
			string decryptedMessage = decryptMessage(encryptedMessage);

			// Displays the decrypted content
			cout << userNameAndTimestamp << ": " << decryptedMessage << endl;
		}
		else
		{
			cout << line << endl; 
		}
	}

	file.close();
}
*/