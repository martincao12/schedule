#include "HGA.h"
#include "JPSO.h"
#include "HSS.h"
#include "heuristic.h"
#include <thread>
using namespace std;


void main(){
	int start_i,start_j;
	cin>>start_i>>start_j;
	using namespace heuristic_name;
	heuristic(start_i,start_j);
	/*using namespace JPSO_name;
	JPSO(start_i,start_j);*/
	/*using namespace HGA_name;
	HGA(start_i,start_j);*/
	/*using namespace HSS_name;
	HSS(start_i,start_j);*/
}