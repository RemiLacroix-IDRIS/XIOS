/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ARRAY_UTIL_HPP__
#define __ARRAY_UTIL_HPP__

/**
 * \file    array_util.hpp
 * \brief   Définition de quelques méthodes pratiques pour travailler avec les tableaux (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    12 Juillet 2011
 */
 
 #ifndef __XIOS_NO_EXTERN
 
// boost Headers
#include <boost/multi_array.hpp>

#endif // __XIOS_NO_EXTERN

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver
{
   /**
    * \class CArrayUtil
    * \brief Classe regroupant quelques méthodes pratiques pour travailler avec les tableaux
    * multi-dimensionnels de la bibliothèque Boost (ie boost::multi_array).
    */
   class CArrayUtil
   {
      public : // Conversion de tableau typé
      
         /**
          * \brief Convertit un tableau de données d'un certain type vers un autre.
          * \tparam ODataType : type du tableau d'entrée.
          * \tparam IDataType : type du tableau de sortie.
          * \tparam Size      : dimension des tableaux.
          * \param _array_in  : un tableau d'entrée de type ODataType.
          * \param _array_out : un tableau de sortie de type IDataType.
          * \remarks Les tableaux d'entrée et de sortie sont tous deux de dimension Size.
          * 
          * <i> Exemple de conversion d'un tableau de rééls en double précision 
          * en tableau de rééls en simple précision: </i>
          * \code
          * // Conversion de tableau
          * const boost::multi_array<double, 3> input(boost::extents[10][12][3]);
          * boost::multi_array<float, 3> output;
          * xios::CArrayUtil::SameShape(input, output);
          * xios::CArrayUtil::Convert(input, output);
          * \endcode
          */
         template <typename ODataType, typename IDataType, std::size_t Size>
            static inline void Convert
               (const boost::multi_array<IDataType, Size> & _array_in,
                      boost::multi_array<ODataType, Size> & _array_out);
                      
      public : // Opérations sur les tableaux 
         
         /**
          * \brief Applique une opération binaire à des tableaux (tableau et tableau).
          * \tparam DataType : type de données des tableaux.
          * \tparam Size : dimension des tableaux.
          * \param _input_array1  : premier tableau multidimensionnel d'entrée.
          * \param _input_array2  : second tableau multidimensionnel d'entrée.
          * \param _output_array  : tableau multidimensionnel de sortie.
          * \param _function      : opération binaire à appliquer.
          * 
          * <i> Exemple de somme des éléments de deux tableaux</i>
          * \code
          * boost::multi_array<double, 2> 
          *       input1(boost::extents[10][12]),
          *       input2(boost::extents[10][12]),
          *       output(boost::extents[10][12]);
          * xios::CArrayUtil::Apply1(input1, input2, output, &xios::AUSum);
          * \endcode
          */
         template <typename DataType, std::size_t Size>
            static inline void Apply1(const boost::multi_array<DataType, Size> & _input_array1,
                                      const boost::multi_array<DataType, Size> & _input_array2,
                                            boost::multi_array<DataType, Size> & _output_array,
                                      DataType (*_function) (DataType, DataType));
         
         /**
          * \brief Applique une opération binaire à un tableau (tableau et scalaire).
          * \tparam DataType      : type de données du tableau.
          * \tparam ScalDataType  : type de données du scalaire.
          * \tparam Size          : dimension des tableaux.
          * \param _input_array   : tableau multidimensionnel d'entrée.
          * \param _output_array  : tableau multidimensionnel de sortie.
          * \param _scalar        : scalaire de type ScalDataType
          * \param _function      : opération binaire à appliquer.
          * 
          * <i> Exemple de somme des éléments de tableaux avec les scalaire 3.</i>
          * \code
          * boost::multi_array<double, 2> 
          *       input(boost::extents[10][12]),
          *       output(boost::extents[10][12]);
          * xios::CArrayUtil::Apply2(input, output, 3., &xios::AUSum<double, double>);
          * \endcode
          */                                                                                                          
         template <typename DataType, typename ScalDataType, std::size_t Size>
            static inline void Apply2(const boost::multi_array<DataType, Size> & _input_array,
                                            boost::multi_array<DataType, Size> & _output_array,
                                            ScalDataType _scalar,
                                      DataType (*_function) (DataType, ScalDataType));
                                                                                      
      public : // Autres méthodes 

         /**
          * \brief Associe au tableau de sortie le même profil que le tableau d'entrée.
          * \tparam ODataType : type du tableau d'entrée.
          * \tparam IDataType : type du tableau de sortie.
          * \tparam Size      : dimension des tableaux.
          * \param _array_in  : un tableau d'entrée de type ODataType.
          * \param _array_out : un tableau de sortie de type IDataType.
          * \remarks Les tableaux d'entrée et de sortie sont tous deux de dimension Size.
          */
         template <typename ODataType, typename IDataType, std::size_t Size>
            static inline void SameShape
               (const boost::multi_array<IDataType, Size> & _array_in,
                      boost::multi_array<ODataType, Size> & _array_out);              

   }; // CArrayUtil
 
// ////////////////////////// Définitions d'opérations ////////////////////// //
   
   /// \brief Somme de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      inline DataType AUSum (DataType _value_1, ScalDataType _value_2);
   
   /// \brief Division de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      inline DataType AUDiv (DataType _value_1, ScalDataType _value_2);
   
   /// \brief Maximum de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      inline DataType AUMax (DataType _value_1, ScalDataType _value_2);
   
    /// \brief Minimum de _value_1 et _value_2
   template <typename DataType, typename ScalDataType>
      inline DataType AUMin (DataType _value_1, ScalDataType _value_2);

} // namespace xmlioserver

#endif // __ARRAY_UTIL_HPP__
