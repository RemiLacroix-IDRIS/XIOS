#ifndef __XMLIO_CGroupFactory_impl__
#define __XMLIO_CGroupFactory_impl__

namespace xmlioserver
{
   /// ////////////////////// Définitions ////////////////////// ///

   template <typename U>
      void CGroupFactory::AddGroup(boost::shared_ptr<U> pgroup,
                                   boost::shared_ptr<U> cgroup)
   {
      if (cgroup.get() == NULL || pgroup.get() == NULL )
         ERROR("CGroupFactory::AddGroup(boost::shared_ptr<U> pgroup, boost::shared_ptr<U> cgroup)",
               << " pgroup or cgroup NULL !");
      if (!cgroup->hasId())
         pgroup->groupList.insert(pgroup->groupList.end(), cgroup);
      else
      {
         pgroup->groupList.insert(pgroup->groupList.end(), cgroup);
         pgroup->groupMap.insert(std::make_pair(cgroup->getId(), cgroup));
      }
   }

   template <typename U>
      void CGroupFactory::AddChild(boost::shared_ptr<U> group,
                                   boost::shared_ptr<typename U::RelChild> child)
   {
      if (group.get() == NULL || child.get() == NULL )
         ERROR("CGroupFactory::AddGroup(boost::shared_ptr<U> pgroup, boost::shared_ptr<U> cgroup)",
               << " pgroup or cgroup NULL !");
      if (!child->hasId())
         group->childList.insert(group->childList.end(), child);
      else
      {
         group->childList.insert(group->childList.end(), child);
         group->childMap.insert(std::make_pair(child->getId(), child));
      }
   }

   template <typename U>
      boost::shared_ptr<U>
         CGroupFactory::CreateGroup(boost::shared_ptr<U> group, const StdString & id)
   {
      CObjectFactory::SetCurrentContextId
      (CGroupFactory::GetCurrentContextId());
      if (id.size() == 0)
      {
         boost::shared_ptr<U> value = CObjectFactory::CreateObject<U>();
         group->groupList.insert(group->groupList.end(), value);
         return (value);
      }
      else if (CGroupFactory::HasGroup(group, id))
         return (CGroupFactory::GetGroup(group, id));
      else
      {
         boost::shared_ptr<U> value = CObjectFactory::CreateObject<U>(id);
         group->groupList.insert(group->groupList.end(), value);
         group->groupMap.insert(std::make_pair(id, value));
         return (value);
      }
   }

   template <typename U>
      boost::shared_ptr<typename U::RelChild>
         CGroupFactory::CreateChild(boost::shared_ptr<U> group, const StdString & id)
   {
      CObjectFactory::SetCurrentContextId
      (CGroupFactory::GetCurrentContextId());
      if (id.size() == 0)
      {
         boost::shared_ptr<typename U::RelChild> value =
               CObjectFactory::CreateObject<typename U::RelChild>();
         group->childList.insert(group->childList.end(), value);
         return (value);
      }
      else if (CGroupFactory::HasChild(group, id))
         return (CGroupFactory::GetChild(group, id));
      else
      {
         boost::shared_ptr<typename U::RelChild> value =
               CObjectFactory::CreateObject<typename U::RelChild>(id);
         group->childList.insert(group->childList.end(), value);
         group->childMap.insert(std::make_pair(id, value));
         return (value);
      }
   }

   template <typename U>
      bool CGroupFactory::HasGroup(boost::shared_ptr<U> group, const StdString & id)
   {  return (group->groupMap.find(id) != group->groupMap.end()); }

   template <typename U>
      bool CGroupFactory::HasChild(boost::shared_ptr<U> group, const StdString & id)
   {  return (group->childMap.find(id) != group->childMap.end()); }

   template <typename U>
      int CGroupFactory::GetGroupNum(boost::shared_ptr<U> group)
   { return (group->groupList.size()); }

   template <typename U>
      int CGroupFactory::GetGroupIdNum(boost::shared_ptr<U> group)
   { return (group->groupMap.size()); }

   template <typename U>
      int CGroupFactory::GetChildNum(boost::shared_ptr<U> group)
   { return (group->childList.size()); }

   template <typename U>
      int CGroupFactory::GetChildIdNum(boost::shared_ptr<U> group)
   { return (group->childMap.size()); }

   template <typename U>
      boost::shared_ptr<U>
         CGroupFactory::GetGroup(boost::shared_ptr<U> group, const StdString & id)
   {
      if (!CGroupFactory::HasGroup<U>(group, id))
         ERROR("CGroupFactory::GetGroup(boost::shared_ptr<U> group, const StdString & id)",
               << "[ id = " << id << ", U = " << U::GetName() << " ] "
               << " group is not referenced !");
      return (group->groupMap[id]);
   }

   template <typename U>
      boost::shared_ptr<typename U::RelChild>
         CGroupFactory::GetChild(boost::shared_ptr<U> group, const StdString & id)
   {
      if (!CGroupFactory::HasChild<U>(group, id))
         ERROR("CGroupFactory::GetChild(boost::shared_ptr<U> group, const StdString & id)",
               << "[ id = " << id << ", U = " << U::GetName() << " ] "
               << " child is not referenced !");
      return (group->childMap[id]);
   }

} // namespace xmlioserver

#endif // __XMLIO_CGroupFactory_impl__
