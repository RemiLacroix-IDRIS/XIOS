/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __NODE_ENUM_HPP__
#define __NODE_ENUM_HPP__

#define DECLARE_NODE(Name_, name_)     ,e##Name_, g##Name_
#define DECLARE_NODE_PAR(Name_, name_) ,e##Name_, g##Name_

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver
{
   namespace tree
   {
      typedef enum _node_type
      {
         Unknown = 0

#include "node_type.conf"

      } ENodeType;

   } // namespace tree
} // namespace xmlioserver

#endif // __NODE_ENUM_HPP__
