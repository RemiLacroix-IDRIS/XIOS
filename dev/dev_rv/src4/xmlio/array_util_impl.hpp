/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ARRAY_UTIL_IMPL_HPP__
#define __ARRAY_UTIL_IMPL_HPP__

/**
 * \file    array_util_impl.hpp
 * \brief   Définition de quelques méthodes pratiques pour travailler avec les tableaux (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    12 Juillet 2011
 */

 #ifndef __XIOS_NO_EXTERN
 
// standard Headers
#include <algorithm>

#endif // __XIOS_NO_EXTERN

// ////////////////////////////// Déclarations ////////////////////////////// //

namespace xmlioserver
{
   // ---------------------- Conversion de tableau typé ------------------------
   
   // Convertit un tableau de données d'un certain type vers un autre.
   template <typename ODataType, typename IDataType, std::size_t Size>
      void CArrayUtil::Convert(const boost::multi_array<IDataType, Size> & _array_in,
                                     boost::multi_array<ODataType, Size> & _array_out)
   {
      _array_out.assign(_array_in.data(), _array_in.data()+_array_in.num_elements());
   }
   
   // --------------------- Opérations sur les tableaux  -----------------------
   
   // Applique une opération binaire à des tableaux (tableau et tableau).             
   template <typename DataType, std::size_t Size>
      void CArrayUtil::Apply1(const boost::multi_array<DataType, Size> & _input_array1,
                              const boost::multi_array<DataType, Size> & _input_array2,
                                    boost::multi_array<DataType, Size> & _output_array,
                              DataType (*_function) (DataType, DataType))
   {     
      const DataType * it1 = _input_array1.data(), * it2 = _input_array2.data(), 
                     * end = _input_array1.data() + _input_array1.num_elements();
            DataType * it = _output_array.data();
      
      for (; it1 != end; it1++, it2++, it++) *it = (*_function)(*it1, *it2);
   }
   
   // Applique une opération binaire à un tableau (tableau et scalaire).
   template <typename DataType, typename ScalDataType, std::size_t Size>
      void CArrayUtil::Apply2(const boost::multi_array<DataType, Size> & _input_array,
                                    boost::multi_array<DataType, Size> & _output_array,
                                    ScalDataType _scalar,
                              DataType (*_function) (DataType, ScalDataType))
   {
      const DataType * it1 = _input_array.data(),
                     * end = _input_array.data() + _input_array.num_elements();
            DataType * it = _output_array.data();
      for (; it1 != end; it1++, it++) *it = (*_function)(*it1, _scalar);
   }
   
   // --------------------------- Autres méthodes ------------------------------
   
   // Associe au tableau de sortie le même profil que le tableau d'entrée.
   template <typename ODataType, typename IDataType, std::size_t Size>
      void CArrayUtil::SameShape(const boost::multi_array<IDataType, Size> & _array_in,
                                       boost::multi_array<ODataType, Size> & _array_out)
   {
      const std::size_t * _shape = _array_in.shape();
      std::vector<std::size_t> shape(_shape, _shape+Size);
      _array_out.resize(shape);
   }
   
   // ----------------------- Opération élémentaires ---------------------------

   // Somme de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      DataType AUSum (DataType _value_1, ScalDataType _value_2)
   { return (_value_1+_value_2); }
   
   // Division de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      DataType AUDiv (DataType _value_1, ScalDataType _value_2)
   { return (_value_1/_value_2); }
   
   // Maximum de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      DataType AUMax (DataType _value_1, ScalDataType _value_2)
   { return (std::max(_value_1, _value_2)); }
   
    // Minimum de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      DataType AUMin (DataType _value_1, ScalDataType _value_2)
   { return (std::min(_value_1, _value_2)); }
                                     
} // namespace xmlioserver

#endif // __ARRAY_UTIL_IMPL_HPP__
