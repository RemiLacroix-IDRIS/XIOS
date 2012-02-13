/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
#ifndef __BUFFER_IMPL_HPP__
#define __BUFFER_IMPL_HPP__

// /////////////////////////////// Définitions ////////////////////////////// //

#define DATA_HEADER_SIZE  sizeof(CBufferDataType) + 2 * sizeof(std::size_t) + sizeof(bool)

namespace xmlioserver {
namespace comm {
   
   // ------------------------------- Accesseurs -------------------------------
   
   template <class T>
      std::size_t CBuffer::getRequestedSize(T _data)
   { 
      return (DATA_HEADER_SIZE + sizeof (T)); 
   }

   template <class T>
      std::size_t CBuffer::getRequestedSize(boost::multi_array<T, 1> & _data)
   { 
      return (DATA_HEADER_SIZE + _data.num_elements() * sizeof (T));
   }

   template <>
      std::size_t CBuffer::getRequestedSize(std::string _data);

   template <class T>
      void CBuffer::getData(T & data, std::size_t position) const
   {
      if (this->size < (position + this->getRequestedSize(data)))
         XIOS_ERROR("CBuffer::getData(data, position)",
                    << " Buffer size <  size + position !");
      CBufferData bufdata;
      this->getBufferData(bufdata, position);
      if (bufdata.type != this->getBufferDataType<T>())
         XIOS_ERROR("CBuffer::getData(data, position)", 
                    << "[ BufferDataType Read : " << bufdata.type                 << ", "
                    << ", BufferDataType T : "    << this->getBufferDataType<T>() << "] "
                    << " invalid type !");
      if (bufdata.isArray != false)
         XIOS_ERROR("CBuffer::getData(data, position)",
                   << " type should be an array !");

      this->getData(reinterpret_cast<char*>(&data), bufdata.size , bufdata.pos);
   }

   template <class T>
      void CBuffer::getDataArray(boost::multi_array<T, 1> & _data, std::size_t _position) const
   {
      CBufferData bufdata;
      this->getBufferData(bufdata, _position);
      if (bufdata.type != this->getBufferDataType<T>())
         XIOS_ERROR("CBuffer::getDataArray(data, position)", << " invalid type !");
      if (bufdata.isArray != true)
         XIOS_ERROR("CBuffer::getDataArray(data, position)",
               << " type should not be an array !");
      if (this->size < (_position + (DATA_HEADER_SIZE + bufdata.size)))
         XIOS_ERROR("CBuffer::getData<T>(data, size, position)",
                << " Buffer size <  size + position !");
      _data.resize(boost::extents[bufdata.size/sizeof(T)]);
      this->getData(reinterpret_cast<char*>(_data.data()), bufdata.size , bufdata.pos);
   }

    // ------------------------------- Mutateurs --------------------------------

   template <class T>
      CBuffer::CBufferData CBuffer::setData(const T & data, std::size_t position)
   {
      if (this->size < (position + this->getRequestedSize(data)))
         XIOS_ERROR("CBuffer::setData<T>(data, size, position)",
                     << " Buffer size <  size + position !");
      CBufferData bufdata;
      bufdata.type     = this->getBufferDataType<T>();
      bufdata.isArray  = false;
      bufdata.size     = sizeof(T);
      bufdata.pos      = position + DATA_HEADER_SIZE;
      this->setBufferData(bufdata, position);
      this->setData(reinterpret_cast<const char*>(&data), bufdata.size , bufdata.pos);
      return (bufdata);
   }

   template <class T>
      CBuffer::CBufferData CBuffer::setDataArray
         (const boost::multi_array<T, 1> & _data, std::size_t _position)
   {
      if (this->size < (_position + this->getRequestedSize(_data)))
         XIOS_ERROR("CBuffer::setDataArray<T>(data, size, position)",
                   << " Buffer size <  size + position !");
      CBufferData bufdata;
      bufdata.type     = this->getBufferDataType<T>();
      bufdata.isArray  = true;
      bufdata.size     = _data.num_elements() * sizeof (T);
      bufdata.pos      = _position + DATA_HEADER_SIZE;

      this->setBufferData(bufdata, _position);
      this->setData(reinterpret_cast<const char*>(_data.data()), bufdata.size , bufdata.pos);
      return (bufdata);
   }
   
} // namespace comm
} // namespace xmlioserver

#endif //__BUFFER_IMPL_HPP__