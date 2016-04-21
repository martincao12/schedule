#include "functions.h"
#include "variables.h"
#include "readData.h"

namespace heuristic_name{
	int LFT=1;//denote whether to use LFT to initialize the swarm
	int for_num=2;//the number of particles in forward particle swarm
	int back_num=0;//the number of particles in backward particle swarm
	int iter_num=20;//the number of iterations
	double Chi=0.4;//the constriction factor in PSO
	int DJ=1;//denote whether to use the double justification
	int Mapping=0;//denote whether to use the mapping method after each iteration
	double GR=0;//gbest ratio
	int c1=2;
	int c2=2;

	double *gbest_for;//global best for forward paticle swarms
	double **lbest_for;//local best for forward particle swarms
	int gbest_for_value;
	int *lbest_for_value;
	double **x_for;//position for forward paticle swarms
	double **v_for;//volecity for forward paticle swarms
	double *gbest_back;//global best for backward paticle swarms
	double **lbest_back;//local best for backward particle swarms
	double **x_back;//position for backward paticle swarms
	double **v_back;//volecity for backward paticle swarms
	int gbest_back_value;
	int *lbest_back_value;
	int *gbest_for_St;
	int *gbest_back_St;

	//generate activity list
	template <class T>
	void gen_activityList(T *orging_x,int *activityList)
	{
		T *x=new T[task_num];
		array_copy(orging_x,x,task_num);
		for(int i=0;i<task_num;i++)
		{
			activityList[i]=i;
		}
		for(int i=0;i<task_num;i++)
		{
			for(int j=0;j<task_num-i-1;j++)
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

	boolean canBeScheduled(int i,int *isScheduled,int order)
	{
		if(order==0)
		{
			for(int k=0;k<P_num[i];k++)
			{
				if(isScheduled[P[i][k]]==-1) return false;
			}
			return true;
		}
		else
		{
			for(int k=0;k<S_num[i];k++)
			{
				if(isScheduled[S[i][k]]==-1) return false;
			}
			return true;
		}
	}

	//right justification
	void right_justification_out(int *stime)
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
	void left_justification_out(int *stime)
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

	//right justification
	void right_justification(int *Sg,int *Fg,int order)
	{
		if(order==0)
		{
			int *stime=new int[task_num];
			for(int i=0;i<task_num;i++) stime[Sg[i]]=Fg[i]-d[Sg[i]];
			right_justification_out(stime);
			for(int i=0;i<task_num;i++){Sg[i]=i;Fg[i]=stime[i]+d[i];}
			delete[] stime;
			//int *activityList=new int[task_num];
			//gen_activityList(Fg,activityList);
			//
			//int *activity_Fg=new int[task_num];
			//for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
			//for(int i=0;i<task_num;i++)
			//{
			//	if(Sg[activityList[i]]==task_num-1) continue;
			//	int tem_LF=LF[task_num-1];
			//	for(int j=0;j<S_num[Sg[activityList[i]]];j++)
			//	{
			//		int tem_int=activity_Fg[S[Sg[activityList[i]]][j]]-d[S[Sg[activityList[i]]][j]];
			//		if(tem_int<tem_LF) tem_LF=tem_int;
			//	}
			//	int *R_copy=new int[R_num];

			//	for(int t=tem_LF-d[Sg[activityList[i]]];t>=Fg[activityList[i]]-d[Sg[activityList[i]]];t--)
			//	{
			//		int rFeasible=1;
			//		{
			//			array_copy(R,R_copy,R_num);
			//			for(int l=0;l<task_num;l++)
			//			{
			//				if(l==activityList[i]) continue;
			//				if(Fg[l]-d[Sg[l]]<=t&&t<Fg[l])
			//				{
			//					for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
			//				}
			//			}
			//			for(int l=0;l<R_num;l++)
			//			{
			//				if(R_copy[l]<r[Sg[activityList[i]]][l])
			//				{
			//					rFeasible=0;
			//					break;
			//				}
			//			}
			//		}
			//		for(int s=0;s<task_num;s++)
			//		{
			//			if(s==activityList[i]) continue;
			//			//if(t<Fg[s]&&Fg[s]<t+d[Sg[activityList[i]]])
			//			//{
			//			//	array_copy(R,R_copy,R_num);
			//			//	for(int l=0;l<task_num;l++)
			//			//	{
			//			//		if(l==activityList[i]) continue;
			//			//		if(Fg[l]-d[Sg[l]]<=Fg[s]&&Fg[s]<Fg[l])
			//			//		{
			//			//			for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
			//			//		}
			//			//	}				
			//			//	for(int l=0;l<R_num;l++)
			//			//	{
			//			//		if(R_copy[l]<r[Sg[activityList[i]]][l])
			//			//		{
			//			//			rFeasible=0;
			//			//			break;
			//			//		}
			//			//	}
			//			//}
			//			if(t<Fg[s]-d[Sg[s]]&&Fg[s]-d[Sg[s]]<t+d[Sg[activityList[i]]])
			//			{
			//				array_copy(R,R_copy,R_num);
			//				for(int l=0;l<task_num;l++)
			//				{
			//					if(l==activityList[i]) continue;
			//					if(Fg[l]-d[Sg[l]]<=Fg[s]-d[Sg[s]]&&Fg[s]-d[Sg[s]]<Fg[l])
			//					{
			//						for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
			//					}
			//				}				
			//				for(int l=0;l<R_num;l++)
			//				{
			//					if(R_copy[l]<r[Sg[activityList[i]]][l])
			//					{
			//						rFeasible=0;
			//						break;
			//					}
			//				}
			//			}
			//		}

			//		if(rFeasible==1)
			//		{
			//			Fg[activityList[i]]=t+d[Sg[activityList[i]]];
			//			break;
			//		}
			//	}
			//	delete[] R_copy;
			//	for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
			//}
			////for(int i=task_num-1;i>=0;i--) Fg[i]-=Fg[0];
			//delete[] activityList;
			//delete[] activity_Fg;
		}
		else
		{
			int *activityList=new int[task_num];
			gen_activityList(Fg,activityList);
			int *activity_Fg=new int[task_num];
			for(int i=0;i<task_num;i++) activity_Fg[Sg[i]]=Fg[i];
			for(int i=0;i<task_num;i++)
			{
				if(Sg[activityList[i]]==0) continue;
				int tem_LF=LF[task_num-1];
				for(int j=0;j<P_num[Sg[activityList[i]]];j++)
				{
					int tem_int=activity_Fg[P[Sg[activityList[i]]][j]]-d[P[Sg[activityList[i]]][j]];
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
						//if(t<Fg[s]&&Fg[s]<t+d[Sg[activityList[i]]])
						//{
						//	array_copy(R,R_copy,R_num);
						//	for(int l=0;l<task_num;l++)
						//	{
						//		if(l==activityList[i]) continue;
						//		if(Fg[l]-d[Sg[l]]<=Fg[s]&&Fg[s]<Fg[l])
						//		{
						//			for(int m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
						//		}
						//	}				
						//	for(int l=0;l<R_num;l++)
						//	{
						//		if(R_copy[l]<r[Sg[activityList[i]]][l])
						//		{
						//			rFeasible=0;
						//			break;
						//		}
						//	}
						//}
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
			for(int i=task_num-1;i>=0;i--) Fg[i]-=Fg[0];
			delete[] activityList;
			delete[] activity_Fg;
		}
	}



	//left justification
	void left_justification(int *Sg,int *Fg,int order)
	{
		if(order==0)
		{
			int *stime=new int[task_num];
			for(int i=0;i<task_num;i++) stime[Sg[i]]=Fg[i]-d[Sg[i]];
			left_justification_out(stime);
			for(int i=0;i<task_num;i++){Sg[i]=i;Fg[i]=stime[i]+d[i];}
			delete[] stime;
			/*for(int i=0;i<task_num;i++) cout<<endl<<i<<" "<<Sg[i]<<" "<<Fg[i];
			for(int i=0;i<task_num;i++){
				if(status[Sg[i]]==3||Sg[i]==0) Fg[i]=0;
			}
			for(int i=0;i<task_num;i++) cout<<endl<<i<<" "<<Sg[i]<<" "<<Fg[i];
			
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
				if(standby_time[Sg[activityList[i]]]>tem_ES) tem_ES=Sg[activityList[i]];
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
			for(int i=0;i<task_num;i++) cout<<endl<<i<<" "<<Sg[i]<<" "<<Fg[i];
			int xx;cin>>xx;
			delete[] activityList;
			delete[] activity_Fg;*/
		}
		else
		{
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
				if(Sg[activityList[i]]==task_num-1) continue;
				int tem_ES=0;
				for(int j=0;j<S_num[Sg[activityList[i]]];j++)
				{
					int tem_int=activity_Fg[S[Sg[activityList[i]]][j]];
					if(tem_int>tem_ES) tem_ES=tem_int;
				}
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
						/*if(t<Fg[s]&&Fg[s]<t+d[Sg[activityList[i]]])
						{
							array_copy(R,R_copy,R_num);
							for(int l=0;l<task_num;l++)
							{
								if(l==activityList[i]) continue;
								if(Fg[l]-d[Sg[l]]<=Fg[s]&&Fg[s]<Fg[l])
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
						}*/
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
			delete[] activityList;
			delete[] activity_Fg;
		}
	}

	//SGS using forward method
	int SGS_for(double *x)
	{
		int *Sg=new int[task_num];
		int *Fg=new int[task_num];
		int *isScheduled=new int[task_num];
		int *activityList=new int[task_num];
		gen_activityList(x,activityList);
		for(int i=0;i<task_num;i++)
		{
			isScheduled[i]=-1;
		}
		Sg[0]=0; Fg[0]=0; isScheduled[0]=0;
		for(int i=1;i<task_num;i++)
		{
			for(int j=0;j<task_num;j++)
			{
				if(isScheduled[activityList[j]]!=-1) continue;
				if(!canBeScheduled(activityList[j],isScheduled,0)) continue;
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

		

		if(DJ==1)
		{
			right_justification(Sg,Fg,0);
			left_justification(Sg,Fg,0);
		}
		
		int *y=new int[task_num];
		for(int i=0;i<task_num;i++){
			y[Sg[i]]=Fg[i];
		}
		for(int i=0;i<task_num;i++) y[i]-=d[i];
		int result=compute_score(y);

		delete[] Sg;
		delete[] Fg;
		delete[] isScheduled;
		delete[] activityList;
		delete[] y;
		return result;
	}
	//SGS using backward method
	int SGS_back(double *x)
	{
		int *Sg=new int[task_num];
		int *Fg=new int[task_num];
		int *isScheduled=new int[task_num];
		int *activityList=new int[task_num];
		gen_activityList(x,activityList);
		for(int i=0;i<task_num;i++)
		{
			isScheduled[i]=-1;
		}
		Sg[0]=task_num-1; Fg[0]=0; isScheduled[task_num-1]=0;
		for(int i=1;i<task_num;i++)
		{
			for(int j=0;j<task_num;j++)
			{
				if(isScheduled[activityList[j]]!=-1) continue;
				if(!canBeScheduled(activityList[j],isScheduled,1)) continue;
				int *R_copy=new int[R_num];
				int Fgi_tem=LF[task_num-1];

				int ES_tem=0;
				for(int k=0;k<S_num[activityList[j]];k++)
				{
					int tem_int=Fg[isScheduled[S[activityList[j]][k]]];
					if(ES_tem<tem_int) ES_tem=tem_int;
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

		if(DJ==1)
		{
			right_justification(Sg,Fg,1);
			left_justification(Sg,Fg,1);
		}

		int result=Fg[task_num-1];
		delete[] Sg;
		delete[] Fg;
		delete[] isScheduled;
		delete[] activityList;
		return result;
	}

	void get_for_St(double *x,int *y)
	{
		int i=0,j=0,k=0,s=0,l=0,m=0;
		int *Sg=new int[task_num];
		int *Fg=new int[task_num];
		int *isScheduled=new int[task_num];
		int *activityList=new int[task_num];
		gen_activityList(x,activityList);
		for(i=0;i<task_num;i++)
		{
			isScheduled[i]=-1;
		}
		Sg[0]=0; Fg[0]=0; isScheduled[0]=0;
		for(i=1;i<task_num;i++)
		{
			for(j=0;j<task_num;j++)
			{
				if(isScheduled[activityList[j]]!=-1) continue;
				if(!canBeScheduled(activityList[j],isScheduled,0)) continue;
				int *R_copy=new int[R_num];
				int Fgi_tem=LF[task_num-1];

				int ES_tem=0;
				for(k=0;k<P_num[activityList[j]];k++)
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

				for(k=0;k<i;k++)
				{
					if(Fg[k]<ES_tem) continue;
					for(l=0;l<R_num;l++) R_copy[l]=R[l];
					int rFeasible=1;

					for(s=0;s<i;s++)
					{
						if(!(Fg[k]<=Fg[s]&&Fg[s]<Fg[k]+d[activityList[j]])) continue;
					
						for(l=0;l<i;l++)
						{
							if(Fg[l]-d[Sg[l]]<=Fg[s]&&Fg[s]<Fg[l])
							{
								for(m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
							}
						}				
						for(l=0;l<R_num;l++)
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

		if(DJ==1)
		{
			right_justification(Sg,Fg,0);
			left_justification(Sg,Fg,0);
		}

		for(i=0;i<task_num;i++) y[Sg[i]]=Fg[i];
		delete[] Sg;
		delete[] Fg;
		delete[] isScheduled;
		delete[] activityList;
		for(i=0;i<task_num;i++) y[i]-=d[i];
	}

	void get_back_St(double *x,int *y)
	{
		int i=0,j=0,k=0,s=0,l=0,m=0;
		int *Sg=new int[task_num];
		int *Fg=new int[task_num];
		int *isScheduled=new int[task_num];
		int *activityList=new int[task_num];
		gen_activityList(x,activityList);
		for(i=0;i<task_num;i++)
		{
			isScheduled[i]=-1;
		}
		Sg[0]=task_num-1; Fg[0]=0; isScheduled[task_num-1]=0;
		for(i=1;i<task_num;i++)
		{
			for(j=0;j<task_num;j++)
			{
				if(isScheduled[activityList[j]]!=-1) continue;
				if(!canBeScheduled(activityList[j],isScheduled,1)) continue;
				int *R_copy=new int[R_num];
				int Fgi_tem=LF[task_num-1];

				int ES_tem=0;
				for(k=0;k<S_num[activityList[j]];k++)
				{
					int tem_int=Fg[isScheduled[S[activityList[j]][k]]];
					if(ES_tem<tem_int) ES_tem=tem_int;
				}

				for(k=0;k<i;k++)
				{
					if(Fg[k]<ES_tem) continue;
					for(l=0;l<R_num;l++) R_copy[l]=R[l];
					int rFeasible=1;

					for(s=0;s<i;s++)
					{
						if(!(Fg[k]<=Fg[s]&&Fg[s]<Fg[k]+d[activityList[j]])) continue;
					
						for(l=0;l<i;l++)
						{
							if(Fg[l]-d[Sg[l]]<=Fg[s]&&Fg[s]<Fg[l])
							{
								for(m=0;m<R_num;m++) R_copy[m]-=r[Sg[l]][m];
							}
						}				
						for(l=0;l<R_num;l++)
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

		if(DJ==1)
		{
			right_justification(Sg,Fg,1);
			left_justification(Sg,Fg,1);
		}
	
		for(i=0;i<task_num;i++) y[Sg[i]]=Fg[i];
		int makespan=Fg[task_num-1];
		delete[] Sg;
		delete[] Fg;
		delete[] isScheduled;
		delete[] activityList;
		for(i=0;i<task_num;i++) y[i]=makespan-y[i];
		for(i=0;i<task_num;i++) y[i]+=d[i];
		int *tem_Sg=new int[task_num];
		for(i=0;i<task_num;i++) tem_Sg[i]=i;
		right_justification(tem_Sg,y,0);
		left_justification(tem_Sg,y,0);
		for(i=0;i<task_num;i++) y[i]-=d[i];
		delete[] tem_Sg;
	}

	//initialize the swarm
	void initialize()
	{
		gbest_for=new double[task_num];
		lbest_for=new double*[for_num];
		x_for=new double*[for_num];
		v_for=new double*[for_num];
		lbest_for_value=new int[for_num];
		gbest_back=new double[task_num];
		lbest_back=new double*[back_num];
		x_back=new double*[back_num];
		v_back=new double*[back_num];
		lbest_back_value=new int[back_num];

		gbest_for_St=new int[task_num];
		gbest_back_St=new int[task_num];

		for(int i=0;i<for_num;i++)
		{
			x_for[i]=new double[task_num];
			v_for[i]=new double[task_num];
			lbest_for[i]=new double[task_num];
			for(int j=0;j<task_num;j++)
			{
				x_for[i][j]=my_rand();
				lbest_for[i][j]=x_for[i][j];
				v_for[i][j]=my_rand()*2-1;
			}
		}

		for(int i=0;i<back_num;i++)
		{
			x_back[i]=new double[task_num];
			v_back[i]=new double[task_num];
			lbest_back[i]=new double[task_num];
			for(int j=0;j<task_num;j++)
			{
				x_back[i][j]=my_rand();
				lbest_back[i][j]=x_back[i][j];
				v_back[i][j]=my_rand()*2-1;
			}
		}
		
		if(LFT==1)
		{
			if(for_num!=0)
			{
				for(int j=0;j<task_num;j++)
				{
					x_for[0][j]=1/((double)LF[j]+0.00001);
					if(status[j]==2) x_for[0][j]=my_rand()*5+5;
					lbest_for[0][j]=x_for[0][j];
				}
			}
			if(back_num!=0)
			{
				for(int j=0;j<task_num;j++)
				{
					x_back[0][j]=1/((double)LF[j]+0.00001);
					if(status[j]==2) x_for[0][j]=my_rand()*5+5;
					lbest_back[0][j]=x_back[0][j];
				}
			}
		}

		if(for_num!=0)
		{
			gbest_for_value=lbest_for_value[0]=SGS_for(x_for[0]);
			array_copy(x_for[0],gbest_for,task_num);
			for(int i=1;i<for_num;i++)
			{
				lbest_for_value[i]=SGS_for(x_for[i]);
				if(lbest_for_value[i]<gbest_for_value)
				{
					gbest_for_value=lbest_for_value[i];
					array_copy(x_for[i],gbest_for,task_num);
				}
			}
		}

		if(back_num!=0)
		{
			gbest_back_value=lbest_back_value[0]=SGS_back(x_back[0]);
			array_copy(x_back[0],gbest_back,task_num);
			for(int i=1;i<back_num;i++)
			{
				lbest_back_value[i]=SGS_back(x_back[i]);
				if(lbest_back_value[i]<gbest_back_value)
				{
					gbest_back_value=lbest_back_value[i];
					array_copy(x_back[i],gbest_back,task_num);
				}
			}
		}
	}

	void evolution()
	{
		for(int i=0;i<iter_num;i++)
		{
			if(for_num!=0)
			{
				for(int j=0;j<for_num;j++)
				{
					double *Y=new double[task_num];
					if(my_rand()<GR)
					{
						array_copy(gbest_for,Y,task_num);
					}
					else
					{
						if(j>0&&j<for_num-1)
						{
							int index=lbest_for_value[j-1]<lbest_for_value[j]?j-1:j;
							index=lbest_for_value[index]<lbest_for_value[j+1]?index:j+1;
							array_copy(lbest_for[index],Y,task_num);
						}
						if(j==0)
						{
							int index=lbest_for_value[for_num-1]<lbest_for_value[j]?for_num-1:j;
							index=lbest_for_value[index]<lbest_for_value[j+1]?index:j+1;
							//for(int ss=0;ss<for_num;ss++) cout<<lbest_for[index]<<" ";
							array_copy(lbest_for[index],Y,task_num);
						}
						if(j==for_num-1)
						{
							int index=lbest_for_value[j-1]<lbest_for_value[j]?j-1:j;
							index=lbest_for_value[index]<lbest_for_value[0]?index:0;
							array_copy(lbest_for[index],Y,task_num);
						}
					}
					for(int k=0;k<task_num;k++)
					{
						v_for[j][k]=Chi*(v_for[j][k]+c1*my_rand()*(lbest_for[j][k]-x_for[j][k])+c2*my_rand()*(Y[k]-x_for[j][k]));
						if(v_for[j][k]>1) v_for[j][k]=1;
						if(v_for[j][k]<-1) v_for[j][k]=-1;
						x_for[j][k]+=v_for[j][k];
						if(x_for[j][k]>1) x_for[j][k]=1;
						if(x_for[j][k]<0) x_for[j][k]=0;
					}

					int tem_int=SGS_for(x_for[j]);
					if(tem_int<lbest_for_value[j]) 
					{
						lbest_for_value[j]=tem_int;
						array_copy(x_for[j],lbest_for[j],task_num);
					}
					if(lbest_for_value[j]<gbest_for_value)
					{
						gbest_for_value=lbest_for_value[j];
						array_copy(lbest_for[j],gbest_for,task_num);
					}

					delete[] Y;
				}		
			}

			if(back_num!=0)
			{
				for(int j=0;j<back_num;j++)
				{
					double *Y=new double[task_num];
					if(my_rand()<GR)
					{
						array_copy(gbest_back,Y,task_num);
					}
					else
					{
						if(j>0&&j<back_num-1)
						{
							int index=lbest_back_value[j-1]<lbest_back_value[j]?j-1:j;
							index=lbest_back_value[index]<lbest_back_value[j+1]?index:j+1;
							array_copy(lbest_back[index],Y,task_num);
						}
						if(j==0)
						{
							int index=lbest_back_value[back_num-1]<lbest_back_value[j]?back_num-1:j;
							index=lbest_back_value[index]<lbest_back_value[j+1]?index:j+1;
							array_copy(lbest_back[index],Y,task_num);
						}
						if(j==back_num-1)
						{
							int index=lbest_back_value[j-1]<lbest_back_value[j]?j-1:j;
							index=lbest_back_value[index]<lbest_back_value[0]?index:0;
							array_copy(lbest_back[index],Y,task_num);
						}
					}
					for(int k=0;k<task_num;k++)
					{
						v_back[j][k]=Chi*(v_back[j][k]+c1*my_rand()*(lbest_back[j][k]-x_back[j][k])+c2*my_rand()*(Y[k]-x_back[j][k]));
						if(v_back[j][k]>1) v_back[j][k]=1;
						if(v_back[j][k]<-1) v_back[j][k]=-1;
						x_back[j][k]+=v_back[j][k];
						if(x_back[j][k]>1) x_back[j][k]=1;
						if(x_back[j][k]<0) x_back[j][k]=0;
					}

					int tem_int=SGS_back(x_back[j]);
					if(tem_int<lbest_back_value[j]) 
					{
						lbest_back_value[j]=tem_int;
						array_copy(x_back[j],lbest_back[j],task_num);
					}
					if(lbest_back_value[j]<gbest_back_value)
					{
						gbest_back_value=lbest_back_value[j];
						array_copy(lbest_back[j],gbest_back,task_num);
					}

					delete[] Y;
				}			
			}
		}
	}

	void finalize(string filePath,int last_time)
	{
		int gbest_value=0;
		if(for_num!=0&&back_num!=0)
		{
			gbest_value=gbest_for_value<gbest_back_value?gbest_for_value:gbest_back_value;
		}
		else if(for_num!=0)
		{
			gbest_value=gbest_for_value;
		}
		else if(back_num!=0)
		{
			gbest_value=gbest_back_value;
		}
		else{}

		FILE* file_write=fopen(filePath.c_str(),"w");
		fprintf(file_write,"time_cost:%dms\n",clock()-last_time);	
		if(for_num!=0&&back_num!=0)
		{
			get_for_St(gbest_for,gbest_for_St);
			get_back_St(gbest_back,gbest_back_St);
			if(gbest_for_value<gbest_back_value)
			{
				for(int i=0;i<task_num;i++)
				{
					fprintf(file_write,"%d\n",gbest_for_St[i]);	
				}	
			}
			else
			{
				for(int i=0;i<task_num;i++)
				{
					fprintf(file_write,"%d\n",gbest_back_St[i]);	
				}	
			}
		}
		else if(for_num!=0)
		{
			get_for_St(x_for[1],gbest_for_St);
			fprintf(file_write,"ontime_proportion:%f\n",compute_ontime_proportion(gbest_for_St));
			fprintf(file_write,"max_delay:%d\n",compute_max_delay(gbest_for_St));
			fprintf(file_write,"avg_delay:%f\n",compute_avg_delay(gbest_for_St));
			fprintf(file_write,"avg_flowtime:%f\n",compute_avg_flowtime(gbest_for_St));
			fprintf(file_write,"object function value:%f\n",compute_score(gbest_for_St));
			for(int i=0;i<task_num;i++)
			{
				fprintf(file_write,"%d\n",gbest_for_St[i]);		
			}
		}
		else if(back_num!=0)
		{
			get_back_St(gbest_back,gbest_back_St);
			for(int i=0;i<task_num;i++)
			{
				fprintf(file_write,"%d\n",gbest_back_St[i]);	
			}
		}
		else{}

		fclose(file_write);
	}

	//free the memory
	void clear()
	{
		delete[] d;
		delete[] R;
		for(int i=0;i<task_num;i++)
		{
			delete[] P[i];
			delete[] S[i];
			delete[] r[i];
		}
		delete[] P;
		delete[] S;
		delete[] r;
		delete[] P_num;
		delete[] S_num;
		delete[] LF;
		delete[] gbest_for;
		delete[] gbest_back;
		for(int i=0;i<for_num;i++)
		{
			delete[] lbest_for[i];
			delete[] x_for[i];
			delete[] v_for[i];
		}
		delete[] lbest_for;
		delete[] x_for;
		delete[] v_for;
		delete[] lbest_for_value;
		for(int i=0;i<back_num;i++)
		{
			delete[] lbest_back[i];
			delete[] x_back[i];
			delete[] v_back[i];
		}
		delete[] lbest_back;
		delete[] x_back;
		delete[] v_back;
		delete[] lbest_back_value;
	}


	int gen_lower_bound()
	{
		int *ES=new int[task_num];
		for(int i=0;i<task_num;i++) ES[i]=0;
		for(int i=0;i<task_num;i++)
		{
			for(int j=0;j<P_num[i];j++)
			{
				if(ES[i]<ES[P[i][j]]+d[P[i][j]]) ES[i]=ES[P[i][j]]+d[P[i][j]];
			}
		}
		return ES[task_num-1];
	}

	//the entrance of process a single project
	void process(string filePath,string outPath)
	{
		int last_time=clock();
		read(filePath);
		dataPreProcess();
		compute_LF();
		initialize();
		//evolution();
		finalize(outPath,last_time);
		clear();
	}

	void heuristic(int start_i,int start_j){
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
					outPath+="\\dataInput_"+s1+"_"+s2+"_RANDresult.txt";
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

