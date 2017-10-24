/*
   This program demonstrates file mapping, especially how to align a
   view with the system file allocation granularity.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
#include <mutex>
#include <vector>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

#define BUFFSIZE 1024 // size of the memory to examine at any one time

#define FILE_MAP_START 138240 // starting point within the file of
                              // the data to examine (5K)
							  
using namespace std;
string FilePath = "../DataTest/Data/";

int main(int argc, char* argv[])
{
	FilePath.append(argv[1]);
	vector<vector<float>> vvfData; 
	LPSTR lps = const_cast<char *>(FilePath.c_str());
	chrono::time_point<chrono::steady_clock> cstart, cend;
	TCHAR * lpcTheFile = TEXT(lps); // the file to be manipulated
	
	HANDLE hMapFile;      // handle for the file's memory-mapped region
    HANDLE hFile;         // the file handle
	BOOL bFlag;           // a result holder
	DWORD dwFileSize;     // temporary storage for file sizes
	DWORD dwFileMapSize;  // size of the file mapping
	DWORD dwMapViewSize;  // the size of the view
	DWORD dwFileMapStart; // where to start the file map view
	DWORD dwSysGran;      // system allocation granularity
	SYSTEM_INFO SysInfo;  // system information; used to get granularity
	LPVOID lpMapAddress;  // pointer to the base address of the
                        // memory-mapped region
	char * pData;         // pointer to the data
	int i;                // loop counter
	int iData;            // on success contains the first int of data
	int iViewDelta;       // the offset into the view where the data
	
	// Create the test file. Open it "Create Always" to overwrite any
	// existing file. The data is re-created below
	hFile = CreateFile(lpcTheFile,GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		printf(TEXT("hFile is NULL\n"));
		exit(1);
	}
	
	// Get the system allocation granularity.
	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;
	printf("dwSysGran : %d\n", dwSysGran); //65536
	
	// To calculate where to start the file mapping, round down the
	// offset of the data into the file to the nearest multiple of the
	// system allocation granularity.
	dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
	printf (TEXT("The file map view starts at %u bytes into the file.\n"), dwFileMapStart);
	
	// Calculate the size of the file mapping view.
	dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
	printf (TEXT("The file map view is %u bytes large.\n"), dwMapViewSize);
	
	// How large will the file mapping object be?
	dwFileMapSize = FILE_MAP_START + BUFFSIZE;
	printf (TEXT("The file mapping object is %u bytes large.\n"), dwFileMapSize);
	
	// The data of interest isn't at the beginning of the
	// view, so determine how far into the view to set the pointer.
	iViewDelta = FILE_MAP_START - dwFileMapStart;
	printf (TEXT("The data is %d bytes into the view.\n"), iViewDelta);
	
	// Verify that the correct file size was written.
	dwFileSize = GetFileSize(hFile,  NULL);
	printf(TEXT("hFile size: %10d\n"), dwFileSize);
	
	// Create a file mapping object for the file
	// Note that it is a good idea to ensure the file size is not zero
	cstart = chrono::steady_clock::now();
	hMapFile = CreateFileMapping( hFile,          // current file handle
					NULL,           // default security
					PAGE_READWRITE, // read/write permission
					0,              // size of mapping object, high
					dwFileMapSize,  // size of mapping object, low
					NULL);          // name of mapping object

	if (hMapFile == NULL){
		printf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError());
		exit(1);
	}
  
	// Map the view and test the results.

	lpMapAddress = MapViewOfFile(hMapFile,          // handle to
													// mapping object
								FILE_MAP_ALL_ACCESS,// read/write
								0,                  // high-order 32
                                                    // bits of file
													// offset
								dwFileMapStart,     // low-order 32
                                                    // bits of file
                                                    // offset
								dwMapViewSize);     // number of bytes
                                                    // to map
													
	if (lpMapAddress == NULL){
		printf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
		exit(1);
	}
	cend = chrono::steady_clock::now();
	long int Time = chrono::duration_cast<std::chrono::milliseconds> (cend - cstart).count();
	printf("\nTime: %ld ms\n", Time);
	// Calculate the pointer to the data.
	pData = (char *) lpMapAddress;
	
	// Extract the data, an int. Cast the pointer pData from a "pointer
	// to char" to a "pointer to int" to get the whole thing
	iData = *(int *)pData;
	
	printf (TEXT("The value at the pointer is %d,\nwhich %s one quarter of the desired file offset.\n"),
            iData,
            iData*4 == FILE_MAP_START ? TEXT("is") : TEXT("is not"));
	
	char *sub = new char[BUFFSIZE+1];
	memcpy(sub, pData, BUFFSIZE); //copy 
	sub[BUFFSIZE] = 0;
	printf("------------\n%s\n------------", sub);
	// Close the file mapping object and the open file
	
	bFlag = UnmapViewOfFile(lpMapAddress);
	bFlag = CloseHandle(hMapFile); // close the file mapping object
	
	if(!bFlag){
		printf(TEXT("\nError %ld occurred closing the mapping object!"),
        GetLastError());
	}

	bFlag = CloseHandle(hFile);   // close the file itself

	if(!bFlag){
		printf(TEXT("\nError %ld occurred closing the file!"),
        GetLastError());
	}
	return 0;
}
