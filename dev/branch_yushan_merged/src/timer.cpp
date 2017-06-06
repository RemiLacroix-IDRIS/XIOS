#include "timer.hpp"
#include "mpi.hpp"
#include <string>
#include <map>
#include "tracer.hpp"
#include <iostream>
#include <sstream>

namespace xios
{
  std::map<std::string,CTimer> CTimer::allTimer;
  std::map<std::string,CTimer> *CTimer::allTimer_ptr = 0;
  
  CTimer::CTimer(const std::string& name_) : name(name_) 
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
    {
      traceEnd(name);
      cumulatedTime += getTime() - lastTime;
    }
    suspended = true;
  }
  
  void CTimer::resume(void)
  {
    if (suspended) 
    {
      lastTime = getTime();
      traceBegin(name);
    }
    suspended = false;
  }
  
  void CTimer::reset(void)
  {
    cumulatedTime = 0.;
    suspended = true;
  }
  
  double CTimer::getCumulatedTime(void)
  {
    return cumulatedTime;
  }
  
  CTimer& CTimer::get(const std::string name)
  {
    // bkp
    // std::map<std::string,CTimer>::iterator it = allTimer.find(name);
    // if (it == allTimer.end())
    //   it = allTimer.insert(std::make_pair(name, CTimer(name))).first;
    // return it->second;

    if(allTimer_ptr == 0) allTimer_ptr = new std::map<std::string,CTimer>;

    std::map<std::string,CTimer>::iterator it = (*allTimer_ptr).find(name);
    if (it == (*allTimer_ptr).end())
      it = (*allTimer_ptr).insert(std::make_pair(name, CTimer(name))).first;
    return it->second;
  }

}
