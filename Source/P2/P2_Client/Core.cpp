#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "Core.h"

using namespace std;

fFile splitLine(string s) {
	fFile f;
	string word;
	stringstream ss(s);
	int pos = 0, len;

	while (ss >> word) {
		pos++;

		if (pos == 1)
		{
			len = word.length();
			strcpy_s(f.fName, len + 1, word.c_str());
		}
		if (pos == 2)
		{
			f.priority = getPriority(word);
		}
	}

	return f;
}

void readInputFile(string filename, vector<fFile>& f, int prev_num) {
	fstream fin;
	fFile temp;

	fin.open(filename.c_str(), ios::in);
	if (!fin.is_open()) {
		cout << "Cannot open this file. Check if something wrong!" << endl;
		exit(0);
	}
	else {
		while (!fin.eof()) {
			string s;
			getline(fin, s);
			if (prev_num > 0)
			{
				prev_num--;
			}
			else
			{
				if (s == "")
				{
					break;
				}

				temp = splitLine(s);
				f.push_back(temp);
			}
		}

		fin.close();
	}
}

int checkIfDownloaded(vector<fFile> downable, string filename) {
	// return 1 if downloaded
	// return 0 if not downloaded
	// return -1 if not exist in the list of downable
	// return -2 for un

	int check = false;

	for (int i = 0; i < downable.size(); i++)
	{
		string down = downable[i].fName;
		if (filename == down)
		{
			check = true;
			fstream fin;
			string tmp = "output/" + filename;
			fin.open(tmp.c_str(), ios::in | ios::binary);
			if (fin.is_open())
			{
				fin.close();
				return 1;
			}
		}
	}

	if (check == true)
	{
		return 0;
	}

	return -1;
}

// https://codelearn.io/sharing/windowsh-ham-dinh-dang-noi-dung-console
// Go to the position X, Y on the console window
void GoToXY(int posX, int posY)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD Position;
	Position.X = posX;
	Position.Y = posY;

	SetConsoleCursorPosition(hStdout, Position);
}

void hideCursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

int getPriority(string a) {
	if (a == "CRITICAL")
	{
		return 10;
	}
	else if (a == "HIGH")
	{
		return 4;
	}

	return 1;
}

void downChunk(string filename, vector<char> s, int size) {
	fstream fout;

	filename = "output/" + filename;
	fout.open(filename.c_str(), ios::binary | ios::out | ios::app);
	fout.write(s.data(), size);
	fout.close();
}

void printPercent(fFile file, int num_of_file, int& already_down) {
	string file_name = file.fName;
	file_name = "output/" + file_name;
	fstream fin(file_name.c_str(), ios::in | ios::binary);
	fin.seekg(0, ios::end);
	long long r_size = fin.tellg();
	fin.close();

	double p = (r_size / double(file.rSize)) * 100;
	//double p = 1.0 * file.curChunk / file.maxChunk;

	int length = strlen(file.fName);

	if (r_size != file.rSize)
	{
		GoToXY(length + 17, num_of_file);
		cout << fixed << setprecision(2) << p << " %                              ";
	}
	else
	{
		already_down += 1;
		GoToXY(length + 17, num_of_file);
		cout << fixed << setprecision(2) << p << " %                              ";
		GoToXY(length + 30, num_of_file);
		cout << "Done!";
	}
}

bool downFile(CSocket& client, vector<fFile> fdown, vector<fFile>& list, int wait_size, int& already_down) {
	long long chunk_size, recv_size = 0, size = 0;
	bool check = true;
	int name_size;
	int length = 0;

	if (client.Receive(&length, sizeof(int), 0) < 0)
	{
		return false;
	}

	char* file_name = new char[length + 1];
	if (client.Receive(file_name, length + 1, 0));
	if (client.Receive(&chunk_size, sizeof(long long), 0));

	vector<char> oneSending(chunkSize);

	for (int i = fdown.size() - 1; i >= 0; i--)
	{
		if (strcmp(fdown[i].fName, file_name) == 0)
		{
			wait_size += i;
			break;
		}
	}

	int pos = 0;
	for (int i = 0; i < list.size(); i++)
	{
		if (strcmp(list[i].fName, file_name) == 0)
		{
			pos = i;
			break;
		}
	}

	GoToXY(0, wait_size);
	cout << file_name << " is downloading:";

	while (true)
	{
		size = client.Receive(oneSending.data(), chunk_size, 0);
		if (size < 0)
		{
			cout << "" << endl;
			system("pause");
			return false;
		}

		chunk_size -= size;

		downChunk(file_name, oneSending, size);

		if (chunk_size == 0)
		{
			break;
		}
	}

	printPercent(list[pos], wait_size, already_down);
	delete[] file_name;

	return true;
}