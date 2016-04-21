#include "functions.h"
#include "variables.h"
#include "readData.h"
#include <queue>
#include <list>

namespace HGA_name{
	double lthreshold=0.75;
	double uthreshold=0.9;
	double pmutation=0.05;

	int POPsize=250;
	int nsche=50000;
	int niter=5;
	double beta=0.3;
	double pi=0.5;

	int **indv_ph1;
	int **indv_ph2;
	int **s_ph1;
	int **s_ph2;

	void get_activityList(int *stime,int *AL){
		int *stime_tem=new int[task_num];
		for(int i=0;i<task_num;i++){
			AL[i]=i;
			stime_tem[i]=stime[i];
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
		delete[] stime_tem;
	}

	boolean canBeScheduled(int i,int *isScheduled)
	{
		for(int k=0;k<P_num[i];k++)
		{
			if(isScheduled[P[i][k]]==-1) return false;
		}
		return true;
	}

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

	//generate activity list
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
			for(int j=0;j<task_num-i-1;j++)
			{
				if(x[j]<=x[j+1])
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
		/*for(int i=0;i<task_num;i++) cout<<endl<<i<<" "<<activityList[i]<<" "<<stime[i];
		int xx;cin>>xx;*/
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

	//search for peak
	void search_peak(int *stime,int *AL,list<list<int>>* peak_list){
		int **R_consume=new int*[stime[task_num-1]];
		double *RUR=new double[stime[task_num-1]];

		for(int t=0;t<stime[task_num-1];t++){
			R_consume[t]=new int[R_num];
			for(int i=0;i<R_num;i++){
				R_consume[t][i]=0;
			}
		}
		for(int i=0;i<task_num;i++){
			for(int t=stime[i];t<stime[i]+d[i];t++){
				for(int j=0;j<R_num;j++){
					R_consume[t][j]+=r[i][j];
				}
			}
		}
		for(int t=0;t<stime[task_num-1];t++){
			RUR[t]=0;
			for(int j=0;j<R_num;j++){
				RUR[t]+=(double)R_consume[t][j]/R[j];
			}
			RUR[t]=RUR[t]/R_num;
		}
		
		double threshold=my_rand()*(uthreshold-lthreshold)+lthreshold;
		
		int* peak_mark=new int[stime[task_num-1]];
		int count=0;
		if(RUR[0]>=threshold){
			count++;
		}
		peak_mark[0]=count;
		
		for(int t=1;t<stime[task_num-1];t++){
			if(RUR[t-1]<threshold&&RUR[t]>=threshold) count++;
			if(RUR[t]>=threshold){
				peak_mark[t]=count;
			}
			else{
				peak_mark[t]=-count;
			}
		}

		
		int start_time=0;
		while(true){
			if(count==0) break;

			list<int>* peak=new list<int>[count];
			
			for(int i=0;i<task_num;i++){
				if(d[AL[i]]==0||stime[AL[i]]<start_time) continue;
				int start_mark=peak_mark[stime[AL[i]]];
				int end_mark=peak_mark[stime[AL[i]]+d[AL[i]]-1];
				if(end_mark==0) continue;
				if(start_mark==end_mark&&start_mark<0) continue;
				if(start_mark==0) start_mark=1;

				if(start_mark<0) start_mark=-start_mark+1;
				if(end_mark<0) end_mark=-end_mark;
				for(int j=start_mark;j<=end_mark;j++){
					peak[j-1].push_back(AL[i]);
				}
			}

			int maximal_pos=0;
			for(int i=0;i<count;i++){
				if(peak[i].size()>peak[maximal_pos].size()) maximal_pos=i;
			}

			if(peak[maximal_pos].size()==0){
				delete[] peak;
				break;
			}
			peak_list[0].push_back(peak[maximal_pos]);
			while(!peak[maximal_pos].empty()){
				int tem_st=stime[peak[maximal_pos].front()]+d[peak[maximal_pos].front()];
				if(start_time<tem_st) start_time=tem_st;
				peak[maximal_pos].pop_front();
			}

			delete[] peak;
			
		}

		for(int t=0;t<stime[task_num-1];t++){
			delete[] R_consume[t];
		}
		delete[] R_consume;
		delete[] RUR;
		delete[] peak_mark;
	}

	//crossover
	void crossover_op(int *father_AL,int *father_stime,int *mother_AL,int *mother_stime,int *new_AL){
		list<list<int>> *peak_list=new list<list<int>>[1];
		search_peak(father_stime,father_AL,peak_list);
		if(peak_list[0].empty()){
			array_copy(father_AL,new_AL,task_num);
			delete[] peak_list;
			return;
		}
		int* belong_to_peak=new int[task_num];
		for(int i=0;i<task_num;i++){
			belong_to_peak[i]=0;
		}
		for(list <list<int>>::iterator peak=peak_list[0].begin();peak!=peak_list[0].end();peak++){
			for(list<int>::iterator activity=(*peak).begin();activity!=(*peak).end();activity++){
				belong_to_peak[*activity]=1;
			}
		}

		int* is_scheduled=new int[task_num];
		for(int i=0;i<task_num;i++) is_scheduled[i]=-1;
		new_AL[0]=0;
		new_AL[task_num-1]=task_num-1;
		is_scheduled[0]=0;
		is_scheduled[task_num-1]=task_num-1;


		int h=1;
		for(list <list<int>>::iterator peak=peak_list[0].begin();peak!=peak_list[0].end();peak++){
			int pos=0;
			for(int i=0;i<task_num;i++){
				if(father_AL[i]==(*peak).front()){
					pos=i;
					break;
				}
			}
			for(int j=1;j<pos;j++){
				if(is_scheduled[mother_AL[j]]==-1&&belong_to_peak[mother_AL[j]]==0&&canBeScheduled(mother_AL[j],is_scheduled)){
					new_AL[h]=mother_AL[j];
					is_scheduled[mother_AL[j]]=h;
					h++;
				}
			}

			list<int> pred;
			
			for(list<int>::iterator activity=(*peak).begin();activity!=(*peak).end();activity++){
				for(int i=0;i<P_num[*activity];i++) pred.push_back(P[*activity][i]);
			}
			pred.sort();
			pred.unique();

			
			while(true){
				int size=pred.size();
				list<int> tem_pred;
				for(list<int>::iterator activity=(pred).begin();activity!=(pred).end();activity++){
					for(int i=0;i<P_num[*activity];i++) tem_pred.push_back(P[*activity][i]);
				}
				tem_pred.sort();
				tem_pred.unique();
				for(list<int>::iterator activity=(tem_pred).begin();activity!=(tem_pred).end();activity++){
					pred.push_back(*activity);
				}
				
				pred.sort();
				pred.unique();

				if(pred.size()==size) break;
			}

			int *mother_scheduled_at=new int[task_num];
			for(int i=0;i<task_num;i++){
				mother_scheduled_at[mother_AL[i]]=i;
			}
			
			int * pred_array=new int[pred.size()];
			int i=0;
			for(list<int>::iterator activity=(pred).begin();activity!=(pred).end();activity++){
				pred_array[i]=*activity;
				i++;
			}

			for(int i=0;i<pred.size();i++){
				for(int j=0;j<pred.size()-i-1;j++){
					if(mother_scheduled_at[pred_array[j]]>mother_scheduled_at[pred_array[j+1]]){
						int tem=pred_array[j+1];
						pred_array[j+1]=pred_array[j];
						pred_array[j]=tem;
					}
				}
			}

			for(int i=0;i<pred.size();i++){
				if(is_scheduled[pred_array[i]]==-1&&belong_to_peak[pred_array[i]]==0&&canBeScheduled(pred_array[i],is_scheduled)){
					new_AL[h]=pred_array[i];
					is_scheduled[pred_array[i]]=h;
					h++;
				}
			}

			for(list<int>::iterator activity=(*peak).begin();activity!=(*peak).end();activity++){
				new_AL[h]=*activity;
				is_scheduled[*activity]=h;
				h++;
			}

			
			delete[] mother_scheduled_at;
			delete[] pred_array;
		}

		for(int j=1;j<task_num-1;j++){
			if(is_scheduled[mother_AL[j]]==-1&&belong_to_peak[mother_AL[j]]==0&&canBeScheduled(mother_AL[j],is_scheduled)){
				new_AL[h]=mother_AL[j];
				is_scheduled[mother_AL[j]]=h;
				h++;
			}
		}


		delete[] peak_list;
		delete[] belong_to_peak;
		delete[] is_scheduled;
	}

	//mutation
	void mutation_op(int *AL){
		for(int i=1;i<task_num-2;i++){
			if(my_rand()<pmutation){
				bool constrained=false;
				for(int j=0;j<P_num[AL[i+1]];j++){
					if(AL[i]==P[AL[i+1]][j]){
						constrained=true;
						break;
					}
				}
				if(!constrained){
					int tem=AL[i+1];
					AL[i+1]=AL[i];
					AL[i]=tem;
				}
			}
		}
	}

	//phase 2, genrate neighbour's population
	void beta_BRSM(int *AL,int *x){
		int *isScheduled=new int[task_num];
		for(int i=0;i<task_num;i++) isScheduled[i]=0;
		x[0]=0;
		x[task_num-1]=task_num-1;
		isScheduled[0]=1;
		isScheduled[task_num-1]=1;

		double *priority=new double[task_num];
		for(int i=1;i<task_num-1;i++){
			priority[AL[i]]=(double)1/i;
		}

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

			double max_priority=0;
			int max_priority_index=0;
			for(int i=1;i<task_num-1;i++){
				if(modi_prio[i]>max_priority){
					max_priority=modi_prio[i];
					max_priority_index=i;
				}
			}

			if(my_rand()<beta){
				x[pos]=max_priority_index;
				isScheduled[max_priority_index]=1;
			}
			else{
				modi_prio[max_priority_index]=0;
				if(array_sum(modi_prio,task_num)==0){
					x[pos]=max_priority_index;
					isScheduled[max_priority_index]=1;
				}
				else{
					double stop=my_rand()*array_sum(modi_prio,task_num);
					for(int i=0;i<task_num;i++){
						if(stop<array_sum(modi_prio,i+1)){
							x[pos]=i;
							isScheduled[i]=1;
							break;
						}
					}
				}
			}
			
			delete[] modi_prio;
		}
		

		delete[] isScheduled;
		delete[] priority;
	}


	//initialize the population
	void initialize(){
		indv_ph1=new int*[POPsize];
		indv_ph2=new int*[POPsize/2];
		s_ph1=new int*[POPsize];
		s_ph2=new int*[POPsize/2];

		for(int i=0;i<POPsize;i++){
			indv_ph1[i]=new int[task_num];
			s_ph1[i]=new int[task_num];
		}

		for(int i=0;i<POPsize/2;i++){
			indv_ph2[i]=new int[task_num];
			s_ph2[i]=new int[task_num];
		}

		double *LFT_priority=new double[task_num];
		for(int i=0;i<task_num;i++){
			LFT_priority[i]=1/(double)LF[i];
			if(status[i]==2) LFT_priority[i]=my_rand()*5+5;
		}
		

		for(int i=0;i<POPsize;i++){
			sampling(indv_ph1[i],LFT_priority);
			SGS(indv_ph1[i],s_ph1[i]);
			right_justification(s_ph1[i]);
			left_justification(s_ph1[i]);
		}
		for(int i=0;i<POPsize;i++){
			get_activityList(s_ph1[i],indv_ph1[i]);
		}

		delete[] LFT_priority;
	}

	//sort the population
	void population_sorting(int **stime,int **AL,int population_size){
		int *tem_stime=new int[task_num];
		int *tem_AL=new int[task_num];

		for(int i=0;i<population_size;i++){
			for(int j=0;j<population_size-i-1;j++){
				if(compute_score(stime[j])>compute_score(stime[j+1])){
					array_copy(stime[j+1],tem_stime,task_num);
					array_copy(stime[j],stime[j+1],task_num);
					array_copy(tem_stime,stime[j],task_num);
					array_copy(AL[j+1],tem_AL,task_num);
					array_copy(AL[j],AL[j+1],task_num);
					array_copy(tem_AL,AL[j],task_num);
				}
			}
		}

		delete[] tem_stime;
		delete[] tem_AL;
	}

	//TODO: phase1 evolution
	void phase1(){
		compute_LF();
		initialize();
		for(int iter=0;iter<niter;iter++){
			population_sorting(s_ph1,indv_ph1,POPsize);

			int** cp=new int*[(int)(pi*POPsize/2)];
			int* is_selected=new int[POPsize];
			for(int i=0;i<POPsize;i++) is_selected[i]=0;

			for(int cp_index=0;cp_index<(int)(pi*POPsize/2);cp_index++){
				cp[cp_index]=new int[2];
				for(int i=0;i<POPsize;i++){
					if(is_selected[i]==0){
						cp[cp_index][0]=i;
						is_selected[i]=1;
						break;
					}
				}

				int rand_num=rand()%POPsize;
				while(is_selected[rand_num]==1){
					rand_num=rand()%POPsize;
				}
				cp[cp_index][1]=rand_num;
				is_selected[rand_num]=1;
			}

			int **cp_stime=new int*[((int)(pi*POPsize/2))*2];
			int **cp_AL=new int*[((int)(pi*POPsize/2))*2];
			for(int i=0;i<((int)(pi*POPsize/2))*2;i++){
				cp_stime[i]=new int[task_num];
				cp_AL[i]=new int[task_num];
			}

			for(int cp_index=0;cp_index<(int)(pi*POPsize/2);cp_index++){
				crossover_op(indv_ph1[cp[cp_index][0]],s_ph1[cp[cp_index][0]],indv_ph1[cp[cp_index][1]],s_ph1[cp[cp_index][1]],cp_AL[2*cp_index]);
				crossover_op(indv_ph1[cp[cp_index][1]],s_ph1[cp[cp_index][1]],indv_ph1[cp[cp_index][0]],s_ph1[cp[cp_index][0]],cp_AL[2*cp_index+1]);
				
				mutation_op(cp_AL[2*cp_index]);
				mutation_op(cp_AL[2*cp_index+1]);

				SGS(cp_AL[2*cp_index],cp_stime[2*cp_index]);
				right_justification(cp_stime[2*cp_index]);
				left_justification(cp_stime[2*cp_index]);
				get_activityList(cp_stime[2*cp_index],cp_AL[2*cp_index]);

				SGS(cp_AL[2*cp_index+1],cp_stime[2*cp_index+1]);
				right_justification(cp_stime[2*cp_index+1]);
				left_justification(cp_stime[2*cp_index+1]);
				get_activityList(cp_stime[2*cp_index+1],cp_AL[2*cp_index+1]);
			}

			population_sorting(cp_stime,cp_AL,((int)(pi*POPsize/2))*2);

			int cp_index=0;
			for(int i=0;i<POPsize&&cp_index<((int)(pi*POPsize/2))*2;i++){
				if(compute_score(cp_stime[cp_index])<compute_score(s_ph1[i])){
					array_copy(cp_stime[cp_index],s_ph1[i],task_num);
					array_copy(cp_AL[cp_index],indv_ph1[i],task_num);
					cp_index++;
				}
			}

			for(int i=0;i<(int)(pi*POPsize/2);i++) delete[] cp[i];
			delete[] cp;
			delete[] is_selected;
			for(int i=0;i<((int)(pi*POPsize/2))*2;i++){
				delete[] cp_stime[i];
				delete[] cp_AL[i];
			}
			delete[] cp_stime;
			delete[] cp_AL;
		}

		population_sorting(s_ph1,indv_ph1,POPsize);
		//cout<<endl<<"time:"<<s_ph1[0][task_num-1]<<endl;
	}

	//TODO: phase2 evolution
	void phase2(){
		for(int i=0;i<POPsize/2;i++){
			beta_BRSM(indv_ph1[0],indv_ph2[i]);
			SGS(indv_ph2[i],s_ph2[i]);
			right_justification(s_ph2[i]);
			left_justification(s_ph2[i]);
			get_activityList(s_ph2[i],indv_ph2[i]);
		}

		for(int iter=0;iter<niter;iter++){
			population_sorting(s_ph2,indv_ph2,POPsize/2);

			int** cp=new int*[(int)(pi*POPsize/4)];
			int* is_selected=new int[POPsize/2];
			for(int i=0;i<POPsize/2;i++) is_selected[i]=0;

			for(int cp_index=0;cp_index<(int)(pi*POPsize/4);cp_index++){
				cp[cp_index]=new int[2];
				for(int i=0;i<POPsize/2;i++){
					if(is_selected[i]==0){
						cp[cp_index][0]=i;
						is_selected[i]=1;
						break;
					}
				}

				int rand_num=rand()%(POPsize/2);
				while(is_selected[rand_num]==1){
					rand_num=rand()%(POPsize/2);
				}
				cp[cp_index][1]=rand_num;
				is_selected[rand_num]=1;
			}

			int **cp_stime=new int*[((int)(pi*POPsize/4))*2];
			int **cp_AL=new int*[((int)(pi*POPsize/4))*2];
			for(int i=0;i<((int)(pi*POPsize/4))*2;i++){
				cp_stime[i]=new int[task_num];
				cp_AL[i]=new int[task_num];
			}

			for(int cp_index=0;cp_index<(int)(pi*POPsize/4);cp_index++){
				crossover_op(indv_ph2[cp[cp_index][0]],s_ph2[cp[cp_index][0]],indv_ph2[cp[cp_index][1]],s_ph2[cp[cp_index][1]],cp_AL[2*cp_index]);
				crossover_op(indv_ph2[cp[cp_index][1]],s_ph2[cp[cp_index][1]],indv_ph2[cp[cp_index][0]],s_ph2[cp[cp_index][0]],cp_AL[2*cp_index+1]);
				
				mutation_op(cp_AL[2*cp_index]);
				mutation_op(cp_AL[2*cp_index+1]);

				SGS(cp_AL[2*cp_index],cp_stime[2*cp_index]);
				right_justification(cp_stime[2*cp_index]);
				left_justification(cp_stime[2*cp_index]);
				get_activityList(cp_stime[2*cp_index],cp_AL[2*cp_index]);

				SGS(cp_AL[2*cp_index+1],cp_stime[2*cp_index+1]);
				right_justification(cp_stime[2*cp_index+1]);
				left_justification(cp_stime[2*cp_index+1]);
				get_activityList(cp_stime[2*cp_index+1],cp_AL[2*cp_index+1]);
			}

			population_sorting(cp_stime,cp_AL,((int)(pi*POPsize/4))*2);

			int cp_index=0;
			for(int i=0;i<POPsize/2&&cp_index<((int)(pi*POPsize/4))*2;i++){
				if(compute_score(cp_stime[cp_index])<compute_score(s_ph2[i])){
					array_copy(cp_stime[cp_index],s_ph2[i],task_num);
					array_copy(cp_AL[cp_index],indv_ph2[i],task_num);
					cp_index++;
				}
			}

			for(int i=0;i<(int)(pi*POPsize/4);i++) delete[] cp[i];
			delete[] cp;
			delete[] is_selected;
			for(int i=0;i<((int)(pi*POPsize/4))*2;i++){
				delete[] cp_stime[i];
				delete[] cp_AL[i];
			}
			delete[] cp_stime;
			delete[] cp_AL;
		}

		population_sorting(s_ph2,indv_ph2,POPsize/2);
		//cout<<endl<<"time:"<<s_ph2[0][task_num-1]<<endl;
	}

	//finalize, record the output
	void finalize(string filePath,int last_time){
		if(compute_score(s_ph1[0])<compute_score(s_ph2[0])){
			FILE* file_write=fopen(filePath.c_str(),"w");
			fprintf(file_write,"time_cost:%dms\n",clock()-last_time);
			fprintf(file_write,"ontime_proportion:%f\n",compute_ontime_proportion(s_ph1[0]));
			fprintf(file_write,"max_delay:%d\n",compute_max_delay(s_ph1[0]));
			fprintf(file_write,"avg_delay:%f\n",compute_avg_delay(s_ph1[0]));
			fprintf(file_write,"avg_flowtime:%f\n",compute_avg_flowtime(s_ph1[0]));
			fprintf(file_write,"object function value:%f\n",compute_score(s_ph1[0]));
			for(int i=0;i<task_num;i++)
			{
				fprintf(file_write,"%d\n",s_ph1[0][i]);	
			}	
			fclose(file_write);
		}
		else{
			FILE* file_write=fopen(filePath.c_str(),"w");
			fprintf(file_write,"time_cost:%dms\n",clock()-last_time);
			fprintf(file_write,"ontime_proportion:%f\n",compute_ontime_proportion(s_ph2[0]));
			fprintf(file_write,"max_delay:%d\n",compute_max_delay(s_ph2[0]));
			fprintf(file_write,"avg_delay:%f\n",compute_avg_delay(s_ph2[0]));
			fprintf(file_write,"avg_flowtime:%f\n",compute_avg_flowtime(s_ph2[0]));
			fprintf(file_write,"object function value:%f\n",compute_score(s_ph2[0]));
			for(int i=0;i<task_num;i++)
			{
				fprintf(file_write,"%d\n",s_ph2[0][i]);	
			}	
			fclose(file_write);
		}
	}

	void clear(){
		for(int i=0;i<POPsize;i++){
			delete[] indv_ph1[i];
			delete[] s_ph1[i];
		}
		for(int i=0;i<POPsize/2;i++){
			delete[] indv_ph2[i];
			delete[] s_ph2[i];
		}

		delete[] indv_ph1;
		delete[] indv_ph2;
		delete[] s_ph1;
		delete[] s_ph2;
	}

	//TODO: processing the data
	void process(string filePath,string outPath){
		int last_time=clock();
		read(filePath);
		dataPreProcess();
		phase1();
		phase2();
		finalize(outPath,last_time);
		clear();
	}

	void HGA(int start_i,int start_j){
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
					outPath+="\\dataInput_"+s1+"_"+s2+"_HGAresult.txt";
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

