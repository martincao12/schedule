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
	d_multi_mode=new int*[task_num];
	r_multi_mode=new int**[task_num];

	engine_no=new int[task_num];
	status=new int[task_num];
	resource_occupation=new int[task_num];
	standby_time=new int[task_num];
	due_time=new int[task_num];
	punish_weight=new double[task_num];
	schedule_start_time=new int[task_num];
	modes=new int[task_num];

	for(int i=0;i<4;i++) {getline(infile,input);}
	for(int i=0;i<R_num;i++){
		infile>>R[i];
	}
	
	for(int i=0;i<9+engine_num;i++){getline(infile,input);}

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
	string tem_ss;
	for(int i=0;i<task_num;i++)
	{
		infile>>tem_int>>tem_ss>>engine_no[i]>>status[i]>>resource_occupation[i]>>standby_time[i]>>due_time[i]>>punish_weight[i]>>schedule_start_time[i]>>modes[i]>>tem_int;
		S_num[i]=tem_int;
		S[i]=new int[tem_int];
		for(int j=0;j<tem_int;j++)
		{
			infile>>S[i][j];
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
		d_multi_mode[i]=new int[modes[i]];
		r_multi_mode[i]=new int*[modes[i]];
		for(int j=0;j<modes[i];j++){
			r_multi_mode[i][j]=new int[R_num];
			infile>>tem_int>>tem_int>>d_multi_mode[i][j];
			for(int k=0;k<R_num;k++) infile>>r_multi_mode[i][j][k];
		}
	}
	
	infile.clear();
	infile.close();
}

void dataPreProcess(){
	
	for(int i=0;i<task_num;i++) r[i]=new int[R_num];

	d[0]=d_multi_mode[0][0];
	array_copy(r_multi_mode[0][0],r[0],R_num);
	d[task_num-1]=d_multi_mode[task_num-1][0];
	array_copy(r_multi_mode[task_num-1][0],r[task_num-1],R_num);
	

	int *mode_active=new int[task_num];
	for(int i=0;i<task_num;i++) mode_active[i]=0;

	int *facility_count=new int[R_num];
	for(int i=0;i<R_num;i++) facility_count[i]=0;
	for(int i=1;i<task_num;i++){
		if((i%10==4||i%10==0)&&status[i]!=3){
			if(status[i]==2){
				for(int j=0;j<modes[i];j++){
					if(r_multi_mode[i][j][resource_occupation[i]]!=0){
						mode_active[i]=j;
						break;
					}
				}
				facility_count[resource_occupation[i]]++;
			}
			else{
				int* facility_list=new int[modes[i]];
				int* probability=new int[modes[i]];
				for(int j=0;j<modes[i];j++){
					for(int k=10;k<R_num;k++){
						if(r_multi_mode[i][j][k]!=0){
							facility_list[j]=k;
							break;
						}
					}
				}
				int tem_min_count=100000;
				for(int j=0;j<modes[i];j++){
					if(facility_count[facility_list[j]]<tem_min_count){
						tem_min_count=facility_count[facility_list[j]];
					}
				}
				for(int j=0;j<modes[i];j++){
					if(facility_count[facility_list[j]]==tem_min_count){
						probability[j]=1;
					}
					else{
						probability[j]=0;
					}
				}
				while(true){
					int tem_mode=my_rand()*modes[i];
					if(probability[tem_mode]==1){
						mode_active[i]=tem_mode;
						facility_count[facility_list[tem_mode]]++;
						break;
					}
				}
				delete[] facility_list;
				delete[] probability;
			}
		
		}
	}
	delete[] facility_count;


	for(int i=1;i<task_num-1;i++){
		if(status[i]==3){
			d[i]=0;
			for(int j=0;j<R_num;j++) r[i][j]=0;
		}
		else if(i%10==4||i%10==0){
			d[i]=d_multi_mode[i][mode_active[i]];
			array_copy(r_multi_mode[i][mode_active[i]],r[i],R_num);
		}
		else{
			d[i]=d_multi_mode[i][0];
			array_copy(r_multi_mode[i][0],r[i],R_num);
		}
		if(status[i]==2){
			d[i]=d[i]+schedule_start_time[i];
		}
	}
	delete[] mode_active;

	for(int i=0;i<task_num;i++) due_time[i]=due_time[i]-d[i];
}