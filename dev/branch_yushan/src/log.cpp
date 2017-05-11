#include "log.hpp"
#include <string>
#include <iostream>
#include <string>

namespace xios
{

  std::filebuf* info_FB[10];


  CLog info("info") ;
  CLog report("report") ;
  CLog error("error", cerr.rdbuf()) ;

  
  CLog& CLog::operator()(int l)
    {
      if (l<=level)
      {
        omp_set_lock( &mutex );
        //rdbuf(strBuf_); 
        rdbuf(strBuf_array[omp_get_thread_num()]); 
        *this<<"-> "<<name<<" : " ;
        omp_unset_lock( &mutex );
      }
      else rdbuf(NULL) ;
      return *this;
    }



  int test_omp_rank;
  #pragma omp threadprivate(test_omp_rank)

  

}
