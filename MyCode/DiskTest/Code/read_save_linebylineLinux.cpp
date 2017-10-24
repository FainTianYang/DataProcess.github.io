#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>  
#include <unistd.h>
#include <windows.h>  
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>

#ifndef CONIO_H
#define CONIO_H

int getch( void );
int getche( void );

#endif // CONIO_H

using namespace std;

const int BUFFER_SIZE = 256; //2008.csv : 144
const int SUB_BUFFER_SIZE = 24;

class Reader{
	private:
		char carrSub[SUB_BUFFER_SIZE]; //input buffer
		char cptrInput[BUFFER_SIZE]; //Sub word
		char *cptrStop; //stop location of sub word
		char *cptrStart; //start locstion of sub word
		vector< vector<float> > *vptrContent;
		void reading(){
			/*---------- Read file ----------*/
			FILE *fp = fopen(strPath.c_str(), "r");
			
			if(!fp){
				printf("File mot exist!!\n");
			}
			
			/*--- read another row(column) ---*/
			while(fgets(cptrInput, BUFFER_SIZE, fp)!=NULL) {
				vector<float> v; 
				cptrStart = cptrInput;
				cptrStop = strchr(cptrStart,',');
				
				while(cptrStop != NULL){
					memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
					carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0;
					cptrStart = cptrStop+1;
					cptrStop = strchr(cptrStart,',');
					v.push_back(atof(carrSub));
				}
				memcpy(carrSub, cptrStart, strlen(cptrStart)-1);
				carrSub[strlen(cptrStart)-1] = 0;
				
				v.push_back(atof(carrSub));
				vptrContent->push_back(v);
			}
			fclose(fp);
		}

	public:
		string strPath;
		Reader(string str, vector< vector<float> > *vC){
			strPath = str;
			vptrContent = vC;
			reading();
		}
};

int main( int argc, char** argv ){
	vector< vector<float> > vContent; //file content
	chrono::time_point<chrono::steady_clock> Start, End;
	Start = chrono::steady_clock::now();
	Reader *reader1 = new Reader("../DataTest/Data/1996_1999.csv", &vContent);
	delete reader1;
	End = chrono::steady_clock::now();
			
	printf("Reading Data Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(End - Start).count());
	/*---------- Show info ----------*/
	cout<<vContent.size()<<"\n";
	
	cout<<"\n";
	for(int j =0; j<10; j++){
		cout<<"\nIndex : "<< j <<" -- ";
		for(int i=0;i<vContent[j].size();i++){
			cout<<vContent[j][i]<<" , ";
		}
	}

	cout<<vContent.size()<<"\n";
	cout<<"vContent Size 1 : "<< vContent.size() * vContent[0].size() * sizeof(float)<<endl;
	cout<<"vContent Size 2 : "<< vContent.size() * sizeof vContent[0] <<endl;
	cout<<"vContent Size 3 : "<< sizeof vContent <<endl;
	
	return 0;
}
/*
$ ./r1
Reading Data Time : 22889 ms
5527884

Index : 0 -- 1 , 27 , 3 , 1906 , 1908 , 2024 , 2005 , 1244 , 78 , 57 , 66 , 19 , -2 , 198 ,
Index : 1 -- 1 , 28 , 4 , 2016 , 1908 , 2126 , 2005 , 1244 , 70 , 57 , 41 , 81 , 68 , 198 ,
Index : 2 -- 1 , 29 , 5 , 1907 , 1908 , 2000 , 2005 , 1244 , 53 , 57 , 43 , -5 , -1 , 198 ,
Index : 3 -- 1 , 31 , 7 , 1932 , 1908 , 2031 , 2005 , 1244 , 59 , 57 , 45 , 26 , 24 , 198 ,
Index : 4 -- 1 , 1 , 5 , 1601 , 1535 , 1707 , 1645 , 297 , 66 , 70 , 47 , 22 , 26 , 257 ,
Index : 5 -- 1 , 2 , 6 , 1651 , 1535 , 1829 , 1645 , 297 , 98 , 70 , 57 , 104 , 76 , 257 ,
Index : 6 -- 1 , 3 , 7 , 0 , 1535 , 0 , 1645 , 297 , 0 , 70 , 0 , 0 , 0 , 257 ,
Index : 7 -- 1 , 4 , 1 , 1559 , 1535 , 1707 , 1645 , 297 , 68 , 70 , 58 , 22 , 24 , 257 ,
Index : 8 -- 1 , 5 , 2 , 1545 , 1535 , 1703 , 1645 , 297 , 78 , 70 , 61 , 18 , 10 , 257 ,
Index : 9 -- 1 , 6 , 3 , 1608 , 1535 , 1754 , 1645 , 297 , 106 , 70 , 56 , 69 , 33 , 257 , 5527884
vContent Size 1 : 309561504
vContent Size 2 : 132669216
vContent Size 3 : 24
1999Sub.csv

$ ./r1
Reading Data Time : 29146 ms
7009728


Index : 0 -- 1 , 3 , 4 , 2003 , 1955 , 2211 , 2225 , 335 , 128 , 150 , 116 , -14 , 8 , 810 ,
Index : 1 -- 1 , 3 , 4 , 754 , 735 , 1002 , 1000 , 3231 , 128 , 145 , 113 , 2 , 19 , 810 ,
Index : 2 -- 1 , 3 , 4 , 628 , 620 , 804 , 750 , 448 , 96 , 90 , 76 , 14 , 8 , 515 ,
Index : 3 -- 1 , 3 , 4 , 926 , 930 , 1054 , 1100 , 1746 , 88 , 90 , 78 , -6 , -4 , 515 ,
Index : 4 -- 1 , 3 , 4 , 1829 , 1755 , 1959 , 1925 , 3920 , 90 , 90 , 77 , 34 , 34 , 515 ,
Index : 5 -- 1 , 3 , 4 , 1940 , 1915 , 2121 , 2110 , 378 , 101 , 115 , 87 , 11 , 25 , 688 ,
Index : 6 -- 1 , 3 , 4 , 1937 , 1830 , 2037 , 1940 , 509 , 240 , 250 , 230 , 57 , 67 , 1591 ,
Index : 7 -- 1 , 3 , 4 , 1039 , 1040 , 1132 , 1150 , 535 , 233 , 250 , 219 , -18 , -1 , 1591 ,
Index : 8 -- 1 , 3 , 4 , 617 , 615 , 652 , 650 , 11 , 95 , 95 , 70 , 2 , 2 , 451 ,
Index : 9 -- 1 , 3 , 4 , 1620 , 1620 , 1639 , 1655 , 810 , 79 , 95 , 70 , -16 , 0 , 451 , 7009728
vContent Size 1 : 392544768
vContent Size 2 : 168233472
vContent Size 3 : 24
2008Sub.csv

$ ./r1
Reading Data Time : 121619 ms
21676431


Index : 0 -- 1 , 29 , 1 , 2039 , 1930 , 2245 , 2139 , 345 , 246 , 249 , 230 , 66 , 69 , 1587 ,
Index : 1 -- 1 , 30 , 2 , 1931 , 1930 , 2142 , 2139 , 345 , 251 , 249 , 224 , 3 , 1 , 1587 ,
Index : 2 -- 1 , 31 , 3 , 1956 , 1930 , 2231 , 2139 , 345 , 275 , 249 , 241 , 52 , 26 , 1587 ,
Index : 3 -- 1 , 1 , 1 , 1730 , 1550 , 1909 , 1745 , 411 , 219 , 235 , 201 , 84 , 100 , 1587 ,
Index : 4 -- 1 , 2 , 2 , 1714 , 1550 , 1841 , 1745 , 411 , 207 , 235 , 195 , 56 , 84 , 1587 ,
Index : 5 -- 1 , 3 , 3 , 1644 , 1550 , 1835 , 1745 , 411 , 231 , 235 , 212 , 50 , 54 , 1587 ,
Index : 6 -- 1 , 4 , 4 , 1558 , 1550 , 1804 , 1745 , 411 , 246 , 235 , 226 , 19 , 8 , 1587 ,
Index : 7 -- 1 , 5 , 5 , 1557 , 1550 , 1807 , 1745 , 411 , 250 , 235 , 224 , 22 , 7 , 1587 ,
Index : 8 -- 1 , 6 , 6 , 1604 , 1550 , 1738 , 1745 , 411 , 214 , 235 , 206 , -7 , 14 , 1587 ,
Index : 9 -- 1 , 7 , 7 , 1743 , 1550 , 1933 , 1745 , 411 , 230 , 235 , 204 , 108 , 113 , 1587 , 21676431
vContent Size 1 : 1213880136
vContent Size 2 : 520234344
vContent Size 3 : 24
1996_1999.csv
*/