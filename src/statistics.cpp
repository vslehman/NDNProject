/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "nlsr.hpp"
#include "lsdb.hpp"
#include "hello-protocol.hpp"
#include "utility/name-helper.hpp"
#include "statistics.hpp"


namespace nlsr
{
 	
 	Statistics::Statistics()
 	{
 
 	}

	


	void
	Statistics::printStatistics()
	{
		
		std::cout   <<"\n++++++++++++++++++++++++++++++++++++++++\n" 
					<< "+                                      +\n"
					<< "+              Statistics              +\n"
					<< "+                                      +\n"
					<< "++++++++++++++++++++++++++++++++++++++++\n+\n"
					<< "+ SENT DATA:\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << m_packetCounter[PacketType::SENT_HELLO_INTEREST]<< "\n"
					<< "+\tSync Interests: " << m_packetCounter[PacketType::SENT_SYNC_INTEREST] << "\n"
					<< "+\tReSync Interests: " << m_packetCounter[PacketType::SENT_RE_SYNC_INTEREST] << "\n"
					<< "+\tLSA Interests: " << m_packetCounter[PacketType::SENT_LSA_INTEREST] << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << m_packetCounter[PacketType::SENT_HELLO_DATA] << "\n"
					<< "+\tSync Data: " << m_packetCounter[PacketType::SENT_SYNC_DATA] << "\n"
					<< "+\tAdj Data: " << m_packetCounter[PacketType::SENT_LSA_ADJ_DATA]<< "\n"
					<< "+\tCoord Data: " << m_packetCounter[PacketType::SENT_LSA_COORD_DATA] << "\n"
					<< "+\tName Data: " << m_packetCounter[PacketType::SENT_LSA_NAME_DATA]<< "\n\n"
					//<< "+ Total Interest: " << m_packetCounter[0) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[) << "\n"
					//<< "+ Total Data: " << m_packetCounter[0)() + m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[)
					<< "\n+\n RECEIVED DATA:\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << m_packetCounter[PacketType::RCV_HELLO_INTEREST]<< "\n"
					<< "+\tSync Interests: " << m_packetCounter[PacketType::RCV_SYNC_INTEREST] << "\n"
					<< "+\tReSync Interests: " << m_packetCounter[PacketType::RCV_RE_SYNC_INTEREST] << "\n"
					<< "+\tLSA Interests: " << m_packetCounter[PacketType::RCV_LSA_INTEREST] << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << m_packetCounter[PacketType::RCV_HELLO_DATA] << "\n"
					<< "+\tSync Data: " << m_packetCounter[PacketType::RCV_SYNC_DATA] << "\n"
					<< "+\tAdj Data: " << m_packetCounter[PacketType::RCV_LSA_ADJ_DATA]<< "\n"
					<< "+\tCoord Data: " << m_packetCounter[PacketType::RCV_LSA_COORD_DATA] << "\n"
					<< "+\tName Data: " << m_packetCounter[PacketType::RCV_LSA_NAME_DATA]<< "\n\n"
					//<< "+ Total Interest: " << m_packetCounter[) + m_packetCounter[) + m_packetCounter[) << "\n"
					//<< "+ Total Data: " << m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[)
					<< "\n++++++++++++++++++++++++++++++++++++++++++\n";
		
	}



	size_t 
 	Statistics::get(PacketType type) const
	{
		std::map<PacketType,int>::const_iterator it = m_packetCounter.find(type);
		if(it != m_packetCounter.end())
		{
			return it->second;
		}
		else
		{
			return 0;
		}
	}
	void
	Statistics::increment (PacketType type)
	{
	  int &value = m_packetCounter[type];
	  ++value;
		/*std::map<PacketType,int>::iterator it = m_packetCounter.find(type);
		if(it != m_packetCounter.end())
		{
			++(it->second);
		}
		else
		{
			std::cout << "Trace INCREMENT RCV_HELLO_INTEREST" << std::endl;
			m_packetCounter[type] = 1;
		}*/
	}

	void
	Statistics::resetAll()
	{
		for( auto& it : m_packetCounter )
		{
			it.second = 0;
		}
	}


	std::ostream&
	operator<<(std::ostream& os, const Statistics& stats) 
	{
	//const std::map<Statistics::PacketType,int>& m_packetCounter = stats.getCounter();
	
	os              <<"\n++++++++++++++++++++++++++++++++++++++++\n" 
					<< "+                                      +\n"
					<< "+              Statistics              +\n"
					<< "+                                      +\n"
					<< "++++++++++++++++++++++++++++++++++++++++\n+\n"
					<< "+ SENT DATA:\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << stats.get(Statistics::PacketType::SENT_HELLO_INTEREST) << "\n"
					<< "+\tSync Interests: " << stats.get(Statistics::PacketType::SENT_SYNC_INTEREST) << "\n"
					<< "+\tReSync Interests: " << stats.get(Statistics::PacketType::SENT_RE_SYNC_INTEREST) << "\n"
					<< "+\tLSA Interests: " << stats.get(Statistics::PacketType::SENT_LSA_INTEREST) << "\n"
					<< "+\tLSA Name Interests: " << stats.get(Statistics::PacketType::SENT_NAME_LSA_INTEREST) << "\n"
					<< "+\tLSA Adj Interests: " << stats.get(Statistics::PacketType::SENT_ADJ_LSA_INTEREST) << "\n"
					<< "+\tLSA Coord Interests: " << stats.get(Statistics::PacketType::SENT_COORD_LSA_INTEREST) << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << stats.get(Statistics::PacketType::SENT_HELLO_DATA) << "\n"
					<< "+\tSync Data: " << stats.get(Statistics::PacketType::SENT_SYNC_DATA) << "\n"
					<< "+\tAdj Data: " << stats.get(Statistics::PacketType::SENT_LSA_ADJ_DATA)<< "\n"
					<< "+\tCoord Data: " << stats.get(Statistics::PacketType::SENT_LSA_COORD_DATA) << "\n"
					<< "+\tName Data: " << stats.get(Statistics::PacketType::SENT_LSA_NAME_DATA)<< "\n\n"
					//<< "+ Total Interest: " << stats.get(0) + stats.get() + stats.get() + stats.get() << "\n"
					//<< "+ Total Data: " << stats.get(0)() + stats.get() + stats.get() + stats.get() + stats.get()
					<< "\n+\n RECEIVED DATA:\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << stats.get(Statistics::PacketType::RCV_HELLO_INTEREST)<< "\n"
					<< "+\tSync Interests: " << stats.get(Statistics::PacketType::RCV_SYNC_INTEREST) << "\n"
					<< "+\tReSync Interests: " << stats.get(Statistics::PacketType::RCV_RE_SYNC_INTEREST) << "\n"
					<< "+\tLSA Interests: " << stats.get(Statistics::PacketType::RCV_LSA_INTEREST) << "\n"
					<< "+\tLSA Name Interests: " << stats.get(Statistics::PacketType::RCV_NAME_LSA_INTEREST) << "\n"
					<< "+\tLSA Adj Interests: " << stats.get(Statistics::PacketType::RCV_ADJ_LSA_INTEREST) << "\n"
					<< "+\tLSA Coord Interests: " << stats.get(Statistics::PacketType::RCV_COORD_LSA_INTEREST) << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << stats.get(Statistics::PacketType::RCV_HELLO_DATA) << "\n"
					<< "+\tSync Data: " << stats.get(Statistics::PacketType::RCV_SYNC_DATA) << "\n"
					<< "+\tAdj Data: " << stats.get(Statistics::PacketType::RCV_LSA_ADJ_DATA)<< "\n"
					<< "+\tCoord Data: " << stats.get(Statistics::PacketType::RCV_LSA_COORD_DATA) << "\n"
					<< "+\tName Data: " << stats.get(Statistics::PacketType::RCV_LSA_NAME_DATA)<< "\n\n"
					//<< "+ Total Interest: " << m_packetCounter[) + m_packetCounter[) + m_packetCounter[) << "\n"
					//<< "+ Total Data: " << m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[)
					<< "\n++++++++++++++++++++++++++++++++++++++++++\n";

	  return os;
	}


	


}//namespace nlsr