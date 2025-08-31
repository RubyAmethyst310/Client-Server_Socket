#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "Core.h"

using namespace std;

int getPriority(string a)
{
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

fFile splitString(string s) {
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
			len = word.length();
			strcpy_s(f.fSize, len + 1, word.c_str());
		}
	}

	return f;
}

void readInputFile(string filename, vector<fFile>& f) {
	fstream fin;
	fin.open(filename.c_str(), ios::in);
	if (!fin.is_open()) {
		cout << "Cannot open this file. Check if something wrong!" << endl;
	}
	else {
		while (!fin.eof()) {
			string s;
			getline(fin, s);
			fFile temp = splitString(s);

			fstream file(temp.fName, ios::in | ios::binary);
			file.seekg(0, ios::end);
			temp.rSize = long long(file.tellg());
			file.close();
			bool check = true;

			for (int i = 0; i < f.size(); i++)
			{
				if (strcmp(temp.fName, f[i].fName) == 0)
				{
					check = false;
					break;
				}
			}

			if (check)
			{
				f.push_back(temp);
			}
		}

		fin.close();
	}
}

bool sendChunks(CSocket& client, fFile& file, int& l) {
	fstream fin(file.fName, ios::in | ios::binary);
	int loop = file.priority;

	fin.seekg(chunkSize * file.curChunk, ios::beg);
	vector<char> data(chunkSize);
	bool check = true;
	long long chunk_size;
	int len;
	l = 0;

	for (int i = 0; i < loop; i++)
	{
		data.clear();
		check = false;
		if (!fin.good())
		{
			return false;
		}

		chunk_size = min(file.rSize - fin.tellg(), chunkSize);
		fin.read(data.data(), chunk_size);

		file.curChunk += 1;
		l++;

		len = strlen(file.fName);
		client.Send(&len, sizeof(int), 0);
		client.Send(file.fName, len + 1, 0);
		client.Send(&chunk_size, sizeof(long long), 0);
		client.Send(data.data(), chunk_size, 0);

		if (file.curChunk == file.maxChunk)
		{
			break;
		}
	}

	fin.close();
	return true;
}

void hideCursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}