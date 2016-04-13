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