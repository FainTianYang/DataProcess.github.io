#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <memory>
#include <vector>
#include <windows.h>  

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

#define COLUMN_WIDTH 7
#define COLUMN_NUMBER 14
#define LINE_STEP 1

enum StatusKind{Exit, ReLoad};

class Data{
public:
	string strFilePath;
	vector<vector<float>> vvfContent;
	
	Data(string str){
		strFilePath = str;
		Read();
		printf("Reading File Finish!\n\n");
		printf("File path : %s\n", strFilePath.c_str());
		printf("File size %zu(bytes) = %f(MB)\n", lliFileSize, lliFileSize/(1024.0*1024.0));
		printf("Size of vector : %zu\n\n", vvfContent.size());
		printf("(Reload : 1, Exit : 0)\nPlease enter next order : \n");
	}
	~Data(){
		printf("~Data -- %s \n", this->strFilePath.c_str());
	}
	void ReLoad();
private:
	size_t lliFileSize = 0;
	int iVectorSize = 0;
	void Read();
};
void Data::ReLoad(){
	chrono::time_point<chrono::steady_clock> start, end;
	ifstream ifs;
	ifs.open(strFilePath.c_str(), ifstream::in);
	if(!ifs){
		printf("File Open Error !\n");
		exit(1);
	}
	filebuf* pbuf = ifs.rdbuf();
	
	size_t lliFileSizeNew = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos(0,ifs.in);
	
	if(lliFileSizeNew == lliFileSize){
		printf("The size of file does not change!!\n");
		printf("File size %zu(bytes) = %f(MB)\n", lliFileSizeNew, lliFileSizeNew/(1024.0*1024.0));
	}
	else if(lliFileSizeNew < lliFileSize){
		printf("The size of file reduce!!\n");
		printf("File size %zu(bytes) = %f(MB)\n", lliFileSizeNew, lliFileSizeNew/(1024.0*1024.0));
	}
	else{
		int iBufferSize = lliFileSizeNew - lliFileSize + 1;
		char* buffer = new char[iBufferSize];
		start = chrono::steady_clock::now();
		pbuf->pubseekpos(lliFileSize);
		pbuf->sgetn(buffer,iBufferSize);
		if(!ifs){
			printf("error");
			exit(1);
		}
		buffer[iBufferSize] = 0;
		end = chrono::steady_clock::now();
		printf("\n(ReLoad)Reading data time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
		/*----- Split Input Data(vcharBuffer)-----*/
		start = chrono::steady_clock::now();
		int pos=0;
		char *sub = new char[COLUMN_WIDTH+1];
		vector<float> v(COLUMN_NUMBER);
		char *pch = strtok(buffer,"\n");
		while(pch){
			//cout<<pch<<endl;
			pos=0;
			for(int i=0;i<COLUMN_NUMBER;i++){
				memcpy(sub, pch+pos, COLUMN_WIDTH);
				pos+=COLUMN_WIDTH;
				sub[COLUMN_WIDTH] = 0;
				v[i] = atof(sub);
			}
			vvfContent.push_back(v);
			pch = strtok(NULL,"\n");
		}
		end = chrono::steady_clock::now();
		printf("(ReLoad)Spliting data time: %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());\
		
		ifs.close();
		delete[] buffer;
		
		lliFileSize = lliFileSizeNew;
		
		printf("New size of file : %zu\n", lliFileSize);
		printf("new size of vector : %zu\n\n", vvfContent.size());
		
		for(int j=iVectorSize; j<iVectorSize+20; j++){
			cout<<"Ind : "<<j<<"~~";
			for(int m=0; m<vvfContent[j].size(); m++){
				cout<<vvfContent[j][m]<<" , ";
			}
			cout<<endl;
		}
		
		iVectorSize = vvfContent.size();
	}
}
void Data::Read(){
	chrono::time_point<chrono::steady_clock> start, end;
	
	/*----- Read file -----*/
	ifstream ifs;
	ifs.open(strFilePath.c_str(), ifstream::in);
	if(!ifs){
		printf("File Open Error !\n");
		exit(1);
	}
	filebuf* pbuf = ifs.rdbuf();
	
	lliFileSize = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos(0,ifs.in);
	
	char* buffer = new char[lliFileSize+1];
	
	start = chrono::steady_clock::now();
	pbuf->sgetn(buffer, lliFileSize);
	
	
	/*----- Split Input Data(vcharBuffer)-----*/
	start = chrono::steady_clock::now();
	int pos=0;
	char *sub = new char[COLUMN_WIDTH+1];
	vector<float> v(COLUMN_NUMBER);
	char *pch = strtok(buffer,"\n");
	while(pch){
		//cout<<pch<<endl;
		pos=0;
		for(int i=0;i<COLUMN_NUMBER;i++){
			memcpy(sub, pch+pos, COLUMN_WIDTH);
			pos+=COLUMN_WIDTH;
			sub[COLUMN_WIDTH] = 0;
			v[i] = atof(sub);
		}
		vvfContent.push_back(v);
		pch = strtok(NULL,"\n");
	}
	ifs.close();
	end = chrono::steady_clock::now();
	printf("Spliting data time: %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	delete[] buffer;
	
	iVectorSize = vvfContent.size();
}

int main(){
	//string strFilePath = "./1999Fix.txt";
	string strFilePath = "./1999Fix.txt";
	unique_ptr<Data> dataptr2008txt(new Data(strFilePath.c_str()));
	int iUserInput = -1;
	while(1){
		scanf("%d", &iUserInput);
		switch(iUserInput){
			case 0:
				printf("--------------- Choosing 0! Program finish! ---------------\n");
				exit(1);
			break;
			case 1:
				printf("--------------- Choosing 1! Reload file! ---------------\n");
				dataptr2008txt->ReLoad();
			break;
		}
		printf("\n(Reload : 1, Exit : 0)\nPlease enter next order : \n\n");
	}
	return 0;
}

/*
add 2008Fixt
$ ./rl
Spliting data time: 12180 ms
Reading File Finish!

File path : ./1999Fix.txt
File size 858687291(bytes) = 818.908015(MB)
Size of vector : 8673609

(Reload : 1, Exit : 0)
Please enter next order :
1
--------------- Choosing 1! Reload file! ---------------
The size of file does not change!!
File size 858687291(bytes) = 818.908015(MB)

(Reload : 1, Exit : 0)
Please enter next order :

1
--------------- Choosing 1! Reload file! ---------------

(ReLoad)Reading data time : 67 ms
(ReLoad)Spliting data time: 1191 ms
New size of file : 962496216
new size of vector : 9722184

Ind : 8673609~~1 , 3 , 4 , 2003 , 1955 , 2211 , 2225 , 335 , 128 , 150 , 116 , -14 , 8 , 810 ,
Ind : 8673610~~1 , 3 , 4 , 754 , 735 , 1002 , 1000 , 3231 , 128 , 145 , 113 , 2 , 19 , 810 ,
Ind : 8673611~~1 , 3 , 4 , 628 , 620 , 804 , 750 , 448 , 96 , 90 , 76 , 14 , 8 , 515 ,
Ind : 8673612~~1 , 3 , 4 , 926 , 930 , 1054 , 1100 , 1746 , 88 , 90 , 78 , -6 , -4 , 515 ,
Ind : 8673613~~1 , 3 , 4 , 1829 , 1755 , 1959 , 1925 , 3920 , 90 , 90 , 77 , 34 , 34 , 515 ,
Ind : 8673614~~1 , 3 , 4 , 1940 , 1915 , 2121 , 2110 , 378 , 101 , 115 , 87 , 11 , 25 , 688 ,
Ind : 8673615~~1 , 3 , 4 , 1937 , 1830 , 2037 , 1940 , 509 , 240 , 250 , 230 , 57 , 67 , 1591 ,
Ind : 8673616~~1 , 3 , 4 , 1039 , 1040 , 1132 , 1150 , 535 , 233 , 250 , 219 , -18 , -1 , 1591 ,
Ind : 8673617~~1 , 3 , 4 , 617 , 615 , 652 , 650 , 11 , 95 , 95 , 70 , 2 , 2 , 451 ,
Ind : 8673618~~1 , 3 , 4 , 1620 , 1620 , 1639 , 1655 , 810 , 79 , 95 , 70 , -16 , 0 , 451 ,
Ind : 8673619~~1 , 3 , 4 , 706 , 700 , 916 , 915 , 100 , 130 , 135 , 106 , 1 , 6 , 828 ,
Ind : 8673620~~1 , 3 , 4 , 1644 , 1510 , 1845 , 1725 , 1333 , 121 , 135 , 107 , 80 , 94 , 828 ,
Ind : 8673621~~1 , 3 , 4 , 1426 , 1430 , 1426 , 1425 , 829 , 60 , 55 , 39 , 1 , -4 , 162 ,
Ind : 8673622~~1 , 3 , 4 , 715 , 715 , 720 , 710 , 1016 , 65 , 55 , 37 , 10 , 0 , 162 ,
Ind : 8673623~~1 , 3 , 4 , 1702 , 1700 , 1651 , 1655 , 1827 , 49 , 55 , 35 , -4 , 2 , 162 ,
Ind : 8673624~~1 , 3 , 4 , 1029 , 1020 , 1021 , 1010 , 2272 , 52 , 50 , 37 , 11 , 9 , 162 ,
Ind : 8673625~~1 , 3 , 4 , 1452 , 1425 , 1640 , 1625 , 675 , 228 , 240 , 213 , 15 , 27 , 1489 ,
Ind : 8673626~~1 , 3 , 4 , 754 , 745 , 940 , 955 , 1144 , 226 , 250 , 205 , -15 , 9 , 1489 ,
Ind : 8673627~~1 , 3 , 4 , 1323 , 1255 , 1526 , 1510 , 4 , 123 , 135 , 110 , 16 , 28 , 838 ,
Ind : 8673628~~1 , 3 , 4 , 1416 , 1325 , 1512 , 1435 , 54 , 56 , 70 , 49 , 37 , 51 , 220 ,

(Reload : 1, Exit : 0)
Please enter next order :

1
--------------- Choosing 1! Reload file! ---------------

(ReLoad)Reading data time : 7629 ms
(ReLoad)Spliting data time: 7862 ms
New size of file : 1656459288
new size of vector : 16731912

Ind : 9722184~~1 , 3 , 4 , 2003 , 1955 , 2211 , 2225 , 335 , 128 , 150 , 116 , -14 , 8 , 810 ,
Ind : 9722185~~1 , 3 , 4 , 754 , 735 , 1002 , 1000 , 3231 , 128 , 145 , 113 , 2 , 19 , 810 ,
Ind : 9722186~~1 , 3 , 4 , 628 , 620 , 804 , 750 , 448 , 96 , 90 , 76 , 14 , 8 , 515 ,
Ind : 9722187~~1 , 3 , 4 , 926 , 930 , 1054 , 1100 , 1746 , 88 , 90 , 78 , -6 , -4 , 515 ,
Ind : 9722188~~1 , 3 , 4 , 1829 , 1755 , 1959 , 1925 , 3920 , 90 , 90 , 77 , 34 , 34 , 515 ,
Ind : 9722189~~1 , 3 , 4 , 1940 , 1915 , 2121 , 2110 , 378 , 101 , 115 , 87 , 11 , 25 , 688 ,
Ind : 9722190~~1 , 3 , 4 , 1937 , 1830 , 2037 , 1940 , 509 , 240 , 250 , 230 , 57 , 67 , 1591 ,
Ind : 9722191~~1 , 3 , 4 , 1039 , 1040 , 1132 , 1150 , 535 , 233 , 250 , 219 , -18 , -1 , 1591 ,
Ind : 9722192~~1 , 3 , 4 , 617 , 615 , 652 , 650 , 11 , 95 , 95 , 70 , 2 , 2 , 451 ,
Ind : 9722193~~1 , 3 , 4 , 1620 , 1620 , 1639 , 1655 , 810 , 79 , 95 , 70 , -16 , 0 , 451 ,
Ind : 9722194~~1 , 3 , 4 , 706 , 700 , 916 , 915 , 100 , 130 , 135 , 106 , 1 , 6 , 828 ,
Ind : 9722195~~1 , 3 , 4 , 1644 , 1510 , 1845 , 1725 , 1333 , 121 , 135 , 107 , 80 , 94 , 828 ,
Ind : 9722196~~1 , 3 , 4 , 1426 , 1430 , 1426 , 1425 , 829 , 60 , 55 , 39 , 1 , -4 , 162 ,
Ind : 9722197~~1 , 3 , 4 , 715 , 715 , 720 , 710 , 1016 , 65 , 55 , 37 , 10 , 0 , 162 ,
Ind : 9722198~~1 , 3 , 4 , 1702 , 1700 , 1651 , 1655 , 1827 , 49 , 55 , 35 , -4 , 2 , 162 ,
Ind : 9722199~~1 , 3 , 4 , 1029 , 1020 , 1021 , 1010 , 2272 , 52 , 50 , 37 , 11 , 9 , 162 ,
Ind : 9722200~~1 , 3 , 4 , 1452 , 1425 , 1640 , 1625 , 675 , 228 , 240 , 213 , 15 , 27 , 1489 ,
Ind : 9722201~~1 , 3 , 4 , 754 , 745 , 940 , 955 , 1144 , 226 , 250 , 205 , -15 , 9 , 1489 ,
Ind : 9722202~~1 , 3 , 4 , 1323 , 1255 , 1526 , 1510 , 4 , 123 , 135 , 110 , 16 , 28 , 838 ,
Ind : 9722203~~1 , 3 , 4 , 1416 , 1325 , 1512 , 1435 , 54 , 56 , 70 , 49 , 37 , 51 , 220 ,

(Reload : 1, Exit : 0)
Please enter next order :

1
--------------- Choosing 1! Reload file! ---------------
The size of file does not change!!
File size 1656459288(bytes) = 1579.722679(MB)

(Reload : 1, Exit : 0)
Please enter next order :

0
--------------- Choosing 0! Finish program! ---------------


*/