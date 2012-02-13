/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <fstream>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "buffer.hpp" 
#include "buffer_impl.hpp" 

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace comm {

   // ------------------------------ Constructeurs -----------------------------

   CBuffer::CBuffer(std::size_t _size)
      : size(_size), delIdata(true)
   {
      this->idata = new char[_size]();
      //CMPIManager::AllocMem(&idata, size);
   }

   CBuffer::CBuffer(char * _data, std::size_t _size)
      : size(_size), delIdata(false)
   {
      this->idata = _data;
   }
      
   CBuffer::CBuffer(const CBuffer & _buffer_ref)
      : size(_buffer_ref.size), delIdata(true)
   {
     this->idata = new char[size]();
     std::copy (_buffer_ref.idata, _buffer_ref.idata+size, this->idata);
   }
      
   CBuffer::CBuffer(const CBuffer * const _buffer_ptr)
      : size(_buffer_ptr->size), delIdata(true)
   {
     this->idata = new char[size]();
      std::copy (_buffer_ptr->idata, _buffer_ptr->idata+size, this->idata);       
   }

   // ------------------------------- Destructeur ------------------------------
   
   CBuffer::~CBuffer(void)
   {
      if (delIdata) delete [] this->idata;
      else this->fillData('\0', this->getSize(), 0);
      //CMPIManager::FreeMem(idata);
   }
   
   // ------------------------------- Opérateurs -------------------------------
   
   CBuffer::operator char * (void)
   {
      return (this->getData()); 
   }

   char * CBuffer::operator[](std::size_t _position)
   {
      return (this->getData(_position)); 
   }
   
   // ------------------------------- Mutateurs --------------------------------
   
   void CBuffer::setData(const char * _data, std::size_t _size, std::size_t _position)
   {
      if (this->size < (_size + _position))
         XIOS_ERROR("CBuffer::getData(data, size, position)",
                    << " Buffer size <  size + position !");
      std::copy(&(_data[0]), &(_data[size]), this->getData(_position));
   }
  
   void CBuffer::setBufferData(const CBufferData & _bufdata, std::size_t _position)
   {
      std::size_t currposition = _position;
      this->setData( reinterpret_cast<const char*>(&(_bufdata.type))
                   , sizeof(CBufferDataType), currposition);
      this->setData( reinterpret_cast<const char*>(&(_bufdata.isArray))
                   , sizeof(bool), currposition += sizeof(CBufferDataType));
      this->setData( reinterpret_cast<const char*>(&(_bufdata.size))
                   , sizeof(std::size_t), currposition += sizeof(bool));
     this->setData( reinterpret_cast<const char*>(&(_bufdata.pos))
                   , sizeof(std::size_t), currposition += sizeof(std::size_t));
   }
  
   void CBuffer::updateBufferData(std::size_t _position)
   {
      CBufferData bufdata;
      this->getBufferData(bufdata, _position);
      bufdata.pos = _position + DATA_HEADER_SIZE;
      this->setBufferData(bufdata, _position);
   }
  
   void CBuffer::fillData(char _data, std::size_t _size, std::size_t _position)
   {
      if (this->size < (_size + _position))
         XIOS_ERROR("CBuffer::getData(data, size, position)", << " Buffer size <  size + position !");
      std::fill_n(this->getData(_position), _size, _data);
   }
  
   // ------------------------------- Accesseurs -------------------------------
   
   std::size_t CBuffer::getSize(std::size_t _position) const
   {
      if (_position > this->size)
         XIOS_ERROR("CBuffer::getSize(position)", << " Buffer size <  size + position !");
      return (this->size - _position);
   }

   char * CBuffer::getData(std::size_t _position) const
   {
      if (_position > this->size)
         XIOS_ERROR("CBuffer::getData(position)", << " Buffer size < position !");
      return &(this->idata[_position]);
   }
   
   void CBuffer::getData(char * _data, std::size_t _size, std::size_t _position) const
   {
      if (this->size < (_size + _position))
        XIOS_ERROR("CBuffer::getData(data, size, position)", << " Buffer size <  size + position !");
       std::copy(this->getData(_position), this->getData(_position + _size), _data);
   }
   
   void CBuffer::getBufferData(CBufferData & _bufdata, std::size_t _position) const
   {
      std::size_t currposition = _position;
      this->getData( reinterpret_cast<char*>(&(_bufdata.type))
                    , sizeof(CBufferDataType), currposition);
      this->getData( reinterpret_cast<char*>(&(_bufdata.isArray))
                   , sizeof(bool), currposition += sizeof(CBufferDataType));
      this->getData( reinterpret_cast<char*>(&(_bufdata.size))
                   , sizeof(std::size_t), currposition += sizeof(bool));
      this->getData( reinterpret_cast<char*>(&(_bufdata.pos))
                   , sizeof(std::size_t), currposition += sizeof(std::size_t));
   }
  
   std::size_t CBuffer::getNextDataPosition(std::size_t _position)
   {
      CBufferData  bufdata;
      this->updateBufferData(_position);
      this->getBufferData(bufdata, _position);
      return (bufdata.size + bufdata.pos);
   } 

   template <>
      std::size_t CBuffer::getRequestedSize(std::string data)
   { 
      return (DATA_HEADER_SIZE + data.size() * sizeof (char)); 
   }
   
   // --------------------------------------------------------------------------
   
#define BufferDataTypeGetter(type, type_enum)\
   template <> CBuffer::CBufferDataType \
      CBuffer::getBufferDataType<type>(void) { return (type_enum); }; \
   template <> CBuffer::CBufferDataType \
      CBuffer::getBufferDataType<boost::multi_array<type, 1> >(void) { return (type_enum); };

      BufferDataTypeGetter(bool,     TBOOL8);
      BufferDataTypeGetter(char,     TCHAR8);
      BufferDataTypeGetter(float,    TFLOAT32);
      BufferDataTypeGetter(double,   TDOUBLE64);
      BufferDataTypeGetter(long int, TINT32);

#undef BufferDataTypeGetter
      
   // ------------------------- Ecriture binaire -------------------------------

   void CBuffer::printToBinaryFile (const std::string & _filename)
   {
      std::ofstream ofs(_filename.c_str());
      this->printToBinaryStream(ofs);
      ofs.close();
   }
     
   void CBuffer::printToBinaryStream (std::ostream & _ostr)
   {  
      _ostr.write (this->getData(), this->getSize()); 
   }
   
   // --------------------------------------------------------------------------
   
#define CBufferGetter(type, Type)       \
   type CBuffer::get##Type(std::size_t _position) const \
   { type retvalue; this->getData<type>(retvalue, _position); return (retvalue); }


      CBufferGetter(char     , Char)
      CBufferGetter(bool     , Bool)
      CBufferGetter(float    , Float)
      CBufferGetter(double   , Double)
      CBufferGetter(long int , Int)

#undef CBufferGetter

   // --------------------------------------------------------------------------

#define CBufferArrayGetter(type, Type)                                   \
   boost::shared_ptr<boost::multi_array<type, 1> >                       \
                  CBuffer::get##Type##Array(std::size_t _position) const \
   {  boost::shared_ptr<boost::multi_array<type, 1> > retvalue           \
	         (new boost::multi_array<type, 1>(boost::extents[1]));        \
      this->getDataArray<type>(*retvalue, _position);                    \
      return (retvalue); }

      CBufferArrayGetter(char     , Char)
      CBufferArrayGetter(bool     , Bool)
      CBufferArrayGetter(float    , Float)
      CBufferArrayGetter(double   , Double)
      CBufferArrayGetter(long int , Int)

#undef CBufferArrayGetter

      std::string CBuffer::getString(std::size_t position) const
      {
         boost::shared_ptr<boost::multi_array<char, 1> > array = this->getCharArray(position);
         return (std::string(array->data(), array->num_elements()));
      }

   // --------------------------------------------------------------------------

#define CBufferSetter(type, Type) \
      void  CBuffer::set##Type(type _value, std::size_t _position) \
      { this->setData(_value, _position); }

      CBufferSetter(char     , Char)
      CBufferSetter(bool     , Bool)
      CBufferSetter(float    , Float)
      CBufferSetter(double   , Double)
      CBufferSetter(long int , Int)

#undef CBufferSetter

   // --------------------------------------------------------------------------

#define CBufferArraySetter(type, Type)                                                 \
      void  CBuffer::set##Type##Array                                                  \
      (boost::shared_ptr<boost::multi_array<type, 1> >  _value, std::size_t _position) \
      { this->setDataArray(*_value, _position); }

      CBufferArraySetter(char     , Char)
      CBufferArraySetter(bool     , Bool)
      CBufferArraySetter(float    , Float)
      CBufferArraySetter(double   , Double)
      CBufferArraySetter(long int , Int)

#undef CBufferArraySetter

      void CBuffer::setString(const std::string & value, std::size_t position)
      {
         boost::shared_ptr<boost::multi_array<char, 1> > arr          
	         (new boost::multi_array<char, 1>(boost::extents[value.size()]));
         std::copy(value.data(), &(value.data()[value.size()]), arr->data());
         this->setCharArray(arr, position);
      }
      
   // --------------------------------------------------------------------------

} // namespace comm
} // namespace xmlioserver