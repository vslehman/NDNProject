/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  The University of Memphis,
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

#include "nlsr-runner.hpp"

#include "conf-file-processor.hpp"
#include "logger.hpp"

namespace nlsr {

INIT_LOGGER("NlsrRunner");

NlsrRunner::NlsrRunner(std::string& configFileName, bool isDaemonProcess)
  : m_scheduler(m_ioService)
  , m_face(m_ioService)
  , m_nlsr(m_ioService, m_scheduler, m_face)
{
  m_nlsr.setConfFileName(configFileName);
  m_nlsr.setIsDaemonProcess(isDaemonProcess);
}

void
NlsrRunner::run()
{
  ConfFileProcessor configProcessor(m_nlsr, m_nlsr.getConfFileName());

  if (!configProcessor.processConfFile()) {
    throw Error("Error in configuration file processing! Exiting from NLSR");
  }

  if (m_nlsr.getConfParameter().isLog4CxxConfAvailable()) {
    INIT_LOG4CXX(m_nlsr.getConfParameter().getLog4CxxConfPath());
  }
  else {
    INIT_LOGGERS(m_nlsr.getConfParameter().getLogDir(), m_nlsr.getConfParameter().getLogLevel());
  }

  m_nlsr.initialize();

  if (m_nlsr.getIsSetDaemonProcess()) {
    m_nlsr.daemonize();
  }
 
  try {
    
    m_nlsr.startEventLoop();
    
  }
  catch (std::exception& e) {
    _LOG_FATAL("ERROR: " << e.what());
    std::cerr << "ERROR: " << e.what() << std::endl;


    m_nlsr.getFib().clean();
    m_nlsr.destroyFaces();
  }
  _LOG_INFO("After catch");
  	/*

    STATISTICS COUNT
	
	Data collection printed on file

  SENT_HELLO_INTEREST = 1,
  SENT_SYNC_INTEREST,
  SENT_RE_SYNC_INTEREST,
  SENT_LSA_INTEREST,

  SENT_HELLO_DATA,
  SENT_SYNC_DATA,
  SENT_LSA_ADJ_DATA,
  SENT_LSA_COORD_DATA,
  SENT_LSA_NAME_DATA,
  
    RCV_HELLO_INTEREST,
    RCV_SYNC_INTEREST,  
    RCV_RE_SYNC_INTEREST,
    RCV_LSA_INTEREST,

    RCV_HELLO_DATA,
    RCV_SYNC_DATA,
    RCV_LSA_ADJ_DATA,
    RCV_LSA_COORD_DATA,
    RCV_LSA_NAME_DATA,
    

    */
   
  _LOG_INFO("StatisticsCollection" << m_nlsr.getStatistics());
  //m_nlsr.getStatistics().printStatistics();

/*  
  _LOG_INFO("\n\tHello Interest: " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_HELLO_INTEREST));
  _LOG_INFO("\n\tHello Data: " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_HELLO_DATA));
  _LOG_INFO("\n\tSync Interest: " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_SYNC_INTEREST));
  _LOG_INFO("\n\tRe Sync Interest: " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_RE_SYNC_INTEREST));
  _LOG_INFO("\n\tSync Data: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_SYNC_DATA));
  _LOG_INFO("\n\tLSA Interest " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_LSA_INTEREST));
  _LOG_INFO("\n\tAdj Data: " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_LSA_ADJ_DATA));
  _LOG_INFO("\nCoord Data " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_LSA_COORD_DATA));
  _LOG_INFO("\nName Data " << m_nlsr.getStatistics().get(Statistics::PacketType::SENT_LSA_NAME_DATA));

  _LOG_INFO("\n\n\tHello Interest RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_HELLO_INTEREST));
  _LOG_INFO("\n\tHello Data RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_HELLO_DATA));
  _LOG_INFO("\n\tSync Interest RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_SYNC_INTEREST));
  _LOG_INFO("\n\tRe Sync Interest RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_RE_SYNC_INTEREST));
  _LOG_INFO("\n\tSync Data RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_SYNC_DATA));
  _LOG_INFO("\n\tLSA Interest RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_LSA_INTEREST));
  _LOG_INFO("\n\tAdj Data RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_LSA_ADJ_DATA));
  _LOG_INFO("\n\tCoord Data RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_LSA_COORD_DATA));
  _LOG_INFO("\n\tName Data RCV: " << m_nlsr.getStatistics().get(Statistics::PacketType::RCV_LSA_NAME_DATA));
*/
  
}

void
NlsrRunner::printUsage(const std::string& programName)
{
  std::cout << "Usage: " << programName << " [OPTIONS...]" << std::endl;
  std::cout << "   NDN routing...." << std::endl;
  std::cout << "       -d          Run in daemon mode" << std::endl;
  std::cout << "       -f <FILE>   Specify configuration file name" << std::endl;
  std::cout << "       -V          Display version information" << std::endl;
  std::cout << "       -h          Display this help message" << std::endl;
}

} // namespace nlsr
