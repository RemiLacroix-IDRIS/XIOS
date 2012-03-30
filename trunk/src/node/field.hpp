#ifndef __XMLIO_CField__
#define __XMLIO_CField__

/// xios headers ///
#include "xmlioserver_spl.hpp"
#include "group_factory.hpp"
#include "functor.hpp"
#include "functor_type.hpp"
#include "duration.hpp"
#include "date.hpp"
#include "declare_group.hpp"
#include "calendar_util.hpp"
//#include "context.hpp"


namespace xios {
namespace tree {
   
   /// ////////////////////// Déclarations ////////////////////// ///

   class CFieldGroup;
   class CFieldAttributes;
   class CField;

   class CFile;
   class CGrid;
   class CContext ;
   ///--------------------------------------------------------------

   // Declare/Define CFieldAttribute
   BEGIN_DECLARE_ATTRIBUTE_MAP(CField)
#  include "field_attribute.conf"
   END_DECLARE_ATTRIBUTE_MAP(CField)

   ///--------------------------------------------------------------
   class CField
      : public CObjectTemplate<CField>
      , public CFieldAttributes
   {
         /// friend ///
         friend class CFile;

         /// typedef ///
         typedef CObjectTemplate<CField>   SuperClass;
         typedef CFieldAttributes SuperClassAttribute;

      public :

         typedef CFieldAttributes RelAttributes;
         typedef CFieldGroup      RelGroup;

         enum EEventId
         {
           EVENT_ID_UPDATE_DATA
         } ;
         
         /// Constructeurs ///
         CField(void);
         explicit CField(const StdString & id);
         CField(const CField & field);       // Not implemented yet.
         CField(const CField * const field); // Not implemented yet.

         /// Accesseurs ///
               boost::shared_ptr<CField> getDirectFieldReference(void) const;
         const boost::shared_ptr<CField> getBaseFieldReference(void)   const;
         const std::vector<boost::shared_ptr<CField> > & getAllReference(void) const;

         boost::shared_ptr<CGrid> getRelGrid(void) const ;
         boost::shared_ptr<CFile> getRelFile(void) const ;

      public :

         StdSize getNStep(void) const;

         const date::CDuration & getFreqOperation(void) const;
         const date::CDuration & getFreqWrite(void) const;

         boost::shared_ptr<date::CDate> getLastWriteDate(void) const;
         boost::shared_ptr<date::CDate> getLastOperationDate(void) const;

         boost::shared_ptr<func::CFunctor> getFieldOperation(void) const;
         
         ARRAY(double, 1) getData(void) const;

         const StdString & getBaseFieldId(void) const;

         /// Mutateur ///
         void setRelFile(const boost::shared_ptr<CFile> _file);
         void incrementNStep(void);
         void resetNStep() ;

         template <StdSize N> bool updateData(const ARRAY(double, N)   data);
         
         bool updateDataServer
               (const date::CDate & currDate,
                const std::deque<ARRAY(double, 1)> storedClient);
 
       public :

         /// Test ///
         bool hasDirectFieldReference(void) const;
         bool isActive(void) const;

         /// Traitements ///
         void solveRefInheritance(void);
         void solveGridReference(void);
         void solveOperation(void);

         virtual void fromBinary(StdIStream & is);

         /// Destructeur ///
         virtual ~CField(void);

         /// Accesseurs statiques ///
         static StdString GetName(void);
         static StdString GetDefName(void);
         
         static ENodeType GetType(void);
         
        template <StdSize N> void setData(const ARRAY(double, N) _data) ;
        static bool dispatchEvent(CEventServer& event) ;
        void sendUpdateData(void) ;
        static void recvUpdateData(CEventServer& event) ;
        void recvUpdateData(vector<int>& ranks, vector<CBufferIn*>& buffers) ;
        void writeField(void) ;
        void outputField(ARRAY(double,3) fieldOut) ;
        void outputField(ARRAY(double,2) fieldOut) ;
        
      public :

         /// Propriétés privées ///
         
         std::vector<boost::shared_ptr<CField> > refObject;
         boost::shared_ptr<CField> baseRefObject;
         boost::shared_ptr<CGrid>  grid ;
         boost::shared_ptr<CFile>  file;

         date::CDuration freq_operation, freq_write;
         date::CDuration freq_operation_srv, freq_write_srv;

         StdSize nstep;
         boost::shared_ptr<date::CDate>    last_Write, last_operation;
         boost::shared_ptr<date::CDate>    lastlast_Write_srv,last_Write_srv, last_operation_srv;
         
         boost::shared_ptr<func::CFunctor> foperation;
         map<int,boost::shared_ptr<func::CFunctor> > foperation_srv;
         
         ARRAY(double, 1) data;
         map<int,ARRAY(double,1)> data_srv ;

   }; // class CField

   ///--------------------------------------------------------------

   // Declare/Define CFieldGroup and CFieldDefinition
   DECLARE_GROUP(CField);

   } // namespace tree

   ///-----------------------------------------------------------------

   template <>
      void CGroupTemplate<CField, CFieldGroup, CFieldAttributes>::solveRefInheritance(void);

   ///-----------------------------------------------------------------
} // namespace xios


#endif // __XMLIO_CField__
