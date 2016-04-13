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