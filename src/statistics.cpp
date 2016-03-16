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
		std::cout 	<<"\n++++++++++++++++++++++++++++++++++++++++\n" 
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
					<< "+\tSync Data: " << m_packetCounter[PacketType::SENT_HELLO_DATA] << "\n"
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
					<< "+\tSync Data: " << m_packetCounter[PacketType::RCV_HELLO_DATA] << "\n"
					<< "+\tAdj Data: " << m_packetCounter[PacketType::RCV_LSA_ADJ_DATA]<< "\n"
					<< "+\tCoord Data: " << m_packetCounter[PacketType::RCV_LSA_COORD_DATA] << "\n"
					<< "+\tName Data: " << m_packetCounter[PacketType::RCV_LSA_NAME_DATA]<< "\n\n"
					//<< "+ Total Interest: " << m_packetCounter[) + m_packetCounter[) + m_packetCounter[) << "\n"
					//<< "+ Total Data: " << m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[) + m_packetCounter[)
					<< "\n++++++++++++++++++++++++++++++++++++++++++\n";
	}



	size_t 
 	Statistics::get(PacketType type)
	{
		std::map<PacketType,int>::iterator it = m_packetCounter.find(type);
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
	/*
	std::ostream&
	operator<<(std::ostream& os, const Statistics& stats)
	{
	
	os <<              "\n++++++++++++++++++++++++++++++++++++++++\n" 
					<< "+                                      +\n"
					<< "+              Statistics              +\n"
					<< "+                                      +\n"
					<< "++++++++++++++++++++++++++++++++++++++++\n+\n"
					<< "+ SENT DATA:\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << getSentHelloInt() << "\n"
					<< "+\tSync Interests: " << getSyncInt() << "\n"
					<< "\tReSync Interests: " << getReSyncInt() << "\n"
					<< "+\tLSA Interests: " << getSentLSAInt() << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << getSentHelloData() << "\n"
					<< "+\tSync Data: " << getSentSyncData() << "\n"
					<< "+\tAdj Data: " << getSentAdjData() << "\n"
					<< "+\tCoord Data: " << getSentCoorData() << "\n"
					<< "+\tName Data: " << getSentNameData() << "\n\n"
					<< "+ Total Interest: " << getSentHelloInt() + getSyncInt() + getReSyncInt() + getSentLSAInt() << "\n"
					<< "+ Total Data: " << getSentHelloData() + getSentSyncData() + getSentAdjData() + getSentCoorData() + getSentNameData()
					<< "+\n+\n+ RECEIVED DATA:\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << getRcvHelloInt() << "\n"
					<< "+\tSync Interests: " << getRcvSyncInt() << "\n"
					<< "+\tLSA Interests: " << getRcvLSAInt() << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << getRcvHelloData() << "\n"
					<< "+\tSync Data: " << getRcvSyncData() << "\n"
					<< "+\tAdj Data: " << getRcvAdjData() << "\n"
					<< "+\tCoord Data: " << getRcvCoorData() << "\n"
					<< "+\tName Data: " << getRcvNameData() << "\n\n"
					<< "+ Total Interest: " << getRcvHelloInt() + getRcvSyncInt() + getRcvLSAInt() << "\n"
					<< "+ Total Data: " << getRcvHelloData() + getRcvSyncData() + getRcvAdjData() + getRcvCoorData() + getRcvNameData()
					<< "\n++++++++++++++++++++++++++++++++++++++++++\n";

	  return os;
	}
*/

	


}//namespace nlsr