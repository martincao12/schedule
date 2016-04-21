#include <iostream>
#include <direct.h>
#include <Windows.h>
#include <string>
#include <io.h>
#include <fstream>
#include <strstream>
#include <time.h>
#include "variables.h"
using namespace std;



//compute the proportion of engines finished on time
double compute_ontime_proportion(int *stime){
	int finised=0;
	for(int i=0;i<engine_in_process_num+engine_to_start_num;i++){
		if(due_time[10*i+10]>=stime[10*i+10]){
			finised++;
		}
	}
	return finised/double(engine_in_process_num+engine_to_start_num);
}

//compute maximum delay
int compute_max_delay(int *stime){
	int max_delay=0;
	for(int i=0;i<engine_in_process_num+engine_to_start_num;i++){
		if(stime[10*i+10]-due_time[10*i+10]>max_delay){
			max_delay=stime[10*i+10]-due_time[10*i+10];
		}
	}
	return max_delay;
}

//compute average delay
double compute_avg_delay(int *stime){
	int total_delay=0;
	for(int i=0;i<engine_in_process_num+engine_to_start_num;i++){
		if(-due_time[10*i+10]+stime[10*i+10]>0){
			total_delay+=-due_time[10*i+10]+stime[10*i+10];
		}
	}
	return total_delay/double(engine_in_process_num+engine_to_start_num);
}

//compute schedule deviatoin
double compute_deviation(int *stime){
	int deviation_count=0;
	int total_count=0;
	for(int i=1;i<task_num-1;i++){
		if(status[i]==1) total_count++;
		if(status[i]==1&&stime[i]!=schedule_start_time[i]) deviation_count++;
	}
	return deviation_count/double(total_count);
}

//compute average flow time
double compute_avg_flowtime(int *stime){
	int total_flow_time=0;
	for(int i=0;i<engine_in_process_num+engine_to_start_num;i++){
		int start_time,end_time;
		start_time=stime[i*10+1];
		end_time=stime[i*10+10]+d[i*10+10];
		if(status[i*10+1]==2||status[i*10+1]==3) start_time=schedule_start_time[i*10+1];
		if(status[i*10+10]==2||status[i*10+10]==3) end_time=schedule_start_time[i*10+10]+d[i*10+10];
		total_flow_time+=end_time-start_time;
	}
	return total_flow_time/double(engine_in_process_num+engine_to_start_num);
}

//compute the value of the object function
double compute_score(int *stime){
	return (1-compute_ontime_proportion(stime))*10000+(compute_avg_flowtime(stime)-30)/30*1000;
}

//compute the sum of an array
int array_sum(int *myarray,int array_length)
{
	int sum=0;
	for(int i=0;i<array_length;i++) sum+=myarray[i];
	return sum;
}

double array_sum(double *myarray,int array_length){
	double sum=0;
	for(int i=0;i<array_length;i++) sum+=myarray[i];
	return sum;
}

//generate a real number between 0 and 1
double my_rand()
{
	return (double)rand()/(RAND_MAX+1);
}

void array_copy(int *from_array,int *to_array,int length)
{
	for(int i=0;i<length;i++) to_array[i]=from_array[i];
}

void array_copy(double *from_array,double *to_array,int length)
{
	for(int i=0;i<length;i++) to_array[i]=from_array[i];
}


//compute the latest finish time
void compute_LF()
{
	LF[task_num-1]=array_sum(d,task_num);
	for(int i=task_num-2;i>=0;i--)
	{
		LF[i]=0;
		for(int j=0;j<S_num[i];j++)
		{
			int tem_s=LF[S[i][j]]-d[S[i][j]];
			if(LF[i]>tem_s||LF[i]==0) LF[i]=tem_s;
		}
	}
}