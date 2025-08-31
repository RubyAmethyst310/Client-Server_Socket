#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "Core.h"

using namespace std;

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

void readInputFile(string filename, vector<fZip>& f, int prev_num) {
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

			if (prev_num > 0)
			{
				prev_num--;
			}
			else
			{
				fZip temp = splitString(s);

				if (temp.fName != "")
					f.push_back(temp);
			}
		}
		fin.close();
	}
}

int checkIfDownloaded(vector<fZip> downable, string filename) {
	// return 1 if downloaded
	// return 0 if not downloaded
	// return -1 if not exist in the list of downable
	// return -2 for un

	int check = false;

	for (int i = 0; i < downable.size(); i++) {
		if (filename == downable[i].fName) {
			check = true;
			fstream fin;
			string tmp = "output/" + filename;
			fin.open(tmp.c_str(), ios::in | ios::binary);
			if (fin.is_open()) {
				fin.close();
				return 1;
			}
		}
	}
	if (check == true) return 0;
	else return -1;
}

void printPercent(long long size, long long r_size, int length_file, int num_of_file) {
	double p = (r_size / double(size)) * 100;
	GoToXY(length_file + 17, num_of_file);
	fflush(stdout);
	cout << fixed << setprecision(2) << p << " %                              ";
	fflush(stdout);
}

void downChunk(string filename, vector<char> data, int size) {
	fstream fout;
	fout.open(filename.c_str(), ios::binary | ios::out | ios::app);
	if (!fout.is_open()) {
		cout << "Something wrong!" << endl;
		return;
	}
	else {
		fout.write(data.data(), size);
		fout.close();
	}
}

void downFile(CSocket& client, string filename, int wait_size) {
	bool downloading = true; // check if file is downloading or not
	long long all_size; // size of the downloading file
	client.Receive(&all_size, sizeof(all_size), 0);

	GoToXY(0, wait_size);
	cout << filename << " is downloading:";

	filename = "output/" + filename; // save in "output" folder
	long long size = 0;

	vector<char> chunk(chunkSize); // buffer to save chunk data
	int size_get; // size of one chunk

	while (downloading) {

		size_get = (client.Receive(chunk.data(), chunkSize, 0));

		if (size_get < 0)
		{
			// Failed
			break;
		}

		downChunk(filename, chunk, size_get);
		size += size_get;

		printPercent(all_size, size, filename.length(), wait_size);

		if (size == all_size)
		{
			break;
		}
	}

	GoToXY(filename.length() + 30, wait_size);
	cout << "Done!";
}