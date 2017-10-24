#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>  
#include <unistd.h>
#include <windows.h>  

#ifndef CONIO_H
#define CONIO_H

int getch( void );
int getche( void );

#endif // CONIO_H

using namespace std;

/*---------- Class & Template ----------*/
class Reader{
	private:
		static const int BUFFER_SIZE = 256; //2008.csv : 144
		static const int SUB_BUFFER_SIZE = 24;
		
		char carrSub[SUB_BUFFER_SIZE]; //input buffer
		char cptrInput[BUFFER_SIZE]; //Sub word
		char *cptrStop; //stop location of sub word
		char *cptrStart; //start locstion of sub word
		vector< tuple<int, string> > *vptrName;
		vector< vector<int> > *vptrContent;
		
		bool boolReadColumn; //whether save column(only first file need save)
		
		void reading(){
			/*---------- Read file ----------*/
			FILE *fp = fopen(strPath.c_str(), "r");
			int columnInd = 0;
			
			if (!fp) {
				printf("File not exist !!");
				exit(EXIT_FAILURE);
			}

			fgets(cptrInput, BUFFER_SIZE, fp);
			cptrStart = cptrInput;
			cptrStop = strchr(cptrStart,',');
			
			while(cptrStop != NULL){
				memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
				carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0;
				cptrStart = cptrStop+1;//1 : skip a ','
				cptrStop = strchr(cptrStart,',');
				tuple<int, string> t = make_tuple(columnInd, carrSub);
				if(boolReadColumn) vptrName->push_back(t);
				else {}
				columnInd++;
			}
			//the last word is start to end of carrSub 
			memcpy(carrSub, cptrStart, strlen(cptrStart));
			carrSub[strlen(cptrStart)] = 0;
			tuple<int, string> t = make_tuple(columnInd, carrSub);
			if(boolReadColumn) vptrName->push_back(t);
			else {}
			columnInd++;
			
			/*--- read another row(column) ---*/
			while(fgets(cptrInput, BUFFER_SIZE, fp)!=NULL) {
				vector<int> v; 
				cptrStart = cptrInput;
				cptrStop = strchr(cptrStart,',');
				
				while(cptrStop != NULL){
					memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
					carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0;
					cptrStart = cptrStop+1;
					cptrStop = strchr(cptrStart,',');
					v.push_back(atoi(carrSub));
				}
				memcpy(carrSub, cptrStart, strlen(cptrStart)-1);
				carrSub[strlen(cptrStart)-1] = 0;
				
				v.push_back(atoi(carrSub));
				vptrContent->push_back(v);
			}
			fclose(fp);
		}

	public:
		string strPath;
		Reader(string str, vector< tuple<int, string> > *vN, vector< vector<int> > *vC, bool boolColumn){
			strPath = str;
			vptrName = vN;
			vptrContent = vC;
			boolReadColumn = boolColumn;
			reading();
		}
};

class data{
	public:
		string *strPath; //file path (pointer to array)
		int ifileNum; //file number
		vector< tuple<int, string> > vColumn_name; //column name and index(first row)
		vector< vector<int> > vContent;
		int iColumnNum; //column number
		int iRow; //row column
		
		data(string *str, int i){
			strPath = str;
			iColumnNum = 0;
			iRow = 0;
			ifileNum = i;
		}
		data(){}
	
		void readFile(){
			for(int n=0; n<ifileNum; n++){
				bool boolReadColumn = true;
				
				if(n == 0) boolReadColumn = true;
				else boolReadColumn = false;
				
				Reader *reader1 = new Reader(strPath[n].c_str(), &vColumn_name, &vContent, boolReadColumn);
				delete reader1;
				iColumnNum = vColumn_name.size();
				iRow = vContent.size();
				
				cout<<"Finish : "<< strPath[n] <<endl;
				cout<<"iColumnNum : "<<iColumnNum<<endl;
				cout<<"iRow : "<<iRow<<endl;
			}
		}
};

class dataGroup{
private:
	struct ClusterData{
		string strName; //column name
		vector<int> vInd; //index in "data->vContent"
		atomic<int> atoSum; //sumnth of group
		atomic<long long> atoSDSub; //Sub*Sub sum of group
		float StandardDeviation; //Standard Deviation of group
		
		ClusterData(){
			strName = "";
			StandardDeviation = 0.0;
			atoSum = 0;
			atoSDSub = 0;
		}
	};
	
	string strColumnName;
	int iColumnInd;
	vector<ClusterData*> vCluData;
	mutex mutexCD; //mutex of vCluData
	int iThreadNUm; //thread number
	int iSubThread; //processed data number of each thread
	atomic<int> *iCpuUsed; //thread use which cpu
	
	void grouping(weak_ptr<data>); 
	void clusterSD(weak_ptr<data>);
	void groupingThread(int, weak_ptr<data> );
	void clusterSDThread(int, weak_ptr<data> );
	void addCluData(string name){
		ClusterData *CD = new ClusterData();
		vCluData.push_back(new ClusterData());
		vCluData[vCluData.size()-1]->strName.append(name);
		delete CD;
	};
	
public:
	
	dataGroup(int, weak_ptr<data>, int);
	dataGroup(string, weak_ptr<data>);
	void showInfo();
}; 
dataGroup::dataGroup(int ind, weak_ptr<data> wptrD, int iThNum){
	chrono::time_point<chrono::steady_clock> start, end;
	start = chrono::steady_clock::now();
	
	shared_ptr<data> sptrD = wptrD.lock();
	iThreadNUm = iThNum;
	iSubThread = sptrD->vContent.size() / iThreadNUm;
	this->iColumnInd = ind;
	for(auto tp : sptrD->vColumn_name){
		if(ind == get<0>(tp)){
			this->strColumnName = get<1>(tp);
			break;
		}
	}
	iCpuUsed = new atomic<int>[8];
	for(int k=0; k<8; k++) { iCpuUsed[k]=0; } //init array pointer 
	
	grouping(wptrD);
	//clusterSD(wptrD);
	
	sptrD.reset(); // reset pointer
	wptrD.reset();
	
	end = chrono::steady_clock::now();
	chrono::duration<double> elapsed_seconds = end-start;
	
	/*----- Show Data Info -----*/
	printf("\niThreadNUm num : %d\n", iThreadNUm);
	printf("CPU used : \n");
	for(int k=0; k<8; k++) { printf("[%d] : %d\n", k, iCpuUsed[k].load()); }
	printf("Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	
	showInfo();
}
void dataGroup::showInfo(){
	printf("strColumnName : %s\n", strColumnName.c_str());
	printf("iColumnInd : %d\n", iColumnInd);
	printf("ClusterData : %zd \n", vCluData.size());
	for(int n=0; n<vCluData.size(); n++){ 
		printf("    strName : %s -- ElementNum : %zd\n", vCluData[n]->strName.c_str(), vCluData[n]->vInd.size()); 
		printf("    	SDNum : %lld -- Sum : %d \n", vCluData[n]->atoSDSub.load(), vCluData[n]->atoSum.load()); 
		printf("    	StandardDeviation : %f\n", vCluData[n]->StandardDeviation); 
	}
}
void dataGroup::grouping(weak_ptr<data> wptrD){
	thread *arrThread;
	
	/*----- Create Thread -----*/
	arrThread = new thread[iThreadNUm];
	
	for(int i=0; i<iThreadNUm; i++){ arrThread[i] = thread(&dataGroup::groupingThread, this, i, wptrD); }
	for(int i=0; i<iThreadNUm; i++){ arrThread[i].join(); }
}
void dataGroup::clusterSD(weak_ptr<data> wptrD){
	thread *arrThread;
	
	/*----- Create Thread -----*/
	arrThread = new thread[iThreadNUm];
	
	for(int i=0; i<iThreadNUm; i++){ arrThread[i] = thread(&dataGroup::clusterSDThread, this, i, wptrD); }
	for(int i=0; i<iThreadNUm; i++){ arrThread[i].join(); }
	
	for(int j=0; j<vCluData.size(); j++){
		vCluData[j]-> StandardDeviation = sqrt(vCluData[j]->atoSDSub.load()/(float)vCluData[j]->vInd.size());
	}
	
};
void dataGroup::groupingThread(int iThreadCount, weak_ptr<data> wptrD){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	vector<string> vStrName; //cluster name
	vector<int> vISum; //cluster sum
	vector< vector<int> > vvInd; //index of element (grouping)
	int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
	
	shared_ptr<data> sptrDTh = wptrD.lock(); //data pointer
	
	iStart = iThreadCount * iSubThread;
	if(iThreadCount < iThreadNUm-1) iEnd = ((iThreadCount+1) * iSubThread) - 1;
	else iEnd = sptrDTh->iRow-1;
	
	string strGroName = ""; //element group name
	int iColNamInd = -1; //element group index in vStrName;
	vector<int> vvI; //empty vector
	
	for(int n=iStart; n<=iEnd; n++){
		strGroName = to_string(sptrDTh->vContent[n][this->iColumnInd]);
		iColNamInd = -1;
		if(vStrName.empty()){
			vStrName.push_back(strGroName);
			iColNamInd = 0;
			vvInd.push_back(vvI);
			vISum.push_back(0);
		}
		else{
			for(int i=0; i<vStrName.size(); i++){
				if(strGroName == vStrName[i]){
					iColNamInd = i;
					break;
				}
			}
			if(iColNamInd == -1){
				vStrName.push_back(strGroName);
				iColNamInd = vStrName.size()-1;
				vector<int> vvI;
				vvInd.push_back(vvI);
				vISum.push_back(0);
			} 
		}
		vvInd[iColNamInd].push_back(n);
		vISum[iColNamInd]+= sptrDTh->vContent[n][13];
	}
	
	
	int i_vCluData_Ind = -1;
	unique_lock<mutex> lk(mutexCD);
	for(int j=0; j<vStrName.size(); j++){
		i_vCluData_Ind = -1;
		for(int i=0; i<vCluData.size(); i++){
			if(vStrName[j] == vCluData[i]->strName){
				i_vCluData_Ind = i;
				break;
			}
		}
		if(i_vCluData_Ind == -1){
			addCluData(vStrName[j]);
			i_vCluData_Ind = vCluData.size()-1;
		}
		vCluData[i_vCluData_Ind]->vInd.insert(vCluData[i_vCluData_Ind]->vInd.end(), vvInd[j].begin(), vvInd[j].end());
		vCluData[i_vCluData_Ind]->atoSum+= vISum[j];
	}
	lk.unlock();
	
	iCpuUsed[iCupId]+=1;
	sptrDTh.reset();
}
void dataGroup::clusterSDThread(int iThreadCount, weak_ptr<data> wptrD){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
	int iSubStep = 0;
	long long iSub = 0;
	
	shared_ptr<data> sptrDTh = wptrD.lock(); //data pointer
	
	for(int i=0; i<vCluData.size(); i++){
		iSubStep = vCluData[i]->vInd.size() / iThreadNUm;
		
		iStart = iThreadCount * iSubStep;
		if(iThreadCount < iThreadNUm-1) iEnd = ((iThreadCount+1) * iSubStep) - 1;
		else iEnd = vCluData[i]->vInd.size() - 1;
		
		for(int n=iStart; n<=iEnd; n++){
			iSub = sptrDTh->vContent[vCluData[i]->vInd[n]][13] - vCluData[i]->atoSum.load()/vCluData[i]->vInd.size();
			vCluData[i]->atoSDSub += iSub*iSub;
		}
	}
}

/*---------- Main ----------*/
int main( int argc, char** argv ){
	cout << "DEBUG: main() called by process " << getpid() << " (parent: " << ::getppid() << ")" << std::endl;
	string strPath[1] = {"C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2008Sub.csv"};
	int iFileNum = 1;
	/*
	string strPath[3] = {"C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2008Sub.csv",
						 "C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2007Sub.csv",
						 "C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2006Sub.csv"};
	int iFileNum = 3;*/
	
	/*----- Read file -----*/
	shared_ptr<data> sptrData = make_shared<data>(data(strPath, iFileNum));
	weak_ptr<data> wptrData(sptrData);
	sptrData->readFile();
	
	/*----- Show Data Info -----*/
	unique_ptr<dataGroup> dgPtr1(new dataGroup(0, wptrData, 1));
	unique_ptr<dataGroup> dgPtr2(new dataGroup(0, wptrData, 2));
	//unique_ptr<dataGroup> dgPtr4(new dataGroup(0, wptrData, 4));
	//unique_ptr<dataGroup> dgPtr8(new dataGroup(0, wptrData, 8));
	
	sptrData.reset();
	wptrData.reset();
	
	return 0;
}
