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

 #ifndef NDN_STATISTICS_HPP
 #define NDN_STATISTICS_HPP


 #include <map>
 //#include "enum_stats.hpp"


 namespace nlsr 
{

 	class Statistics
 	{
 
 	public:
 		enum class PacketType
 		{
 		SENT_HELLO_INTEREST = 1,
 		RCV_HELLO_INTEREST,
 		SENT_HELLO_DATA,
 		RCV_HELLO_DATA,
 		SENT_SYNC_INTEREST,
 		SENT_RE_SYNC_INTEREST,
 		RCV_SYNC_INTEREST,
 		SENT_SYNC_DATA,
 		RCV_SYNC_DATA,
 		SENT_LSA_INTEREST,
 		SENT_LSA_ADJ_DATA,
 		SENT_LSA_COORD_DATA,
 		SENT_LSA_NAME_DATA,
 		RCV_LSA_INTEREST,
 		RCV_LSA_ADJ_DATA,
 		RCV_LSA_COORD_DATA,
 		RCV_LSA_NAME_DATA,
 		RCV_RE_SYNC_INTEREST

 	}; 

 		Statistics();
 		 		
 		void 
 		printStatistics();

 		/***** GETTERS *****/
 		size_t 
 		get(PacketType);
		
		void
		resetAll();

		/***** SETTERS *****/

 		void 
 		increment(PacketType);

 		
	
		private:

	 		std::map<PacketType,int> m_packetCounter;

  	};

  	std::ostream&
	operator<<(std::ostream&, const Statistics& );
  	//Statistics stats;

}

 #endif
