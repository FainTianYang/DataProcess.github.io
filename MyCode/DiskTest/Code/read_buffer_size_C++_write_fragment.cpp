/*Testing speed of reading file using different size of buffer,
  using filebuf 
  (write fragment testing)
*/
/*./<exe> <檔名> <Buffer Size>*/
/*Buffer Size = 0 : buffer size = file size*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <mutex>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

string strFileName = "../DataTest/Data/";
string strFileNameOut = "../Re_Data/CNew";

unsigned int BUFFER_SIZE = 16;
int alltime = 0;
int SubNum = 0;

int main(int argc, char* argv[])
{
	strFileName.append(argv[1]);
	strFileNameOut.append(argv[1]);
	BUFFER_SIZE = atoi(argv[2]);
	
	/*Read*/
	char* buffer;
	char* bufferCopy;
	ifstream ifs ;
	ofstream ofs;
	chrono::time_point<chrono::steady_clock> start, end;
	
	ofs.open(strFileNameOut.c_str(), ios::out | ios::binary | ios::trunc);
	ifs.open(strFileName.c_str(), ifstream::in | ifstream::binary);

	filebuf* pbuf = ifs.rdbuf();

	size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	
	switch(BUFFER_SIZE){
		case 0: //一次讀整個檔案
			printf("Hold File\n");
			BUFFER_SIZE = size;
			buffer=new char[BUFFER_SIZE];
	
			start = chrono::steady_clock::now();
			
			pbuf->sgetn (buffer,BUFFER_SIZE);
			if(!ifs){
				printf("error");
				exit(1);
			}
			
			ofs.write (buffer, BUFFER_SIZE);
			if(!ofs){
				printf("Write Error!!\n");
			}
			end = chrono::steady_clock::now();
			printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
			
		break;
		
		default:
			buffer=new char[BUFFER_SIZE];
	
			if((unsigned int)size%BUFFER_SIZE == 0) SubNum = (unsigned int)size/BUFFER_SIZE;
			else SubNum = (unsigned int)size/BUFFER_SIZE + 1;
			
			start = chrono::steady_clock::now();
			
			for(int i=0; i<SubNum; i++){
				if(i==SubNum-1) memset(buffer, 0, BUFFER_SIZE);
				pbuf->pubseekpos(i*BUFFER_SIZE);
				pbuf->sgetn (buffer,BUFFER_SIZE);
				/*printf("\ni = %d  :  %s", i, buffer);*/
				if(!ifs){
					printf("error");
					exit(1);
				}
				
				ofs.write (buffer, BUFFER_SIZE);
				if(!ofs){
					printf("Write Error!!\n");
				}
				ofs.flush();
			}
			
			ifs.ignore(std::numeric_limits<std::streamsize>::max());
			end = chrono::steady_clock::now();
			printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
		break;
	}
	ofs.flush();
	ofs.close();
	ifs.close();
	
	/*Read finish*/
	printf("size : %zu\n", size);
	printf("BUFFER_SIZE : %u\n", BUFFER_SIZE);
	printf("SubNum : %d\n", SubNum);
	
    return 0;
}
/*
$ ./rC 1996_1999.csv 524288

File Colse : 83874 ms
size : 1404601726
BUFFER_SIZE : 524288
SubNum : 2680

$ ./Contig64.exe -a C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/Re_Data/CN                                                                                                              ew1996_1999.csv

Contig v1.8 - Contig
Copyright (C) 2001-2016 Mark Russinovich
Sysinternals


Processing C:\Users\chun-hsin\Desktop\C++Project\pthreadTest\Re_Data\CNew1996_19                                                                                                              C:\Users\chun-hsin\Desktop\C++Project\pthreadTest\Re_Data\CNew1996_1999.csv is i                                                                                                              n 15 fragments

Summary:
     Number of files processed:      1
     Number unsuccessfully procesed: 0
     Average fragmentation       : 15 frags/file

	 
------------------------
$ ./rC 1996_1999.csv 0
Hold File

File Colse : 79339 ms
size : 1404601726
BUFFER_SIZE : 1404601726
SubNum : 0

$ cd C:/Users/chun-hsin/Downloads/Contig
ew1996_1999.csve -a C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/Re_Data/CNe

Contig v1.8 - Contig
Copyright (C) 2001-2016 Mark Russinovich
Sysinternals


C:\Users\chun-hsin\Desktop\C++Project\pthreadTest\Re_Data\CNew1996_1999.csv is defragmented

Summary:
     Number of files processed:      1
     Number unsuccessfully procesed: 0
     Average fragmentation       : 1 frags/file

	 
------------------------
./rC 1996_1999.csv 4096

File Colse : 7074 ms
size : 1404601726
BUFFER_SIZE : 4096
SubNum : 342921

Contig v1.8 - Contig
Copyright (C) 2001-2016 Mark Russinovich
Sysinternals


C:\Users\chun-hsin\Desktop\C++Project\pthreadTest\Re_Data\CNew1996_1999.csv is in 196 fragments

Summary:
     Number of files processed:      1
     Number unsuccessfully procesed: 0
     Average fragmentation       : 196 frags/file

*/