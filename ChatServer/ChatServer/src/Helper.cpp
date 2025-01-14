#include "Helper.h"

vector<History> allHistory;

const char key = 'K';

// �����ļ�·������
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

	//������������벻һ�£���������
	if (passWord != confirmPass)
	{
		cout << " The passwords entered twice do not match. Please re-enter " << endl;
		goto AddFlag;
	}
	AppendUser(userName, passWord);
	cout << "Add successfully" << endl;
}
//�޸��û�
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
	//�����û��ļ�
	WriteToFile(allUser);
}
//ɾ���û�
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
	//ɾ���û�
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

	//�����û��ļ�
	WriteToFile(allUser);
	cout << "Successfully Delete" << endl;
}
void WriteToFile(vector<User> allUser)
{
	ofstream outFile;	//����ofstream����outFile
	QString filePath = getDataFilePath(USER_INFO_FILE);
	outFile.open(filePath.toStdString(), ios::trunc);	//���ļ� ����д��
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
	ofstream outFile;	//����ofstream����outFile
	QString filePath = getDataFilePath(USER_INFO_FILE);
	outFile.open(filePath.toStdString(), ios::app);	//���ļ� ׷��д
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

		// �������ݷָ�Ϊ IP ��ʱ��
		while (iss >> token)
		{
			tokens.push_back(token);
		}

		// ��� IP Ϊ "0.0.0.0"��������ʾ
		if (!tokens.empty() && tokens[0] == "0.0.0.0")
		{
			continue;
		}

		// ��ӡ��Ч�����Ӽ�¼
		if (tokens.size() >= 2)
		{
			cout << "IP: " << tokens[0] << "  Time: " << tokens[1] << endl;
		}
	}

	file.close();
}


void WirteToHistory(string ip, string connTime)
{
	// ����Ƿ�Ϊ��Ч IP �����������ĵ�ַ
	if (ip == "0.0.0.0" || ip == "127.0.0.1")
	{
		cout << "Skipped writing invalid or server IP: " << ip << endl;
		return;
	}

	ofstream outFile; // ���� ofstream ����
	QString filePath = getDataFilePath(HISTORY_FILE);
	outFile.open(filePath.toStdString(), ios::app); // ׷��д���ļ�
	if (!outFile.is_open())
	{
		printf("Failed to open file: %s\n", filePath.toStdString().c_str());
		return;
	}

	outFile << ip << " " << connTime << endl;
	outFile.close();
}

// ������Ϣ
string encryptMessage(const string& message)
{
	string encrypted = message;
	for (char& c : encrypted)
	{
		c ^= key; // ʹ�� XOR ����
	}
	return encrypted;
}

// ������Ϣ
string decryptMessage(const string& encryptedMessage)
{
	string decrypted = encryptedMessage;
	for (char& c : decrypted)
	{
		c ^= key; // ʹ�� XOR ����
	}
	return decrypted;
}

// ��¼������Ϣ���ļ�
void logChatMessage(const string& userName, const string& message, const string& timestamp)
{
	QString filePath = getDataFilePath(CHAT_HISTORY_FILE);
	ofstream outFile(filePath.toStdString(), ios::app);
	if (!outFile.is_open())
	{
		cout << "Unable to open chat log file" << endl;
		return;
	}

	// ������Ϣ
	string encryptedMessage = encryptMessage(message);

	// ��¼���ܺ����Ϣ
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