#include "HGA.h"
#include "JPSO.h"
#include "HSS.h"
#include <thread>
using namespace std;


void main(){
	using namespace JPSO_name;
	JPSO();
	using namespace HGA_name;
	HGA();
	using namespace HSS_name;
	HSS();
	int x;
	cin>>x;
}