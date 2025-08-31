// server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Core.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool isDownloading = false;

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {

	system("cls");
	cout << endl << "Caught signal Ctrl + C, exit the program..." << endl;

	// Terminate program
	exit(signum);
}

// The one and only application object

CWinApp theApp;

int main(int argc, TCHAR* argv[], TCHAR* envp[]) {
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

			server.Create();
			server.Connect(_T("127.0.0.1"), 4567); // Default : 127.0.0.1

			// Register signal and signal handler
			signal(SIGINT, signal_callback_handler);

			// Receive list of downable file from sever
			vector<fZip> downable;

			int num_of_file, len;
			server.Receive(&num_of_file, sizeof(int), 0);

			bool check = true;
			fZip temp;

			for (int i = 0; i < num_of_file; i++) {

				//server.Receive(&temp, sizeof(fZip), 0);

				server.Receive(&len, sizeof(len), 0);
				server.Receive(temp.fName, len + 1, 0);

				server.Receive(&len, sizeof(len), 0);
				server.Receive(temp.fSize, len + 1, 0);

				downable.push_back(temp);
			}

			for (int i = 0; i < downable.size(); i++) {
				cout << downable[i].fName << " " << downable[i].fSize << endl;
			}

			cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -";

			vector<fZip> waiting;

			readInputFile("input.txt", waiting, 0);
			int prev_num = waiting.size(),
				wait_size = downable.size();

			char* dFile;
			int ch;

			do {
				fflush(stdin);
				// read input file
				waiting.clear();
				readInputFile("input.txt", waiting, prev_num);

				for (int i = 0; i < waiting.size(); i++) {
					ch = checkIfDownloaded(downable, waiting[i].fName);

					// if file is not downloaded
					if (ch == 0) {
						len = strlen(waiting[i].fName);
						server.Send(&len, sizeof(len), 0);
						server.Send(waiting[i].fName, len + 1, 0);

						wait_size += 1;
						downFile(server, waiting[i].fName, wait_size);
					}
				}

				prev_num += waiting.size();

			} while (1);

			server.Close();
		}
	}
	else {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}


