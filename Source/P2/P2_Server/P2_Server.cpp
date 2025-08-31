// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Core.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

DWORD WINAPI function_operate(LPVOID arg)
{
	CSocket client;
	HANDLE threadStatus;
	DWORD threadID;

	SOCKET* hConnected = (SOCKET*)arg;
	client.Attach(*hConnected);

	cout << "New client found!" << endl;

	vector<fFile> downable;
	readInputFile("text.txt", downable);
	for (int i = 0; i < downable.size(); i++) {
		fstream fin;
		fin.open(downable[i].fName, ios::in | ios::binary);
		fin.seekg(0, ios::end);
		long long count = long long(fin.tellg());
		fin.close();

		downable[i].curChunk = 0;
		downable[i].maxChunk = count / chunkSize;
		if (count % chunkSize != 0)
		{
			downable[i].maxChunk += 1;
		}
	}

	int num_file = downable.size();
	client.Send(&num_file, sizeof(num_file), 0);

	int len;
	for (int i = 0; i < num_file; i++)
	{
		len = strlen(downable[i].fName);
		client.Send(&len, sizeof(int), 0);
		client.Send(downable[i].fName, len + 1, 0);

		len = strlen(downable[i].fSize);
		client.Send(&len, sizeof(int), 0);
		client.Send(downable[i].fSize, len + 1, 0);

		client.Send(&downable[i].rSize, sizeof(downable[i].rSize), 0);
	}

	int n = 0;
	fFile temp;
	fstream fin;
	int num_of_file;

	// Prepare to read file each 2s
	Read* clientGet = new Read();
	clientGet->file.reserve(10000);
	clientGet->nof = 0;

	bool flag;
	int total_loop = 0;
	int loop_one;

	do {
		n = clientGet->nof;
		total_loop = 0;

		for (int i = 0; i < n; i++)
		{
			if (clientGet->file[i].curChunk < clientGet->file[i].maxChunk) {

				flag = sendChunks(client, clientGet->file[i], loop_one);
				total_loop += loop_one;

				if (!flag) goto Exit;
			}
		}

		if (total_loop == 0)
		{
			total_loop = 1;
		}

		for (int i = 0; i < total_loop; i++)
		{
			if (client.Receive(&num_of_file, sizeof(int), 0) < 0) goto Exit;

			for (int i = 0; i < num_of_file; i++)
			{
				if (client.Receive(&len, sizeof(int), 0) < 0) goto Exit;
				client.Receive(temp.fName, len + 1, 0);

				client.Receive(&temp.rSize, sizeof(long long), 0);
				client.Receive(&temp.priority, sizeof(int), 0);

				fin.open(temp.fName, ios::in | ios::binary);
				fin.seekg(0, ios::end);
				temp.rSize = long long(fin.tellg());
				fin.close();

				temp.curChunk = 0;
				temp.maxChunk = temp.rSize / chunkSize;
				if (temp.rSize % chunkSize != 0)
				{
					temp.maxChunk += 1;
				}

				cout << temp.fName << " " << temp.priority << " " << temp.rSize << " " << temp.maxChunk << endl;

				clientGet->file.push_back(temp);
			}

			clientGet->nof += num_of_file;
		}


	} while (true);

Exit:
	cout << "Client exit." << endl << endl;
	delete clientGet;
	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) {
	hideCursor();
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL) {

		//string a = "abc";
		//int len = a.length();
		//char* b = new char[len + 1];
		//cout << a.c_str() << " " << len << endl;

		//cout << sizeof(b) << endl;

		//strcpy_s(b, sizeof(b), a.c_str());
		//cout << b << endl;

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
			DWORD threadID;
			HANDLE threadStatus;

			cout << "Server listen to client..." << endl;

			SOCKET* hConnected;
			server.Create(4567, SOCK_STREAM, NULL);

			do {
				server.Listen();

				// Change CSocket to Socket (In order to use Thread)
				server.Accept(s);
				hConnected = new SOCKET();
				*hConnected = s.Detach();

				// Khoi tao thread tuong ung voi moi client Connect vao server.
				// Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng

				threadStatus = CreateThread(NULL, 0, function_operate, hConnected, 0, &threadID);

				//s.Attach(*hConnected);
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
