#ifndef __XIOS_LOG_HPP__
#define __XIOS_LOG_HPP__

#include <string>
#include <iostream>
#include <string>
#include <stdio.h>
#include <omp.h>

namespace xios
{
  using namespace std ;

  class CLog : public ostream
  {
    public :
    CLog(const string& name_, std::streambuf* sBuff = cout.rdbuf())
      : ostream(cout.rdbuf()), level(0), name(name_), strBuf_(sBuff) 
    {
      omp_init_lock( &mutex );
      for(int i=0; i<16; i++)
        strBuf_array[i] = sBuff;
    }

    ~CLog()
    {
      omp_destroy_lock( &mutex );
    }


    CLog& operator()(int l);
    void setLevel(int l) {level=l; }
    int  getLevel() {return level ;}
    bool isActive(void) { if (rdbuf()==NULL) return true ; else return false ;}
    bool isActive(int l) {if (l<=level) return true ; else return false ; }

  public:
    //! Write log into a file with its streambuf
    void write2File(std::streambuf* sBuff) { changeStreamBuff(sBuff); }

    //! Write log into standard output
    void write2StdOut() { changeStreamBuff(cout.rdbuf()); }

    //! Write log into standard error output
    void write2StdErr() { changeStreamBuff(cerr.rdbuf()); }

  private:
    /*!
     * \brief Change current streambuf (by default std::cout) to new one
     * This function associates a new streambuf to the current log object
     * \param [in] pointer to new streambuf
    */
    void changeStreamBuff(std::streambuf* sBuff) 
    { 
      strBuf_ = sBuff; 
      strBuf_array[omp_get_thread_num()] = sBuff;
      rdbuf(sBuff);
    }

    int level ;
    string name ;
    std::streambuf* strBuf_;
    std::streambuf* strBuf_array[16];
    omp_lock_t mutex;
  };

  extern CLog info;
  extern CLog report;
  extern CLog error;


  extern std::filebuf* info_FB[16];


}
#endif
