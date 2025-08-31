// server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Core.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {

	system("cls");
	GoToXY(0, 0);
	cout << endl << "Caught signal Ctrl + C, exit the program..." << endl;

	// Terminate program
	exit(signum);
}

// The one and only application object

CWinApp theApp;


DWORD WINAPI function_readFile(LPVOID arg)
{
	Read* clientNew = (Read*)arg;
	vector<fFile> temp;
	readInputFile("input.txt", temp, clientNew->prev_num);
	clientNew->prev_num = temp.size();
	int prev_size = 0;

	do {
		readInputFile("input.txt", clientNew->temp, clientNew->prev_num);
		clientNew->prev_num += (clientNew->temp.size() - prev_size);
		prev_size = clientNew->temp.size();

		Sleep(2000);

	} while (true);

	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) {
	int nRetCode = 0;
	hideCursor();
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL) {

		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0)) {

			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else {
			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			CSocket server;
			DWORD threadID;
			HANDLE threadStatus;

			server.Create();
			server.Connect(_T("127.0.0.1"), 4567);

			cout << "Connected successfully!" << endl;

			// Register signal and signal handler
			signal(SIGINT, signal_callback_handler);

			// receive list of downable file from sever
			vector<fFile> downable;
			downable.reserve(10000);

			int num_of_file, len;

			if (server.Receive(&num_of_file, sizeof(int), 0) < 0) return 0;
			fFile temp;
			for (int i = 0; i < num_of_file; i++) {
				server.Receive(&len, sizeof(int), 0);
				server.Receive(temp.fName, len + 1, 0);

				server.Receive(&len, sizeof(int), 0);
				server.Receive(temp.fSize, len + 1, 0);

				server.Receive(&temp.rSize, sizeof(temp.rSize), 0);

				//server.Receive(&temp, sizeof(fFile), 0);
				downable.push_back(temp);
			}

			for (int i = 0; i < downable.size(); i++) {
				cout << downable[i].fName << " " << downable[i].fSize << endl;
			}

			cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;

			// Prepare to read file each 2s
			Read* clientNew = new Read;
			clientNew->file.reserve(10000);
			clientNew->temp.reserve(10000);
			clientNew->prev_num = 0;

			// Doc file 2s 1 lan
			threadStatus = CreateThread(NULL, 0, function_readFile, clientNew, 0, &threadID);

			int prev_num = 0, wait_size = 0, n = 0, already_down = 0;
			wait_size = downable.size() + 2;
			bool flag;
			num_of_file = 0;
			int prev_num_of_file = 0, diff;

			do {
				if (already_down < num_of_file)
				{
					flag = downFile(server, clientNew->file, downable, wait_size, already_down);
					Sleep(2);
				}

				num_of_file = clientNew->temp.size();
				diff = num_of_file - prev_num_of_file;

				if (server.Send(&(diff), sizeof(int), 0) < 0) break;

				for (int i = prev_num_of_file; i < num_of_file; i++)
				{
					len = strlen(clientNew->temp[i].fName);
					server.Send(&len, sizeof(int), 0);
					server.Send(clientNew->temp[i].fName, len + 1, 0);

					server.Send(&clientNew->temp[i].rSize, sizeof(long long), 0);
					server.Send(&clientNew->temp[i].priority, sizeof(int), 0);

					clientNew->file.push_back(clientNew->temp[i]);

					//server.Send(&temp[i], sizeof(fFile), 0);
				}

				prev_num_of_file = num_of_file;


			} while (true);
		}
	}
	else {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}



