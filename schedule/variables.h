using namespace std;
extern int task_num;//number of tasks
extern int R_num;//number of renewable resources;
extern int engine_in_process_num;//number of engines in process
extern int engine_to_start_num;//number of engines to start
extern int *d;//duration
extern int *R;//resource limit
extern int **P;//processors
extern int **S;//successors
extern int *P_num;//number of porcessors
extern int *S_num;//number of successors
extern int **r;//requirement of resources
extern int *LF;//latest finish time of each task

extern int *engine_no;//the engine number of the corresponding engine
extern int *status;//denote the status of the step
extern int *resource_occupation;//denote which resource the ongoing step is occupying
extern int *standby_time;//denote the earliest time the step can start
extern int *due_time;//denote the due time of the step
extern double *punish_weight;//denote the punish weight if the step is delayed
extern int *schedule_start_time;//denote the start time of the step in the old schedule
extern int *modes;//denote the number of modes can be used to execute this step