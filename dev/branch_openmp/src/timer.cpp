#include "timer.hpp"
#include "mpi.hpp"
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "tracer.hpp"
using namespace ep_lib;

namespace xios
{
  //std::map<std::string,CTimer> CTimer::allTimer;
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
    if(allTimer_ptr == NULL) allTimer_ptr = new std::map<std::string,CTimer>;

    //std::map<std::string,CTimer>::iterator it = allTimer.find(name);
    std::map<std::string,CTimer>::iterator it = allTimer_ptr->find(name);
    //if (it == allTimer.end())
    if (it == allTimer_ptr->end())
      it = allTimer_ptr->insert(std::make_pair(name, CTimer(name))).first;

      //it = allTimer.insert(std::make_pair(name, CTimer(name))).first;
    return it->second;
  }

  string CTimer::getAllCumulatedTime(void)
  {
    std::ostringstream strOut ;
    if(allTimer_ptr == 0) allTimer_ptr = new std::map<std::string,CTimer>;
    //for(std::map<std::string,CTimer>::iterator it=allTimer.begin();it!=allTimer.end();++it)
    for(std::map<std::string,CTimer>::iterator it=allTimer_ptr->begin();it!=allTimer_ptr->end();++it)
      strOut<<"Timer : "<<it->first<<"    -->   cumulated time : "<<it->second.getCumulatedTime()<<std::endl ;
    return strOut.str() ;
  }
}
