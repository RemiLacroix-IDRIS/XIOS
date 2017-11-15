#include "timerRemap.hpp"
#include "mpi.hpp"
#include <string>
#include <map>
#include <iostream>
using namespace ep_lib;

namespace sphereRemap {

using namespace std;

//map<string,CTimer*> CTimer::allTimer;
map<string,CTimer*> *CTimer::allTimer_ptr = 0;

CTimer::CTimer(const string& name_) : name(name_)
{
	reset();
}

double CTimer::getTime(void)
{
	return MPI_Wtime();
}

void CTimer::suspend(void)
{
	if (!suspended)
		cumulatedTime+=getTime()-lastTime;
	suspended=true;
}

void CTimer::resume(void)
{
	if (suspended)
		lastTime=getTime();
	suspended=false;
}

void CTimer::reset(void)
{
	cumulatedTime=0.;
	suspended=true;
}

double CTimer::getCumulatedTime(void)
{
	return cumulatedTime;
}

void CTimer::print(void)
{
#ifdef TIMEIT
	cout << " Time for "<<name<<" : "<<getCumulatedTime() <<"  s"<<endl;
#endif
}

CTimer& CTimer::get(const string name)
{
	map<string,CTimer*>::iterator it;
	if(allTimer_ptr == 0) allTimer_ptr = new map<string,CTimer*>;
	//it=allTimer.find(name);
	it=allTimer_ptr->find(name);
	//if (it==allTimer.end()) it=allTimer.insert(pair<string,CTimer*>(name,new CTimer(name))).first;
	if (it==allTimer_ptr->end()) it=allTimer_ptr->insert(pair<string,CTimer*>(name,new CTimer(name))).first;
	return *(it->second);
}
}
