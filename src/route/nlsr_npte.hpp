#ifndef NLSR_NPTE_HPP
#define NLSR_NPTE_HPP

#include <list>
#include <utility>
#include "nlsr_rte.hpp"

namespace nlsr
{

  using namespace std;

  class Npte
  {
  public:
    Npte()
      : m_namePrefix()
      , m_nhl()
    {
    }
    Npte(string np)
      : m_nhl()
    {
      m_namePrefix=np;
    }

    string getNamePrefix()
    {
      return m_namePrefix;
    }

    std::list<RoutingTableEntry>& getRteList()
    {
      return m_rteList;
    }

    void resetRteListNextHop()
    {
      if (m_rteList.size() > 0 )
      {
        for( std::list<RoutingTableEntry>::iterator it=m_rteList.begin();
             it != m_rteList.end(); ++it )
        {
          (*it).getNhl().reset();
        }
      }
    }

    int getRteListSize()
    {
      return m_rteList.size();
    }

    Nhl& getNhl()
    {
      return m_nhl;
    }
    void generateNhlfromRteList();
    void removeRoutingTableEntry(RoutingTableEntry& rte);
    void addRoutingTableEntry(RoutingTableEntry &rte);

  private:
    string m_namePrefix;
    std::list<RoutingTableEntry> m_rteList;
    Nhl m_nhl;
  };

  ostream&
  operator<<(ostream& os, Npte& npte);

}//namespace nlsr

#endif