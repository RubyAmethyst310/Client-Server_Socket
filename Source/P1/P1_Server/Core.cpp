#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "Core.h"

using namespace std;

void hideCursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

fZip splitString(string s) {
	fZip f;
	string word;
	stringstream ss(s);
	int pos = 0;
	while (ss >> word)
	{
		pos++;

		if (pos == 1)
		{
			strcpy_s(f.fName, word.length() + 1, word.c_str());
		}

		if (pos == 2)
		{
			strcpy_s(f.fSize, word.length() + 1, word.c_str());
		}
	}
	return f;
}

void readInputFile(string filename, vector<fZip>& f) {
	fstream fin;
	fin.open(filename.c_str(), ios::in);
	if (!fin.is_open()) {
		cout << "Cannot open this file. Check if something wrong!" << endl;
		exit(0);
	}
	else {
		while (!fin.eof()) {
			string s;
			getline(fin, s);
			fZip temp = splitString(s);
			bool check = true;
			for (int i = 0; i < f.size(); i++) {
				if (temp.fName == f[i].fName) {
					check = false;
					break;
				}
			}
			if (check) {
				f.push_back(temp);
			}
		}
		fin.close();
	}
}

bool sendFile(CSocket& client, char* filename) {
	fstream fin(filename, ios::in | ios::binary);

	fin.seekg(0, ios::end);
	long long all_size = fin.tellg();
	client.Send(&all_size, sizeof(all_size), 0);
	fin.seekg(0, ios::beg);

	vector<char> file(chunkSize);
	bool check = true;

	int chunk_size;
	while (true)
	{
		chunk_size = min(all_size - fin.tellg(), chunkSize);

		fin.read(file.data(), chunk_size);
		client.Send(file.data(), chunk_size, 0);

		if (all_size - fin.tellg() == 0) break;
	}

	cout << "File has sent. Now closing " << filename << "." << endl;

	fin.close();
	return true;
}

