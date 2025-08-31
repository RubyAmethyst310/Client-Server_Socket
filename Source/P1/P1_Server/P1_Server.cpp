// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Core.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

int main(int argc, TCHAR* argv[], TCHAR* envp[]) {
	hideCursor();
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL) {

		vector<fZip> file;
		readInputFile("text.txt", file);
		for (int i = 0; i < file.size(); i++) {
			cout << file[i].fName << endl;
		}
		int num_file = file.size();

		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0)) {

			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else {

			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			CSocket server, s;
			fZip tmp;
			bool check = false;

			cout << "Server listen to client..." << endl;
			server.Create(4567, SOCK_STREAM, NULL);

			do {
				server.Listen();
				server.Accept(s);
				cout << "New client found!" << endl;

				s.Send(&num_file, sizeof(num_file), 0);

				check = false;
				int len;

				cout << "Sending downloadable file..." << endl << endl;
				for (int i = 0; i < num_file; i++)
				{
					tmp = file[i];
					check = false;

					len = strlen(file[i].fName);
					s.Send(&len, sizeof(int), 0);
					s.Send(file[i].fName, len + 1, 0);

					len = strlen(file[i].fSize);
					s.Send(&len, sizeof(int), 0);
					s.Send(file[i].fSize, len + 1, 0);
				}

				char* dFile;

				do {
					if (s.Receive(&len, sizeof(len), 0) < 0) break;
					dFile = new char[len + 1];

					s.Receive(dFile, len + 1, 0);

					cout << "Sending " << dFile << ". ";

					if (!sendFile(s, dFile)) break;

					delete[]dFile;
					dFile = NULL;

				} while (1);

				cout << "Client exit." << endl << endl;

				s.Close();
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

