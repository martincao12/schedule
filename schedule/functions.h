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

//compute the value of the object function
double compute_score(int *stime);

//compute the proportion of engines finished on time
double compute_ontime_proportion(int *stime);

//compute maximum delay
int compute_max_delay(int *stime);

//compute average delay
double compute_avg_delay(int *stime);

//compute average flow time
double compute_avg_flowtime(int *stime);

//compute schedule deviatoin
double compute_deviation(int *stime);

//compute the sum of an array
int array_sum(int *myarray,int array_length);
double array_sum(double *myarray,int array_length);

//generate a real number between 0 and 1
double my_rand();



void array_copy(int *from_array,int *to_array,int length);

void array_copy(double *from_array,double *to_array,int length);

//compute the latest finish time
void compute_LF();