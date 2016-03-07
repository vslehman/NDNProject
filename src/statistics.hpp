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


 #include <set>
 #include "statistics.hpp"

 namespace nlsr {
 	
 	class Statistics
 	{
 	private:

	//size_t nHelloData=0, nSyncData=0, nAdjData=0, nCoorData=0, nNameData=0;
 	//size_t nHelloInt=0, nSyncInt=0, nAdjInt=0, nCoordInt=0, nNameInt=0;


 	public:

 		Statistics();
 		
 		void 
 		countInterest(char);
 		
 		void 
 		countData(char);
 		
 		void 
 		printStatistics();

 		size_t 
 		getHelloData();

 		size_t 
 		getHelloInt();

		size_t 
 		getSyncInt();

 		size_t 
 		getReSyncInt();
	
		size_t 
 		getSyncData();

		size_t 
 		getLSAInt();

		size_t 
 		getAdjData();
	
		size_t
		getCoorData();

		size_t
		getNameData();
		
		size_t
		resetAll();

  	};

  	std::ostream&
	operator<<(std::ostream&, const Statistics& );
  	//Statistics stats;

 }

 #endif
