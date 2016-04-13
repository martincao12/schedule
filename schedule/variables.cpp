using namespace std;
int task_num=0;//number of tasks
int R_num=0;//number of renewable resources;
int engine_in_process_num=0;//number of engines in process
int engine_to_start_num=0;//number of engines to start
int *d;//duration
int *R;//resource limit
int **P;//processors
int **S;//successors
int *P_num;//number of porcessors
int *S_num;//number of successors
int **r;//requirement of resources
int *LF;//latest finish time of each task

int *engine_no;//the engine number of the corresponding engine
int *status;//denote the status of the step
int *resource_occupation;//denote which resource the ongoing step is occupying
int *standby_time;//denote the earliest time the step can start
int *due_time;//denote the due time of the step
double *punish_weight;//denote the punish weight if the step is delayed
int *schedule_start_time;//denote the start time of the step in the old schedule
int *modes;//denote the number of modes can be used to execute this step
int **d_multi_mode;//duration with consideration to multi mode
int ***r_multi_mode;//resource requirement with consideration to multi mode