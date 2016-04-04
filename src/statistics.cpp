/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  The University of Memphis,
 *                           Regents of the University of California,
 *                           Arizona Board of Regents.
 *
 * This file is part of NLSR (Named-data Link State Routing).
 * See AUTHORS.md for complete list of NLSR authors and contributors.
 *
 * NLSR is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NLSR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NLSR, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "nlsr.hpp"
#include "lsdb.hpp"
#include "hello-protocol.hpp"
#include "utility/name-helper.hpp"
#include "statistics.hpp"

namespace nlsr {

Statistics::Statistics()
{

}

void
Statistics::printStatistics()
{
  std::cout << "++++++++++++++++++++++++++++++++++++++++\n" 
            << "+                                      +\n"
            << "+              Statistics              +\n"
            << "+                                      +\n"
            << "++++++++++++++++++++++++++++++++++++++++\n"
            << "HELLO PROTOCOL\n"
            << "\tSent Hello Interests: "              << m_packetCounter[PacketType::SENT_HELLO_INTEREST] << "\n"
            << "\tSent Hello Data: "                   << m_packetCounter[PacketType::SENT_HELLO_DATA] << "\n"
            << "\tReceived Hello Interests: "          << m_packetCounter[PacketType::RCV_HELLO_INTEREST] << "\n"
            << "\tReceived Hello Data: "               << m_packetCounter[PacketType::RCV_HELLO_DATA] << "\n"
            << "\n"
            << "SYNC PROTOCOL\n"
            << "\tSent Sync Interests: "               << m_packetCounter[PacketType::SENT_SYNC_INTEREST] << "\n"
            << "\tSent Sync Recovery Interests: "      << m_packetCounter[PacketType::SENT_SYNC_RECOVERY_INTEREST] << "\n"
            << "\tSent Sync Data: "                    << m_packetCounter[PacketType::SENT_SYNC_DATA] << "\n"
            << "\tSent Sync Recovery Data: "           << m_packetCounter[PacketType::SENT_SYNC_RECOVERY_DATA] << "\n"
            << "\tReceived Sync Interest: "            << m_packetCounter[PacketType::RCV_SYNC_INTEREST] << "\n"
            << "\tReceived Sync Recovery Interest: "   << m_packetCounter[PacketType::RCV_SYNC_RECOVERY_INTEREST] << "\n"
            << "\tReceived Sync Data: "                << m_packetCounter[PacketType::RCV_SYNC_DATA] << "\n"
            << "\tReceived Sync Recovery Data: "       << m_packetCounter[PacketType::RCV_SYNC_RECOVERY_DATA] << "\n"
            << "\n"
            << "LSDB\n"
            << "\tTotal Sent LSA Interests: "          << m_packetCounter[PacketType::SENT_LSA_INTEREST] << "\n"
            << "\tTotal Received LSA Interests: "      << m_packetCounter[PacketType::RCV_LSA_INTEREST] << "\n"
            << "\n"
            << "\tSent Adjacency LSA Interests: "      << m_packetCounter[PacketType::SENT_ADJ_LSA_INTEREST] << "\n"
            << "\tSent Coordinate LSA Interests: "     << m_packetCounter[PacketType::SENT_COORD_LSA_INTEREST] << "\n"
            << "\tSent Name LSA Interests: "           << m_packetCounter[PacketType::SENT_NAME_LSA_INTEREST] << "\n"
            << "\tSent Adjacency LSA Data: "           << m_packetCounter[PacketType::SENT_ADJ_LSA_DATA] << "\n"
            << "\tSent Coordinate LSA Data: "          << m_packetCounter[PacketType::SENT_COORD_LSA_DATA] << "\n"
            << "\tSent Name LSA Data: "                << m_packetCounter[PacketType::SENT_NAME_LSA_DATA] << "\n"
            << "\tReceived Adjacency LSA Interests: "  << m_packetCounter[PacketType::RCV_ADJ_LSA_INTEREST] << "\n"
            << "\tReceived Coordinate LSA Interests: " << m_packetCounter[PacketType::RCV_COORD_LSA_INTEREST] << "\n"
            << "\tReceived Name LSA Interests: "       << m_packetCounter[PacketType::RCV_NAME_LSA_INTEREST] << "\n"
            << "\tReceived Adjacency LSA Data: "       << m_packetCounter[PacketType::RCV_ADJ_LSA_DATA] << "\n"
            << "\tReceived Coordinate LSA Data: "      << m_packetCounter[PacketType::RCV_COORD_LSA_DATA] << "\n"
            << "\tReceived Name LSA Data: "            << m_packetCounter[PacketType::RCV_NAME_LSA_DATA] << "\n"
            << "++++++++++++++++++++++++++++++++++++++++\n";
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
  using PacketType = Statistics::PacketType;

  os << "++++++++++++++++++++++++++++++++++++++++\n" 
     << "+                                      +\n"
     << "+              Statistics              +\n"
     << "+                                      +\n"
     << "++++++++++++++++++++++++++++++++++++++++\n"
     << "HELLO PROTOCOL\n"
     << "\tSent Hello Interests: "              << stats.get(PacketType::SENT_HELLO_INTEREST) << "\n"
     << "\tSent Hello Data: "                   << stats.get(PacketType::SENT_HELLO_DATA) << "\n"
     << "\tReceived Hello Interests: "          << stats.get(PacketType::RCV_HELLO_INTEREST) << "\n"
     << "\tReceived Hello Data: "               << stats.get(PacketType::RCV_HELLO_DATA) << "\n"
     << "\n"
     << "SYNC PROTOCOL\n"
     << "\tSent Sync Interests: "               << stats.get(PacketType::SENT_SYNC_INTEREST) << "\n"
     << "\tSent Sync Recovery Interests: "      << stats.get(PacketType::SENT_SYNC_RECOVERY_INTEREST) << "\n"
     << "\tSent Sync Data: "                    << stats.get(PacketType::SENT_SYNC_DATA) << "\n"
     << "\tSent Sync Recovery Data: "           << stats.get(PacketType::SENT_SYNC_RECOVERY_DATA) << "\n"
     << "\tReceived Sync Interest: "            << stats.get(PacketType::RCV_SYNC_INTEREST) << "\n"
     << "\tReceived Sync Recovery Interest: "   << stats.get(PacketType::RCV_SYNC_RECOVERY_INTEREST) << "\n"
     << "\tReceived Sync Data: "                << stats.get(PacketType::RCV_SYNC_DATA) << "\n"
     << "\tReceived Sync Recovery Data: "       << stats.get(PacketType::RCV_SYNC_RECOVERY_DATA) << "\n"
     << "\n"
     << "LSDB\n"
     << "\tTotal Sent LSA Interests: "          << stats.get(PacketType::SENT_LSA_INTEREST) << "\n"
     << "\tTotal Received LSA Interests: "      << stats.get(PacketType::RCV_LSA_INTEREST) << "\n"
     << "\n"
     << "\tSent Adjacency LSA Interests: "      << stats.get(PacketType::SENT_ADJ_LSA_INTEREST) << "\n"
     << "\tSent Coordinate LSA Interests: "     << stats.get(PacketType::SENT_COORD_LSA_INTEREST) << "\n"
     << "\tSent Name LSA Interests: "           << stats.get(PacketType::SENT_NAME_LSA_INTEREST) << "\n"
     << "\tSent Adjacency LSA Data: "           << stats.get(PacketType::SENT_ADJ_LSA_DATA) << "\n"
     << "\tSent Coordinate LSA Data: "          << stats.get(PacketType::SENT_COORD_LSA_DATA) << "\n"
     << "\tSent Name LSA Data: "                << stats.get(PacketType::SENT_NAME_LSA_DATA) << "\n"
     << "\tReceived Adjacency LSA Interests: "  << stats.get(PacketType::RCV_ADJ_LSA_INTEREST) << "\n"
     << "\tReceived Coordinate LSA Interests: " << stats.get(PacketType::RCV_COORD_LSA_INTEREST) << "\n"
     << "\tReceived Name LSA Interests: "       << stats.get(PacketType::RCV_NAME_LSA_INTEREST) << "\n"
     << "\tReceived Adjacency LSA Data: "       << stats.get(PacketType::RCV_ADJ_LSA_DATA) << "\n"
     << "\tReceived Coordinate LSA Data: "      << stats.get(PacketType::RCV_COORD_LSA_DATA) << "\n"
     << "\tReceived Name LSA Data: "            << stats.get(PacketType::RCV_NAME_LSA_DATA) << "\n"
     << "++++++++++++++++++++++++++++++++++++++++\n";

  return os;
}

} //namespace nlsr
