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
	//size_t nHelloData, nSyncData, nAdjData, nCoorData, nNameData;
 	//size_t nHelloInt, nSyncInt, nAdjInt, nCoordInt, nNameInt;
 	size_t nHelloData=0, nSyncData=0, nAdjData=0, nCoorData=0, nNameData=0;
 	size_t nHelloInt=0, nSyncInt=0, nLSAInt=0, nReSyncInt=0;
 	Statistics::Statistics()
 	{
 		/*nHelloInt = 0;
 		nSyncInt = 0;
 		nAdjInt = 0;
 		nCoordInt = 0;
 		nNameInt = 0;
 		nHelloData = 0;
 		nSyncData = 0;
 		nAdjData = 0;
 		nCoorData = 0;
 		nNameData = 0;*/
 	}

	void
	Statistics::countInterest(char type)
	{
		//nHelloData = nHelloData + 1;
		switch(type)
		{
			case 'H':
			case 'h':
				nHelloInt++;
				break;
			case 'S':
			case 's':
				nSyncInt++;
				break;
			case 'A':
			case 'a':
				nLSAInt++;
				break;
			case 'R':
			case 'r':
				nReSyncInt++;
				break;
			default:
				std::cout<<"Incorrect argument, no interest count" << std::endl;
				break;
		}

	}

	void
	Statistics::countData(char type)
	{
		//nHelloData = nHelloData + 1;

		switch(type)
		{
			case 'H':
			case 'h':
				nHelloData++;
				break;
			case 'S':
			case 's':
				nSyncData++;
				break;
			case 'A':
			case 'a':
				nAdjData++;
				break;
			case 'C':
			case 'c':
				nCoorData++;
				break;			
			case 'N':
			case 'n':
				nNameData++;
				break;			
			default:
				std::cout<<"Incorrect argument, no interest count" << std::endl;
				break;
		}
	}

	void
	Statistics::printStatistics()
	{
		std::cout 	<< "\n++++++++++++++++++++++++++++++++++++++++\n" 
					<< "+                                      +\n"
					<< "+              Statistics              +\n"
					<< "+                                      +\n"
					<< "++++++++++++++++++++++++++++++++++++++++\n\n"
					<< "Interest:\n"
					<< "\tHello Interests: " << nHelloInt << "\n"
					<< "\tSync Interests: " << nSyncInt << "\n"
					<< "\tReSync Interests: " << nReSyncInt << "\n"
					<< "\tLSA Interests: " << nLSAInt << "\n"
					<< "Data:\n"
					<< "\tHello Data: " << nHelloData << "\n"
					<< "\tSync Data: " << nSyncData << "\n"
					<< "\tAdj Data: " << nAdjData << "\n"
					<< "\tCoord Data: " << nCoorData << "\n"
					<< "\tName Data: " << nNameData << "\n\n"
					<< "Total Interest: " << nHelloInt + nSyncInt + nLSAInt << "\n"
					<< "Total Data: " << nHelloData + nSyncData + nAdjData + nCoorData + nNameData << std::endl;

	}

	/*size_t 
 	Statistics::getHelloInt()
	{
		return nHelloInt;
	}*/

	size_t 
 	Statistics::getHelloData()
	{
		return nHelloData;
	}
	
	std::ostream&
	operator<<(std::ostream& os, const Statistics& stats)
	{
	
	os <<              "\n++++++++++++++++++++++++++++++++++++++++\n" 
					<< "+                                      +\n"
					<< "+              Statistics              +\n"
					<< "+                                      +\n"
					<< "++++++++++++++++++++++++++++++++++++++++\n+\n"
					<< "+ Interest:\n"
					<< "+\tHello Interests: " << nHelloInt << "\n"
					<< "+\tSync Interests: " << nSyncInt << "\n"
					<< "\tReSync Interests: " << nReSyncInt << "\n"
					<< "+\tLSA Interests: " << nLSAInt << "\n"
					<< "+ Data:\n"
					<< "+\tHello Data: " << nHelloData << "\n"
					<< "+\tSync Data: " << nSyncData << "\n"
					<< "+\tAdj Data: " << nAdjData << "\n"
					<< "+\tCoord Data: " << nCoorData << "\n"
					<< "+\tName Data: " << nNameData << "\n\n"
					<< "+ Total Interest: " << nHelloInt + nSyncInt + nLSAInt << "\n"
					<< "+ Total Data: " << nHelloData + nSyncData + nAdjData + nCoorData + nNameData
					<< "\n++++++++++++++++++++++++++++++++++++++++++\n";

	  return os;
	}

	size_t 
 	Statistics::getHelloInt()
	{
		return nHelloInt;
	}

	size_t 
 	Statistics::getSyncInt()
	{
		return nSyncInt;
	}

	size_t 
 	Statistics::getReSyncInt()
 	{
 		return nReSyncInt;
 	}
	
	size_t 
 	Statistics::getSyncData()
	{
		return nSyncData;
	}
	
	size_t 
 	Statistics::getLSAInt()
	{
		return nLSAInt;
	}
	
	size_t 
 	Statistics::getAdjData()
	{
		return nAdjData;
	}
	
	size_t
	Statistics::getCoorData()
	{
		return nCoorData;
	}
	
	size_t
	Statistics::getNameData()
	{
		return nNameData;
	}

	size_t
	Statistics::resetAll()
	{
	nHelloData=0; nSyncData=0; nAdjData=0; nCoorData=0; nNameData=0;
 	nHelloInt=0; nSyncInt=0; nLSAInt=0; nReSyncInt=0;
	}
}//namespace nlsr