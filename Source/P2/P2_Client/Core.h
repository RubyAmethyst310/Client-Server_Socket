#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include <stdio.h>
#include <tchar.h>
#include <afxsock.h>

using namespace std;
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _AFXDLL

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Demo_Client.rc
//

#define IDS_APP_TITLE			103

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE	101
#define _APS_NEXT_COMMAND_VALUE		40001
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		101
#endif
#endif

#include <SDKDDKVer.h>

// #include <unistd.h>
#include <cstdlib>
#include <signal.h>

#define chunkSize 102400

struct fFile {
	char fName[200];
	char fSize[200];
	long long rSize = 0;
	int curChunk = 0, maxChunk = -1, priority = -1;
};

struct Read {
	vector<fFile> file;
	vector<fFile> temp;
	int prev_num = 0; // to skip the previous read lines
};


void GoToXY(int posX, int posY);
void readInputFile(string filename, vector<fFile>& f, int prev_num);

int checkIfDownloaded(vector<fFile> downable, string filename);
//void printPercent(long long size, long long r_size, int length_file, int num_of_file);
//void downChunk(string filename, vector<char> data, int size);
//void downFile(CSocket& client, string filename, int prev_num);

void signal_callback_handler(int signum);
void hideCursor();

fFile splitLine(string s);
int getPriority(string a);
void downChunk(string filename, vector<char> s, int size);
bool downFile(CSocket& client, vector<fFile> fdown, vector<fFile>& list, int wait_size, int& already_down);
void printPercent(fFile file, int num_of_file, int& already_down);
