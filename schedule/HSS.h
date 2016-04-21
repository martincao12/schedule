#include "functions.h"
#include "variables.h"
#include "readData.h"
#include <cstdlib>
using namespace std;

namespace HSS_name{
	int init_pool_size=100;
	int niter=2;
	int b1_size=20;
	int b2_size=15;
	int b1_threshold=1.1;
	int b2_threshold=2;

	int **indv_b1;
	int **indv_b2;
	int **sche_b1;
	int **sche_b2;
	int *label_b1;

	int obj_best;
	int obj_worst;

	double em_distance=0.2;//minimum distance between p_min and p_max used in EM method
	int crossover_num=2;//number of points chosen to crossover in traditional crossover method

	//generate standard random key from the start time
	void getSRK(int *stime,int *SRK){
		int *stime_tem=new int[task_num];
		int *AL=new int[task_num];
		array_copy(stime,stime_tem,task_num);
		for(int i=0;i<task_num;i++){
			AL[i]=i;
		}

		int swap_container;
		for(int i=0;i<task_num;i++){
			for(int j=0;j<task_num-i-1;j++){
				if(stime_tem[j]>stime_tem[j+1]){
					swap_container=stime_tem[j+1];
					stime_tem[j+1]=stime_tem[j];
					stime_tem[j]=swap_container;
					swap_container=AL[j+1];
					AL[j+1]=AL[j];
					AL[j]=swap_container;
				}
			}
		}

		int SRK_index=1;
		int time_index=0;
		for(int i=1;i<task_num-1;i++){
			if(stime[AL[i]]>time_index){
				time_index=stime[AL[i]];
				SRK_index=i;
			}
			SRK[AL[i]]=SRK_index;
		}

		SRK[0]=0;
		SRK[task_num-1]=task_num-1;

		delete[] AL;
		delete[] stime_tem;
	}


	//generate activity list from priority
	template <class T>
	void gen_activityList(T *orgin_x,int *activityList)
	{
		T *x=new T[task_num];
		array_copy(orgin_x,x,task_num);
		for(int i=0;i<task_num;i++)
		{
			activityList[i]=i;
		}
		for(int i=0;i<task_num;i++)
		{
			for(int j=0;j<=task_num-i-1;j++)
			{
				if(x[j]<x[j+1])
				{
					T tem_d;
					tem_d=x[j];
					x[j]=x[j+1];
					x[j+1]=tem_d;
					int tem_i;
					tem_i=activityList[j];
					activityList[j]=activityList[j+1];
					activityList[j+1]=tem_i;
				}
			}
		}
		delete[] x;
	}

	//generate activity list from NSRK
	template <class T>
	void gen_activityList_NSRK(T *orgin_x,int *activityList)
	{
		T *x=new T[task_num];
		array_copy(orgin_x,x,task_num);
		for(int i=0;i<task_num;i++)
		{
			activityList[i]=i;
		}
		for(int i=0;i<task_num;i++)
		{
			for(int j=0;j<task_num-i-1;j++)
			{
				if(x[j]>x[j+1])
				{
					T tem_d;
					tem_d=x[j];
					x[j]=x[j+1];
					x[j+1]=tem_d;
					int tem_i;
					tem_i=activityList[j];
					activityList[j]=activityList[j+1];
					activityList[j+1]=tem_i;
				}
			}
		}
		delete[] x;
	}

	//check whether one activity can be scheduled according to the precedence constraints
	boolean canBeScheduled(int i,int *isScheduled)
	{
		for(int k=0;k<P_num[i];k++)
		{
			if(isScheduled[P[i][k]]==-1) return false;
		}
		return true;
	}

	//get schedule from activity_list
	void SGS(int *AL,int *stime){
		int *Sg=new int[task_num];
		int *Fg=new int[task_num];
		int *isScheduled=new int[task_num];
		int *activityList=new int[task_num];
		for(int i=0;i<task_num;i++)
		{
			activityList[i]=AL[i];
			isScheduled[i]=-1;
		}
		Sg[0]=0; Fg[0]=0; isScheduled[0]=0;
		for(int i=1;i<task_num;i++)
		{
			for(int j=0;j<task_num;j++)
			{
				if(isScheduled[activityList[j]]!=-1) continue;
				if(!canBeScheduled(activityList[j],isScheduled)) continue;
				int *R_copy=new int[R_num];
				int Fgi_tem=LF[task_num-1];

				int ES_tem=0;
				for(int k=0;k<P_num[activityList[j]];k++)
				{
					int tem_int=Fg[isScheduled[P[activityList[j]][k]]];
					if(ES_tem<tem_int) ES_tem=tem_int;
				}

				if(ES_tem<standby_time[activityList[j]]) ES_tem=standby_time[activityList[j]];
				int max_Fg=0;
				for(int k=0;k<i;k++){
					if(Fg[k]>max_Fg) max_Fg=Fg[k];
				}

				if(max_Fg<=ES_tem){
					Sg[i]=activityList[j];
					Fg[i]=ES_tem+d[activityList[j]];
					isScheduled[activityList[j]]=i;
					delete[] R_copy;
					break;
				}

				for(int k=0;k<i;k++)
				{
					if(Fg[k]<ES_tem) continue;
					for(int l=0;l<R_num;l++) R_copy[l]=R[l];
					int rFeasible=1;

					for(int s=0;s<i;s++)
					{
						if(!(Fg[k]<=Fg[s]&&Fg[s]<Fg[k]+d[activityList[j]])) continue;
					
						for(int l=0;l<i;l++)
						{
							if(Fg[l]-d[Sg[l]]<=Fg[s]&&Fg[s]<Fg[l])
							{
								for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
							}
						}				
						for(int l=0;l<R_num;l++)
						{
							if(R_copy[l]<r[activityList[j]][l])
							{
								rFeasible=0;
								break;
							}
						}
					}
				
					if(rFeasible==1&&Fg[k]<Fgi_tem)
					{
						Fgi_tem=Fg[k];
					}
				}
				Sg[i]=activityList[j];
				Fg[i]=Fgi_tem+d[activityList[j]];
				isScheduled[activityList[j]]=i;
				delete[] R_copy;
				break;
			}
		}

		for(int i=0;i<task_num;i++){
			stime[Sg[i]]=Fg[i]-d[Sg[i]];
		}

		delete[] Sg;
		delete[] Fg;
		delete[] isScheduled;
		delete[] activityList;
	}

	//right justification
	void right_justification(int *stime)
	{
		int *Sg=new int[task_num];
		for(int i=0;i<task_num;i++) Sg[i]=i;

		int *Fg=new int[task_num];
		for(int i=0;i<task_num;i++) Fg[i]=stime[i]+d[i];

		int *activityList=new int[task_num];
		gen_activityList(Fg,activityList);
		int *activity_Fg=new int[task_num];
		for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
		for(int i=0;i<task_num;i++)
		{
			if(Sg[activityList[i]]==task_num-1) continue;
			int tem_LF=LF[task_num-1];
			for(int j=0;j<S_num[Sg[activityList[i]]];j++)
			{
				int tem_int=activity_Fg[S[Sg[activityList[i]]][j]]-d[S[Sg[activityList[i]]][j]];
				if(tem_int<tem_LF) tem_LF=tem_int;
			}
			int *R_copy=new int[R_num];

			for(int t=tem_LF-d[Sg[activityList[i]]];t>=Fg[activityList[i]]-d[Sg[activityList[i]]];t--)
			{
				int rFeasible=1;
				{
					array_copy(R,R_copy,R_num);
					for(int l=0;l<task_num;l++)
					{
						if(l==activityList[i]) continue;
						if(Fg[l]-d[Sg[l]]<=t&&t<Fg[l])
						{
							for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
						}
					}
					for(int l=0;l<R_num;l++)
					{
						if(R_copy[l]<r[Sg[activityList[i]]][l])
						{
							rFeasible=0;
							break;
						}
					}
				}
				for(int s=0;s<task_num;s++)
				{
					if(s==activityList[i]) continue;

					if(t<Fg[s]-d[Sg[s]]&&Fg[s]-d[Sg[s]]<t+d[Sg[activityList[i]]])
					{
						array_copy(R,R_copy,R_num);
						for(int l=0;l<task_num;l++)
						{
							if(l==activityList[i]) continue;
							if(Fg[l]-d[Sg[l]]<=Fg[s]-d[Sg[s]]&&Fg[s]-d[Sg[s]]<Fg[l])
							{
								for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
							}
						}				
						for(int l=0;l<R_num;l++)
						{
							if(R_copy[l]<r[Sg[activityList[i]]][l])
							{
								rFeasible=0;
								break;
							}
						}
					}
				}

				if(rFeasible==1)
				{
					Fg[activityList[i]]=t+d[Sg[activityList[i]]];
					break;
				}
			}
			delete[] R_copy;
			for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
		}
		//for(int i=task_num-1;i>=0;i--) Fg[i]-=Fg[0];
		for(int i=0;i<task_num;i++) stime[i]=Fg[i]-d[i];
		delete[] activityList;
		delete[] activity_Fg;
		delete[] Sg;
		delete[] Fg;
	}

	//left justification
	void left_justification(int *stime)
	{
		stime[0]=0;
		for(int i=0;i<task_num;i++){
			if(status[i]==3) stime[i]=0;
		}
		int *Sg=new int[task_num];
		for(int i=0;i<task_num;i++) Sg[i]=i;

		int *Fg=new int[task_num];
		for(int i=0;i<task_num;i++) Fg[i]=stime[i]+d[i];

		int *activityList=new int[task_num];
		int *Fg_copy=new int[task_num];
		array_copy(Fg,Fg_copy,task_num);
		for(int i=0;i<task_num;i++) Fg_copy[i]-=d[Sg[i]];
		gen_activityList(Fg_copy,activityList);
		delete[] Fg_copy;
		int *activity_Fg=new int[task_num];
		for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
		for(int i=task_num-1;i>=0;i--)
		{
			if(Sg[activityList[i]]==0) continue;
			int tem_ES=0;
			for(int j=0;j<P_num[Sg[activityList[i]]];j++)
			{
				int tem_int=activity_Fg[P[Sg[activityList[i]]][j]];
				if(tem_int>tem_ES) tem_ES=tem_int;
			}
			if(standby_time[Sg[activityList[i]]]>tem_ES) tem_ES=standby_time[Sg[activityList[i]]];

			int *R_copy=new int[R_num];

			for(int t=tem_ES;t<Fg[activityList[i]]-d[Sg[activityList[i]]];t++)
			{
				int rFeasible=1;
				{
					array_copy(R,R_copy,R_num);
					for(int l=0;l<task_num;l++)
					{
						if(l==activityList[i]) continue;
						if(Fg[l]-d[Sg[l]]<=t&&t<Fg[l])
						{
							for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
						}
					}
					for(int l=0;l<R_num;l++)
					{
						if(R_copy[l]<r[Sg[activityList[i]]][l])
						{
							rFeasible=0;
							break;
						}
					}
				}
				for(int s=0;s<task_num;s++)
				{
					if(s==activityList[i]) continue;

					if(t<Fg[s]-d[Sg[s]]&&Fg[s]-d[Sg[s]]<t+d[Sg[activityList[i]]])
					{
						array_copy(R,R_copy,R_num);
						for(int l=0;l<task_num;l++)
						{
							if(l==activityList[i]) continue;
							if(Fg[l]-d[Sg[l]]<=Fg[s]-d[Sg[s]]&&Fg[s]-d[Sg[s]]<Fg[l])
							{
								for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
							}
						}				
						for(int l=0;l<R_num;l++)
						{
							if(R_copy[l]<r[Sg[activityList[i]]][l])
							{
								rFeasible=0;
								break;
							}
						}
					}
				}

				if(rFeasible==1)
				{
					Fg[activityList[i]]=t+d[Sg[activityList[i]]];
					break;
				}
			}
			delete[] R_copy;
			for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
		}
		
		for(int i=0;i<task_num;i++) stime[i]=Fg[i]-d[i];
		delete[] activityList;
		delete[] activity_Fg;
		delete[] Sg;
		delete[] Fg;
	}

	//combination with EM method,generate one child
	void em_combination(int *indv1,int *indv2,int obj_best,int obj_worst,int obj_indv1,int obj_indv2,double *child){
		double q_ij;
		q_ij=(obj_indv1-obj_indv2)/double(obj_worst-obj_best);
		double *F_ij=new double[task_num];
		for(int i=0;i<task_num;i++){
			F_ij[i]=(indv2[i]-indv1[i])*q_ij;
		}

		int p_min=my_rand()*(int((1-em_distance)*(task_num-2))-1)+1;
		int p_max=my_rand()*(int((1-em_distance)*(task_num-2))-p_min)+p_min+int(em_distance*(task_num-2));
		if(p_min<1) p_min=1;
		if(p_max>task_num-2) p_max=task_num-2;

		for(int i=0;i<task_num;i++){
			if(indv1[i]<p_min){
				child[i]=indv1[i]-task_num;
			}
			else if(indv1[i]>p_max){
				child[i]=indv1[i]+task_num;
			}
			else{
				child[i]=indv1[i]+F_ij[i];
			}
		}

		delete[] F_ij;
	}

	//combination with n-point crossover,generate two children
	void crossover(int *indv1,int *indv2,int *child1,int *child2){
		int p_min=my_rand()*(task_num-2-crossover_num-1)+1;
		int p_max=p_min+crossover_num;
		if(p_min<1) p_min=1;
		if(p_max>task_num-2) p_max=task_num-2;

		for(int i=0;i<task_num;i++){
			if(indv1[i]<p_min){
				child1[i]=indv1[i]-task_num;
			}
			else if(indv1[i]>p_max){
				child1[i]=indv1[i]+task_num;
			}
			else{
				child1[i]=indv2[i];
			}
		}

		for(int i=0;i<task_num;i++){
			if(indv2[i]<p_min){
				child2[i]=indv2[i]-task_num;
			}
			else if(indv2[i]>p_max){
				child2[i]=indv2[i]+task_num;
			}
			else{
				child2[i]=indv1[i];
			}
		}
	}


	//get the distance between two SRKs
	//sum of the absolute values of the component-wise differences divided by the number of activities
	double get_distance(int *indv1,int *indv2){
		double sum=0;
		for(int i=1;i<task_num-1;i++){
			sum+=abs(indv1[i]-indv2[i]);
		}
		sum=sum/double(task_num-2);

		return sum;
	}


	void sampling(int *x,double *priority){
		int *isScheduled=new int[task_num];
		for(int i=0;i<task_num;i++) isScheduled[i]=0;
		x[0]=0;
		x[task_num-1]=task_num-1;
		isScheduled[0]=1;
		isScheduled[task_num-1]=1;
		for(int pos=1;pos<task_num-1;pos++){
			double *modi_prio=new double[task_num];
			modi_prio[0]=0;
			modi_prio[task_num-1]=0;
			for(int i=1;i<task_num-1;i++){
				if(isScheduled[i]==1){
					modi_prio[i]=0;
					continue;
				}
				bool canBeScheduled=true;
				for(int j=0;j<P_num[i];j++){
					if(isScheduled[P[i][j]]==0){
						canBeScheduled=false;
						break;
					}
				}
				if(canBeScheduled){
					modi_prio[i]=priority[i]+1;
				}
				else{
					modi_prio[i]=0;
				}
			}

			double stop=my_rand()*array_sum(modi_prio,task_num);
			for(int i=0;i<task_num;i++){
				if(stop<array_sum(modi_prio,i+1)){
					x[pos]=i;
					isScheduled[i]=1;
					break;
				}
			}

			delete[] modi_prio;
		}
		

		delete[] isScheduled;

	}

	//sort the population
	void population_sorting(int **sche,int **SRK,int *label,int population_size){
		int *tem_sche=new int[task_num];
		int *tem_SRK=new int[task_num];
		int tem_label;

		for(int i=0;i<population_size;i++){
			for(int j=0;j<population_size-i-1;j++){
				if(compute_score(sche[j])>compute_score(sche[j+1])){
					array_copy(sche[j+1],tem_sche,task_num);
					array_copy(sche[j],sche[j+1],task_num);
					array_copy(tem_sche,sche[j],task_num);
					array_copy(SRK[j+1],tem_SRK,task_num);
					array_copy(SRK[j],SRK[j+1],task_num);
					array_copy(tem_SRK,SRK[j],task_num);
					tem_label=label[j+1];	
					label[j+1]=label[j];
					label[j]=tem_label;
				}
			}
		}

		delete[] tem_sche;
		delete[] tem_SRK;
	}

	void population_sorting(int **sche,int **SRK,int population_size){
		int *tem_sche=new int[task_num];
		int *tem_SRK=new int[task_num];

		for(int i=0;i<population_size;i++){
			for(int j=0;j<population_size-i-1;j++){
				if(compute_score(sche[j])>compute_score(sche[j+1])){
					array_copy(sche[j+1],tem_sche,task_num);
					array_copy(sche[j],sche[j+1],task_num);
					array_copy(tem_sche,sche[j],task_num);
					array_copy(SRK[j+1],tem_SRK,task_num);
					array_copy(SRK[j],SRK[j+1],task_num);
					array_copy(tem_SRK,SRK[j],task_num);
				}
			}
		}

		delete[] tem_sche;
		delete[] tem_SRK;
	}

	//compare two SRKs
	bool is_same(int *SRK1,int *SRK2){
		bool isSame=true;
		for(int i=0;i<task_num;i++){
			if(SRK1[i]!=SRK2[i]){
				isSame=false;
				break;
			}
		}
		return isSame;
	}

	//initilize the pool and the population
	void initialize(){
		indv_b1=new int*[b1_size];
		indv_b2=new int*[b2_size];
		sche_b1=new int*[b1_size];
		sche_b2=new int*[b2_size];
		label_b1=new int[b1_size];

		for(int i=0;i<b1_size;i++){
			indv_b1[i]=new int[task_num];
			sche_b1[i]=new int[task_num];
			label_b1[i]=1;
		}

		for(int i=0;i<b2_size;i++){
			indv_b2[i]=new int[task_num];
			sche_b2[i]=new int[task_num];
		}

		int **init_pool_indv;
		int **init_pool_sche;
		init_pool_indv=new int*[init_pool_size];
		init_pool_sche=new int*[init_pool_size];
		for(int i=0;i<init_pool_size;i++){
			init_pool_indv[i]=new int[task_num];
			init_pool_sche[i]=new int[task_num];
		}

		compute_LF();
		double *LFT_priority=new double[task_num];
		for(int i=0;i<task_num;i++){
			LFT_priority[i]=1/(double)LF[i];
			if(status[i]==2) LFT_priority[i]=my_rand()*5+5;
		}


		int *isDeleted=new int[init_pool_size];
		for(int i=0;i<init_pool_size;i++){
			sampling(init_pool_indv[i],LFT_priority);
			SGS(init_pool_indv[i],init_pool_sche[i]);
			right_justification(init_pool_sche[i]);
			left_justification(init_pool_sche[i]);
			getSRK(init_pool_sche[i],init_pool_indv[i]);
			isDeleted[i]=0;
		}

		population_sorting(init_pool_sche,init_pool_indv,init_pool_size);
		array_copy(init_pool_indv[0],indv_b1[0],task_num);
		array_copy(init_pool_sche[0],sche_b1[0],task_num);
		isDeleted[0]=1;
		obj_best=obj_worst=compute_score(sche_b1[0]);
		int b1_index=1;

		for(int i=1;i<init_pool_size;i++){
			bool diverse_enough=true;
			for(int j=0;j<b1_index;j++){
				if(get_distance(indv_b1[j],init_pool_indv[i])<b1_threshold){
					diverse_enough=false;
					break;
				}
			}
			if(diverse_enough){
				array_copy(init_pool_indv[i],indv_b1[b1_index],task_num);
				array_copy(init_pool_sche[i],sche_b1[b1_index],task_num);
				isDeleted[i]=1;
				b1_index++;

				if(compute_score(init_pool_sche[i])<obj_best) obj_best=compute_score(init_pool_sche[i]);
				if(compute_score(init_pool_sche[i])>obj_worst) obj_worst=compute_score(init_pool_sche[i]);
			}
			if(b1_index>=b1_size) break;
		}

		//cout<<endl<<"from"<<b1_index;
		if(b1_index<b1_size){
			for(int i=1;i<init_pool_size;i++){
				if(isDeleted[i]!=1){
					array_copy(init_pool_indv[i],indv_b1[b1_index],task_num);
					array_copy(init_pool_sche[i],sche_b1[b1_index],task_num);
					isDeleted[i]=1;
					b1_index++;

					if(compute_score(init_pool_sche[i])<obj_best) obj_best=compute_score(init_pool_sche[i]);
					if(compute_score(init_pool_sche[i])>obj_worst) obj_worst=compute_score(init_pool_sche[i]);
				}
				if(b1_index>=b1_size) break;
			}
			//cout<<endl<<"b1_index"<<b1_index;
		}

		int b2_index=0;
		for(int i=0;i<init_pool_size;i++){
			if(isDeleted[i]==1) continue;

			bool diverse_enough=true;
			for(int j=0;j<b1_size;j++){
				if(get_distance(indv_b1[j],init_pool_indv[i])<b2_threshold){
					diverse_enough=false;
					break;
				}
			}
			if(diverse_enough){
				for(int j=0;j<b2_index;j++){
					if(get_distance(indv_b2[j],init_pool_indv[i])<b2_threshold){
						diverse_enough=false;
						break;
					}
				}
			}

			if(diverse_enough){
				array_copy(init_pool_indv[i],indv_b2[b2_index],task_num);
				array_copy(init_pool_sche[i],sche_b2[b2_index],task_num);
				isDeleted[i]=1;
				b2_index++;

				if(compute_score(init_pool_sche[i])<obj_best) obj_best=compute_score(init_pool_sche[i]);
				if(compute_score(init_pool_sche[i])>obj_worst) obj_worst=compute_score(init_pool_sche[i]);
			}
			if(b2_index>=b2_size) break;

		}

		if(b2_index<b2_size){
			for(int i=1;i<init_pool_size;i++){
				if(isDeleted[i]!=1){
					array_copy(init_pool_indv[i],indv_b2[b2_index],task_num);
					array_copy(init_pool_sche[i],sche_b2[b2_index],task_num);
					isDeleted[i]=1;
					b2_index++;

					if(compute_score(init_pool_sche[i])<obj_best) obj_best=compute_score(init_pool_sche[i]);
					if(compute_score(init_pool_sche[i])>obj_worst) obj_worst=compute_score(init_pool_sche[i]);
				}
				if(b2_index>=b2_size) break;
			}
		}

		for(int i=0;i<init_pool_size;i++){
			delete[] init_pool_indv[i];
			delete[] init_pool_sche[i];
		}
		delete[] init_pool_indv;
		delete[] init_pool_sche;
		delete[] LFT_priority;
		delete[] isDeleted;
	}

	//evolution
	void evolution(){
		//cout<<endl<<sche_b1[0][task_num-1];

		for(int iter=0;iter<niter;iter++){
			cout<<endl<<iter;
			int new_pool_size=b1_size*(b1_size-1)+b1_size*b2_size+b1_size;
			int **new_pool;
			new_pool=new int*[new_pool_size];
			int **new_pool_sche;
			new_pool_sche=new int*[new_pool_size];
			int *new_pool_label;
			new_pool_label=new int[new_pool_size];
			for(int i=0;i<new_pool_size;i++) new_pool_label[i]=0;
			for(int i=0;i<new_pool_size;i++){
				new_pool[i]=new int[task_num];
				new_pool_sche[i]=new int[task_num];
			}
			int new_pool_index=0;

			for(int i=0;i<b1_size;i++){
				if(label_b1[i]!=1) continue;
				for(int j=0;j<i;j++){
					if(label_b1[j]==1) continue;
					crossover(indv_b1[i],indv_b1[j],new_pool[new_pool_index],new_pool[new_pool_index+1]);

					gen_activityList_NSRK(new_pool[new_pool_index],new_pool[new_pool_index]);
					SGS(new_pool[new_pool_index],new_pool_sche[new_pool_index]);
					right_justification(new_pool_sche[new_pool_index]);
					left_justification(new_pool_sche[new_pool_index]);
					getSRK(new_pool_sche[new_pool_index],new_pool[new_pool_index]);

					gen_activityList_NSRK(new_pool[new_pool_index+1],new_pool[new_pool_index+1]);
					SGS(new_pool[new_pool_index+1],new_pool_sche[new_pool_index+1]);
					right_justification(new_pool_sche[new_pool_index+1]);
					left_justification(new_pool_sche[new_pool_index+1]);
					getSRK(new_pool_sche[new_pool_index+1],new_pool[new_pool_index+1]);

					new_pool_index=new_pool_index+2;

					
					
				}
				for(int j=i+1;j<b1_size;j++){
					crossover(indv_b1[i],indv_b1[j],new_pool[new_pool_index],new_pool[new_pool_index+1]);

					gen_activityList_NSRK(new_pool[new_pool_index],new_pool[new_pool_index]);
					SGS(new_pool[new_pool_index],new_pool_sche[new_pool_index]);
					right_justification(new_pool_sche[new_pool_index]);
					left_justification(new_pool_sche[new_pool_index]);
					getSRK(new_pool_sche[new_pool_index],new_pool[new_pool_index]);

					gen_activityList_NSRK(new_pool[new_pool_index+1],new_pool[new_pool_index+1]);
					SGS(new_pool[new_pool_index+1],new_pool_sche[new_pool_index+1]);
					right_justification(new_pool_sche[new_pool_index+1]);
					left_justification(new_pool_sche[new_pool_index+1]);
					getSRK(new_pool_sche[new_pool_index+1],new_pool[new_pool_index+1]);

					new_pool_index=new_pool_index+2;
				}
			}

		
			for(int i=0;i<b1_size;i++){
				for(int j=0;j<b2_size;j++){
					double *child=new double[task_num];
					em_combination(indv_b1[i],indv_b2[j],obj_best,obj_worst,compute_score(sche_b1[i]),compute_score(sche_b2[j]),child);
					gen_activityList_NSRK(child,new_pool[new_pool_index]);
					SGS(new_pool[new_pool_index],new_pool_sche[new_pool_index]);
					right_justification(new_pool_sche[new_pool_index]);
					left_justification(new_pool_sche[new_pool_index]);
					getSRK(new_pool_sche[new_pool_index],new_pool[new_pool_index]);
					delete[] child;
					new_pool_index++;
				}
			}
			
			for(int i=0;i<new_pool_size;i++){
				new_pool_label[i]=1;
				for(int j=0;j<b1_size;j++){
					if(new_pool_sche[i][task_num-1]!=sche_b1[j][task_num-1]) continue;
					if(is_same(new_pool[i],indv_b1[j])){
						new_pool_label[i]=-1;
						break;
					}
				}
			}
			

			for(int i=0;i<b1_size;i++){
				array_copy(indv_b1[i],new_pool[new_pool_index],task_num);
				array_copy(sche_b1[i],new_pool_sche[new_pool_index],task_num);
				new_pool_index++;
			}

			population_sorting(new_pool_sche,new_pool,new_pool_label,new_pool_index);

			int *isDeleted=new int[new_pool_index];
			for(int i=0;i<new_pool_index;i++){
				isDeleted[i]=0;
			}
			array_copy(new_pool[0],indv_b1[0],task_num);
			array_copy(new_pool_sche[0],sche_b1[0],task_num);
			isDeleted[0]=1;
			if(new_pool_label[0]==1){
				label_b1[0]=1;
			}
			else{
				label_b1[0]=0;
			}
			obj_best=obj_worst=compute_score(sche_b1[0]);
			int b1_index=1;


			for(int i=1;i<new_pool_size;i++){
				if(new_pool_label[i]==-1) continue;
				bool diverse_enough=true;
				for(int j=0;j<b1_index;j++){
					if(get_distance(indv_b1[j],new_pool[i])<b1_threshold){
						diverse_enough=false;
						break;
					}
				}
				if(diverse_enough){
					array_copy(new_pool[i],indv_b1[b1_index],task_num);
					array_copy(new_pool_sche[i],sche_b1[b1_index],task_num);
					label_b1[b1_index]=new_pool_label[i];
					isDeleted[i]=1;
					b1_index++;

					if(compute_score(new_pool_sche[i])<obj_best) obj_best=compute_score(new_pool_sche[i]);
					if(compute_score(new_pool_sche[i])>obj_worst) obj_worst=compute_score(new_pool_sche[i]);
				}
				if(b1_index>=b1_size) break;
			}

			if(b1_index<b1_size){
				for(int i=1;i<new_pool_size;i++){
					if(isDeleted[i]!=1&&new_pool_label[i]!=-1){
						array_copy(new_pool[i],indv_b1[b1_index],task_num);
						array_copy(new_pool_sche[i],sche_b1[b1_index],task_num);
						label_b1[b1_index]=new_pool_label[i];
						isDeleted[i]=1;
						b1_index++;

						if(compute_score(new_pool_sche[i])<obj_best) obj_best=compute_score(new_pool_sche[i]);
						if(compute_score(new_pool_sche[i])>obj_worst) obj_worst=compute_score(new_pool_sche[i]);
					}
					if(b1_index>=b1_size) break;
				}
			}

			int b2_index=0;
			for(int i=0;i<new_pool_size;i++){
				if(isDeleted[i]==1||new_pool_label[i]==-1) continue;

				bool diverse_enough=true;
				for(int j=0;j<b1_size;j++){
					if(get_distance(indv_b1[j],new_pool[i])<b2_threshold){
						diverse_enough=false;
						break;
					}
				}
				if(diverse_enough){
					for(int j=0;j<b2_index;j++){
						if(get_distance(indv_b2[j],new_pool[i])<b2_threshold){
							diverse_enough=false;
							break;
						}
					}
				}
				if(diverse_enough){
					array_copy(new_pool[i],indv_b2[b2_index],task_num);
					array_copy(new_pool_sche[i],sche_b2[b2_index],task_num);
					isDeleted[i]=1;
					b2_index++;

					if(compute_score(new_pool_sche[i])<obj_best) obj_best=compute_score(new_pool_sche[i]);
					if(compute_score(new_pool_sche[i])>obj_worst) obj_worst=compute_score(new_pool_sche[i]);
				}
				if(b2_index>=b2_size) break;
			}

			if(b2_index<b2_size){
				for(int i=0;i<new_pool_size;i++){
					if(isDeleted[i]!=1&&new_pool_label[i]!=-1){
						array_copy(new_pool[i],indv_b2[b2_index],task_num);
						array_copy(new_pool_sche[i],sche_b2[b2_index],task_num);
						isDeleted[i]=1;
						b2_index++;

						if(compute_score(new_pool_sche[i])<obj_best) obj_best=compute_score(new_pool_sche[i]);
						if(compute_score(new_pool_sche[i])>obj_worst) obj_worst=compute_score(new_pool_sche[i]);
					}
					if(b2_index>=b2_size) break;
				}
			}

			for(int i=0;i<new_pool_size;i++){
				delete[] new_pool[i];
				delete[] new_pool_sche[i];
			}

			delete[] new_pool;
			delete[] new_pool_sche;
			delete[] new_pool_label;
			delete[] isDeleted;

			//cout<<endl<<sche_b1[0][task_num-1];
		}
	}

	//finalize, record the output
	void finalize(string filePath,int last_time){
		FILE* file_write=fopen(filePath.c_str(),"w");
		fprintf(file_write,"time_cost:%dms\n",clock()-last_time);
		fprintf(file_write,"ontime_proportion:%f\n",compute_ontime_proportion(sche_b1[0]));
		fprintf(file_write,"max_delay:%d\n",compute_max_delay(sche_b1[0]));
		fprintf(file_write,"avg_delay:%f\n",compute_avg_delay(sche_b1[0]));
		fprintf(file_write,"avg_flowtime:%f\n",compute_avg_flowtime(sche_b1[0]));
		fprintf(file_write,"object function value:%f\n",compute_score(sche_b1[0]));
		for(int i=0;i<task_num;i++)
		{
			fprintf(file_write,"%d\n",sche_b1[0][i]);	
		}	
		fclose(file_write);
	}

	//clear the memory
	void clear(){
		for(int i=0;i<b1_size;i++){
			delete[] indv_b1[i];
			delete[] sche_b1[i];
		}
		for(int i=0;i<b2_size;i++){
			delete[] indv_b2[i];
			delete[] sche_b2[i];
		}

		delete[] indv_b1;
		delete[] sche_b1;
		delete[] indv_b2;
		delete[] sche_b2;
		delete[] label_b1;
	}

	//TODO: processing the data
	void process(string filePath,string outPath){
		int last_time=clock();
		read(filePath);
		dataPreProcess();
		initialize();
		evolution();
		/*phase1();
		phase2();*/
		finalize(outPath,last_time);
		clear();
	}

	void HSS(int start_i,int start_j){
		int last_time;
		char path[MAX_PATH];   
		_getcwd(path, MAX_PATH);
		string filePath;
		string outPath;

		{
			cout<<"processing start\n";
			for(int i=start_i;i<5;i++)
			{
				for(int j=start_j;j<20;j++)
				{
					strstream ss1,ss2;
					ss1<<i;
					ss2<<j;
					string s1,s2;
					ss1>>s1;
					ss2>>s2;
					outPath=filePath=path;
					filePath+="\\dataInput_"+s1+"_"+s2+".txt";
					outPath+="\\dataInput_"+s1+"_"+s2+"_HSSresult.txt";
					last_time=clock();
					cout<<"processing dataInput_"<<i<<"_"<<j<<" ...\n";
					process(filePath,outPath);
					cout<<"dataInput_"<<i<<"_"<<j<<" has been processed with "<<clock()-last_time<<" ms\n";

				}
			}
			cout<<"\n\n\n";
		}
	}
}