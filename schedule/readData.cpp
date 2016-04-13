#include "functions.h"
#include "variables.h"
using namespace std;

void read(string filePath)
{
	cout<<filePath;
	ifstream infile(filePath);
	cout<<infile.is_open();
	string input;

	for(int i=0;i<4;i++) {getline(infile,input);}
	engine_in_process_num=atoi(input.substr(20).c_str());
	getline(infile,input);
	engine_to_start_num=atoi(input.substr(20).c_str());
	getline(infile,input);
	task_num=atoi(input.substr(20).c_str());
	getline(infile,input);
	R_num=atoi(input.substr(20).c_str());

	int engine_num=engine_in_process_num+engine_to_start_num;
		
	R=new int[R_num];
	P=new int*[task_num];
	S=new int*[task_num];
	P_num=new int[task_num];
	S_num=new int[task_num];

	d=new int[task_num];
	r=new int*[task_num];
	LF=new int[task_num];

	engine_no=new int[task_num];
	status=new int[tsak]

	for(int i=0;i<4;i++) {getline(infile,input);}
	for(int i=0;i<R_num;i++){
		infile>>R[i];
	}
	
	for(int i=0;i<9+engine_num;i++){getline(infile,input);}
	cout<<input;
	return;

	int **tem_P=new int*[task_num];
	for(int i=0;i<task_num;i++)
	{
		tem_P[i]=new int[task_num];
		for(int j=0;j<task_num;j++)
		{
			tem_P[i][j]=0;
		}
	}

	int tem_int;
	for(int i=0;i<task_num;i++)
	{
		infile>>tem_int>>tem_int>>tem_int;
		S_num[i]=tem_int;
		S[i]=new int[tem_int];
		for(int j=0;j<tem_int;j++)
		{
			infile>>S[i][j];
			S[i][j]-=1;
			tem_P[S[i][j]][i]=1;
		}
	}

	for(int i=0;i<task_num;i++)
	{
		int sum=0;
		for(int j=0;j<task_num;j++)
		{
			sum+=tem_P[i][j];
		}
		P_num[i]=sum;
		P[i]=new int[sum];
		int k=0;
		for(int j=0;j<task_num;j++)
		{
			if(tem_P[i][j]==1)
			{
				P[i][k]=j;
				k++;
			}
		}
	}

	for(int i=0;i<task_num;i++) delete[] tem_P[i];
	delete[] tem_P;
	for(int i=0;i<5;i++) getline(infile,input);

	for(int i=0;i<task_num;i++)
	{
		r[i]=new int[R_num];
		infile>>tem_int>>tem_int>>d[i];
		for(int j=0;j<R_num;j++)
		{
			infile>>r[i][j];
		}
	}

	for(int i=0;i<4;i++) getline(infile,input);
	
	infile.clear();
	infile.close();
}