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