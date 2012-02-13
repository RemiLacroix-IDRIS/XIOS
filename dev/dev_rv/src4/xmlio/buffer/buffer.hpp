/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__ 

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <vector>

// boost Headers
#include <boost/multi_array.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   namespace comm
   {
      class CBuffer
      {
         public : // Définition de types

            typedef enum buffer_data_type
            { TBOOL8 = 0, TCHAR8, TINT32, TFLOAT32, TDOUBLE64
            } CBufferDataType;
            
            typedef struct  buffer_data
            { CBufferDataType type; bool isArray; std::size_t size; std::size_t pos;
            } CBufferData;
            
         protected : // Construteurs            
            
            explicit CBuffer(std::size_t _size);
            CBuffer(char * _data, std::size_t _size);            
            CBuffer(const CBuffer & _buffer_ref);         // NIY
            CBuffer(const CBuffer * const _buffer_ptr);   // NIY
            
         protected : // Destructeur
            
            virtual ~CBuffer(void);
            
         public : // Operateurs d'accès
         
            operator char * (void);
            char * operator[](std::size_t _position);
            
         public : // Accesseurs statiques
            
            template <class T>
               static inline std::size_t getRequestedSize(T _data);
            
            template <class T>
               static inline std::size_t getRequestedSize(boost::multi_array<T, 1> & _data);
            
            template <class T> static CBufferDataType getBufferDataType(void);

            std::size_t getNextDataPosition(std::size_t _position);
            
         public : // Accesseurs
             
            std::size_t getSize(std::size_t _position = 0) const;
            char * getData(std::size_t _position = 0) const;
            
         public : // Accesseurs de données

            template <class T>
               inline void getData(T & data, std::size_t position) const;
            
            template <class T>
               inline void getDataArray
                  (boost::multi_array<T, 1> & _data, std::size_t _position) const;
            
         //--------------------------------------------------------------

            char     getChar  (std::size_t position) const;
            bool     getBool  (std::size_t position) const;
            float    getFloat (std::size_t position) const;
            double   getDouble(std::size_t position) const;
            long int getInt   (std::size_t position) const;

            std::string getString(std::size_t position) const;

            boost::shared_ptr<boost::multi_array<char,     1> > 
               getCharArray  (std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<bool,     1> > 
               getBoolArray  (std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<float,    1> >
               getFloatArray (std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<double,   1> >
               getDoubleArray(std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<long int, 1> > 
               getIntArray   (std::size_t _position) const;

         //--------------------------------------------------------------         

         public : // Mutateurs de données
            
            template <class T>
               inline CBufferData setData(const T & _data, std::size_t _position);
            
            template <class T>
               inline CBufferData setDataArray
                  (const boost::multi_array<T, 1> & _data, std::size_t _position);
            
         //--------------------------------------------------------------

            void setChar  (char     value, std::size_t position) ;
            void setBool  (bool     value, std::size_t position) ;
            void setFloat (float    value, std::size_t position) ;
            void setDouble(double   value, std::size_t position) ;
            void setInt   (long int value, std::size_t position) ;

            void setString(const std::string & value, std::size_t position) ;

            void setCharArray  
               (boost::shared_ptr<boost::multi_array<char,     1> > _value, std::size_t _position) ;
            void setBoolArray  
               (boost::shared_ptr<boost::multi_array<bool,     1> > _value, std::size_t _position) ;
            void setFloatArray 
               (boost::shared_ptr<boost::multi_array<float,    1> > _value, std::size_t _position) ;
            void setDoubleArray
               (boost::shared_ptr<boost::multi_array<double,   1> > _value, std::size_t _position) ;
            void setIntArray   
               (boost::shared_ptr<boost::multi_array<long int, 1> > _value, std::size_t _position) ;

         //-------------------------------------------------------------- 
            
            virtual void clear(void) = 0;
            void updateBufferData(std::size_t _position);
            
         public : // Sortie fichier binaire
             
            void printToBinaryFile (const std::string & _filename);
            
            void printToBinaryStream (std::ostream & _ostr);

         public : // Sortie fichier ascii
             
            virtual void printToTextFile (const std::string & _filename) = 0;
            
            virtual void printToTextStream (std::ostream & _ostr) = 0;
            
         protected : // Mutateurs de données privés
            
            void setData(const char * _data, std::size_t _size, std::size_t _position);
            
            void fillData(char _data, std::size_t _size, std::size_t _position);
            
            void setBufferData(const CBufferData & _bufdata, std::size_t _position);
            
         protected : // Accesseurs de données privés
            
            void getData(char * _data, std::size_t _size, std::size_t _position) const;
            
            void getBufferData(CBufferData & _bufdata, std::size_t  _position) const;
            
         private: // Propriétés privées
            
            std::size_t size;
            bool delIdata; 
            char * idata;
            
      }; // class CBuffer
   } // namespace comm
} // namespace xmlioserver
#endif //__BUFFER_HPP__
