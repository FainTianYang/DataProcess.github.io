#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include <vector>
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <windows.h>  

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

#define COLUMN_WIDTH 7
#define COLUMN_NUMBER 14

class Data{
public:
	string strFilePath;
	vector<vector<float>> vvfContent;
	
	Data(string str, int i){
		strFilePath = str;
		if(i==0) {
			printf("Read\n");
			Read();
		}
		else{
			printf("ReadLine\n");
			ReadLine();
		}
	}
	~Data(){
		printf("~Data -- %s \n", this->strFilePath.c_str());
	}
private:
	size_t lliFileSize = 0;
	ifstream ifs;
	filebuf* pbuf;
	mutex mutex_viContent;
	mutex mutex_pbuf;
	
	void Read();
	void ReadLine();
	int ThreadRead(int);
};

void Data::Read(){
	int i=0;
	chrono::time_point<chrono::steady_clock> start, end;
	
	/*----- Read file -----*/
	ifs.open(strFilePath.c_str(), ifstream::in);
	if(!ifs){
		printf("File Open Error !\n");
		exit(1);
	}
	pbuf = ifs.rdbuf();
	
	lliFileSize = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	printf("size_t : %zu\n", lliFileSize);
	
	char* buffer = new char[lliFileSize+1];
	
	start = chrono::steady_clock::now();
	pbuf->sgetn(buffer, lliFileSize);
	if(!ifs){
		printf("error");
		exit(1);
	}
	buffer[lliFileSize] = 0;
	end = chrono::steady_clock::now();
	printf("\nRead Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
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
	printf("Split Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	cout<<"vvfContent : "<<vvfContent.size()<<endl;
	for(int i=0;i<10;i++){
		for(int j=0;j<vvfContent[i].size(); j++){
			cout<<vvfContent[i][j]<<" , ";
		}
		cout<<endl;
	}
	delete[] buffer;
}
void Data::ReadLine(){
	int i=0;
	chrono::time_point<chrono::steady_clock> start, end;
	
	/*----- Read file -----*/
	ifs.open(strFilePath.c_str(), ifstream::in);
	if(!ifs){
		printf("File Open Error !\n");
		exit(1);
	}
	pbuf = ifs.rdbuf();
	
	lliFileSize = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	printf("size_t : %zu\n", lliFileSize);
	
	char* buffer = new char[lliFileSize+1];
	
	start = chrono::steady_clock::now();
	pbuf->sgetn(buffer, lliFileSize);
	if(!ifs){
		printf("error");
		exit(1);
	}
	buffer[lliFileSize] = 0;
	//cout<<buffer<<endl;
	end = chrono::steady_clock::now();
	printf("\nRead Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	/*----- Split Input Data(vcharBuffer)-----*/
	start = chrono::steady_clock::now();
	int count = 0;
	char *sub = new char[COLUMN_WIDTH+1];
	vector<float> v(COLUMN_NUMBER);
	char *cptrStop; //stoping location of sub word
	char *cptrStart; //starting locstion of sub word
	char *pch = strtok(buffer,"\n");
	while(pch){
		//cout<<pch<<endl;
		cptrStart = pch;
		cptrStop = strchr(cptrStart,','); //step = ','
		count = 0;
		while(cptrStop != NULL){ //find ','
			memcpy(sub, cptrStart, strlen(cptrStart)-strlen(cptrStop)); //copy 
			sub[strlen(cptrStart)-strlen(cptrStop)] = 0;
			cptrStart = cptrStop+1; //1 : location of ','
			cptrStop = strchr(cptrStart,',');
			v[count] = atof(sub);
			count++;
		}
		memcpy(sub, cptrStart, strlen(cptrStart)-1);
		sub[strlen(cptrStart)-1] = 0;
		v[count] = atof(sub);
		
		vvfContent.push_back(v);
		pch = strtok(NULL,"\n");
	}
	end = chrono::steady_clock::now();
	printf("Split Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	cout<<"vvfContent : "<<vvfContent.size()<<endl;
	for(int i=0;i<10;i++){
		for(int j=0;j<vvfContent[i].size(); j++){
			cout<<vvfContent[i][j]<<" , ";
		}
		cout<<endl;
	}
	delete[] buffer;
}
int main(){
	//unique_ptr<Data> dataptr2008csv(new Data("./Data/1999Sub.csv",1));
	unique_ptr<Data> dataptr2008txt(new Data("./Data/2000_2003Fix.txt",0));
	
	return 0;
}

/*
$ time awk -F ',' '{for(i=1; i<=NF; i++){ printf("%7s", $i);} print "";}' 1999Sub.csv > 1999Fix.txt           
real    0m59.000s
user    0m54.646s
sys     0m0.405s

chun-hsin@CY0001292-60224 /cygdrive/c/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data
$ time awk -F ',' '{for(i=1; i<=NF; i++){ printf("%7s", $i);} print "";}' 1996_1999.csv > 1996_1999t.txt

real    4m5.957s
user    3m32.925s
sys     0m1.731s

chun-hsin@CY0001292-60224 /cygdrive/c/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data
$ time awk -F ',' '{for(i=1; i<=NF; i++){ printf("%7s", $i);} print "";}' 2008Sub.csv > 2008Fixt.txt

real    1m12.188s
user    1m8.468s
sys     0m0.436s

-----------------------------------------------------------------------------------------
$ ./rt
ReadLine
size_t : 460560560

Read Data : 204 ms
Split Data : 20985 ms
vvfContent : 7009728
1 , 3 , 4 , 2003 , 1955 , 2211 , 2225 , 335 , 128 , 150 , 116 , -14 , 8 , 810 ,
1 , 3 , 4 , 754 , 735 , 1002 , 1000 , 3231 , 128 , 145 , 113 , 2 , 19 , 810 ,
1 , 3 , 4 , 628 , 620 , 804 , 750 , 448 , 96 , 90 , 76 , 14 , 8 , 515 ,
1 , 3 , 4 , 926 , 930 , 1054 , 1100 , 1746 , 88 , 90 , 78 , -6 , -4 , 515 ,
1 , 3 , 4 , 1829 , 1755 , 1959 , 1925 , 3920 , 90 , 90 , 77 , 34 , 34 , 515 ,
1 , 3 , 4 , 1940 , 1915 , 2121 , 2110 , 378 , 101 , 115 , 87 , 11 , 25 , 688 ,
1 , 3 , 4 , 1937 , 1830 , 2037 , 1940 , 509 , 240 , 250 , 230 , 57 , 67 , 1591 ,
1 , 3 , 4 , 1039 , 1040 , 1132 , 1150 , 535 , 233 , 250 , 219 , -18 , -1 , 1591 ,
1 , 3 , 4 , 617 , 615 , 652 , 650 , 11 , 95 , 95 , 70 , 2 , 2 , 451 ,
1 , 3 , 4 , 1620 , 1620 , 1639 , 1655 , 810 , 79 , 95 , 70 , -16 , 0 , 451 ,
Read
size_t : 693963072

Read Data : 7223 ms
Split Data : 8451 ms
vvfContent : 7009728
1 , 3 , 4 , 2003 , 1955 , 2211 , 2225 , 335 , 128 , 150 , 116 , -14 , 8 , 810 ,
1 , 3 , 4 , 754 , 735 , 1002 , 1000 , 3231 , 128 , 145 , 113 , 2 , 19 , 810 ,
1 , 3 , 4 , 628 , 620 , 804 , 750 , 448 , 96 , 90 , 76 , 14 , 8 , 515 ,
1 , 3 , 4 , 926 , 930 , 1054 , 1100 , 1746 , 88 , 90 , 78 , -6 , -4 , 515 ,
1 , 3 , 4 , 1829 , 1755 , 1959 , 1925 , 3920 , 90 , 90 , 77 , 34 , 34 , 515 ,
1 , 3 , 4 , 1940 , 1915 , 2121 , 2110 , 378 , 101 , 115 , 87 , 11 , 25 , 688 ,
1 , 3 , 4 , 1937 , 1830 , 2037 , 1940 , 509 , 240 , 250 , 230 , 57 , 67 , 1591 ,
1 , 3 , 4 , 1039 , 1040 , 1132 , 1150 , 535 , 233 , 250 , 219 , -18 , -1 , 1591 ,
1 , 3 , 4 , 617 , 615 , 652 , 650 , 11 , 95 , 95 , 70 , 2 , 2 , 451 ,
1 , 3 , 4 , 1620 , 1620 , 1639 , 1655 , 810 , 79 , 95 , 70 , -16 , 0 , 451 ,
~Data -- ./Data/2008SubFix.txt
~Data -- ./Data/2008Sub.csv
--------------------------------------------------------------------------------------------
$ ./rt
ReadLine
size_t : 1404601726

Read Data : 37294 ms
Split Data : 65968 ms
vvfContent : 21676431
1 , 29 , 1 , 2039 , 1930 , 2245 , 2139 , 345 , 246 , 249 , 230 , 66 , 69 , 1587 ,
1 , 30 , 2 , 1931 , 1930 , 2142 , 2139 , 345 , 251 , 249 , 224 , 3 , 1 , 1587 ,
1 , 31 , 3 , 1956 , 1930 , 2231 , 2139 , 345 , 275 , 249 , 241 , 52 , 26 , 1587 ,
1 , 1 , 1 , 1730 , 1550 , 1909 , 1745 , 411 , 219 , 235 , 201 , 84 , 100 , 1587 ,
1 , 2 , 2 , 1714 , 1550 , 1841 , 1745 , 411 , 207 , 235 , 195 , 56 , 84 , 1587 ,
1 , 3 , 3 , 1644 , 1550 , 1835 , 1745 , 411 , 231 , 235 , 212 , 50 , 54 , 1587 ,
1 , 4 , 4 , 1558 , 1550 , 1804 , 1745 , 411 , 246 , 235 , 226 , 19 , 8 , 1587 ,
1 , 5 , 5 , 1557 , 1550 , 1807 , 1745 , 411 , 250 , 235 , 224 , 22 , 7 , 1587 ,
1 , 6 , 6 , 1604 , 1550 , 1738 , 1745 , 411 , 214 , 235 , 206 , -7 , 14 , 1587 ,
1 , 7 , 7 , 1743 , 1550 , 1933 , 1745 , 411 , 230 , 235 , 204 , 108 , 113 , 1587 ,
Read
size_t : 2145966669

Read Data : 81641 ms
Split Data : 32279 ms
vvfContent : 21676431
1 , 29 , 1 , 2039 , 1930 , 2245 , 2139 , 345 , 246 , 249 , 230 , 66 , 69 , 1587 ,
1 , 30 , 2 , 1931 , 1930 , 2142 , 2139 , 345 , 251 , 249 , 224 , 3 , 1 , 1587 ,
1 , 31 , 3 , 1956 , 1930 , 2231 , 2139 , 345 , 275 , 249 , 241 , 52 , 26 , 1587 ,
1 , 1 , 1 , 1730 , 1550 , 1909 , 1745 , 411 , 219 , 235 , 201 , 84 , 100 , 1587 ,
1 , 2 , 2 , 1714 , 1550 , 1841 , 1745 , 411 , 207 , 235 , 195 , 56 , 84 , 1587 ,
1 , 3 , 3 , 1644 , 1550 , 1835 , 1745 , 411 , 231 , 235 , 212 , 50 , 54 , 1587 ,
1 , 4 , 4 , 1558 , 1550 , 1804 , 1745 , 411 , 246 , 235 , 226 , 19 , 8 , 1587 ,
1 , 5 , 5 , 1557 , 1550 , 1807 , 1745 , 411 , 250 , 235 , 224 , 22 , 7 , 1587 ,
1 , 6 , 6 , 1604 , 1550 , 1738 , 1745 , 411 , 214 , 235 , 206 , -7 , 14 , 1587 ,
1 , 7 , 7 , 1743 , 1550 , 1933 , 1745 , 411 , 230 , 235 , 204 , 108 , 113 , 1587 ,
~Data -- ./Data/1996_1999Fix.txt
~Data -- ./Data/1996_1999.csv

-----------------------------------------------------------------------------------------
$ ./rt
ReadLine
size_t : 358084630

Read Data : 4757 ms
Split Data : 16698 ms
vvfContent : 5527884
1 , 27 , 3 , 1906 , 1908 , 2024 , 2005 , 1244 , 78 , 57 , 66 , 19 , -2 , 198 ,
1 , 28 , 4 , 2016 , 1908 , 2126 , 2005 , 1244 , 70 , 57 , 41 , 81 , 68 , 198 ,
1 , 29 , 5 , 1907 , 1908 , 2000 , 2005 , 1244 , 53 , 57 , 43 , -5 , -1 , 198 ,
1 , 31 , 7 , 1932 , 1908 , 2031 , 2005 , 1244 , 59 , 57 , 45 , 26 , 24 , 198 ,
1 , 1 , 5 , 1601 , 1535 , 1707 , 1645 , 297 , 66 , 70 , 47 , 22 , 26 , 257 ,
1 , 2 , 6 , 1651 , 1535 , 1829 , 1645 , 297 , 98 , 70 , 57 , 104 , 76 , 257 ,
1 , 3 , 7 , 0 , 1535 , 0 , 1645 , 297 , 0 , 70 , 0 , 0 , 0 , 257 ,
1 , 4 , 1 , 1559 , 1535 , 1707 , 1645 , 297 , 68 , 70 , 58 , 22 , 24 , 257 ,
1 , 5 , 2 , 1545 , 1535 , 1703 , 1645 , 297 , 78 , 70 , 61 , 18 , 10 , 257 ,
1 , 6 , 3 , 1608 , 1535 , 1754 , 1645 , 297 , 106 , 70 , 56 , 69 , 33 , 257 ,
Read
size_t : 547260516

Read Data : 5848 ms
Split Data : 6966 ms
vvfContent : 5527884
1 , 27 , 3 , 1906 , 1908 , 2024 , 2005 , 1244 , 78 , 57 , 66 , 19 , -2 , 198 ,
1 , 28 , 4 , 2016 , 1908 , 2126 , 2005 , 1244 , 70 , 57 , 41 , 81 , 68 , 198 ,
1 , 29 , 5 , 1907 , 1908 , 2000 , 2005 , 1244 , 53 , 57 , 43 , -5 , -1 , 198 ,
1 , 31 , 7 , 1932 , 1908 , 2031 , 2005 , 1244 , 59 , 57 , 45 , 26 , 24 , 198 ,
1 , 1 , 5 , 1601 , 1535 , 1707 , 1645 , 297 , 66 , 70 , 47 , 22 , 26 , 257 ,
1 , 2 , 6 , 1651 , 1535 , 1829 , 1645 , 297 , 98 , 70 , 57 , 104 , 76 , 257 ,
1 , 3 , 7 , 0 , 1535 , 0 , 1645 , 297 , 0 , 70 , 0 , 0 , 0 , 257 ,
1 , 4 , 1 , 1559 , 1535 , 1707 , 1645 , 297 , 68 , 70 , 58 , 22 , 24 , 257 ,
1 , 5 , 2 , 1545 , 1535 , 1703 , 1645 , 297 , 78 , 70 , 61 , 18 , 10 , 257 ,
1 , 6 , 3 , 1608 , 1535 , 1754 , 1645 , 297 , 106 , 70 , 56 , 69 , 33 , 257 ,
~Data -- ./Data/1999Fix.txt
~Data -- ./Data/1999Sub.csv


*/