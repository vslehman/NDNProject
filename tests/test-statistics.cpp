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

#include "test-common.hpp"
#include "hello-protocol.hpp"
#include "statistics.hpp"
#include "nlsr.hpp"

 #include <ndn-cxx/util/dummy-client-face.hpp>

namespace nlsr {
namespace test {

BOOST_FIXTURE_TEST_SUITE(TestStatistics, BaseFixture)

BOOST_AUTO_TEST_CASE(HelloProtocolStatistics)
{
  std::shared_ptr<ndn::util::DummyClientFace> face = std::make_shared<ndn::util::DummyClientFace>();
  Nlsr nlsr(g_ioService, g_scheduler, ndn::ref(*face));
  HelloProtocol& hello = nlsr.getHelloProtocol();

  ConfParameter& conf = nlsr.getConfParameter();
  conf.setNetwork("/ndn");
  conf.setSiteName("/router");
  conf.setRouterName("/memphis");
  conf.buildRouterPrefix();
  
  nlsr.initialize();

  face->processEvents(ndn::time::milliseconds(1));
  face->sentInterests.clear();

  Adjacent other("/ndn/router/other ", "udp://other", 0, Adjacent::STATUS_ACTIVE, 0, 0);

  // This router's Adjacency LSA
  nlsr.getAdjacencyList().insert(other);

  ndn::Name name(conf.getRouterPrefix());
  name.append("NLSR");
  name.append("INFO");
  name.append(other.getName().wireEncode());

  BOOST_CHECK_EQUAL(face->sentData.size(), 0);

  ndn::Interest interest(name);
  hello.processInterest(ndn::Name(), interest);

  face->processEvents(ndn::time::milliseconds(1));

  BOOST_CHECK_EQUAL(face->sentData.size(), 1);

  hello.expressInterest(name, 1);
  face->processEvents(ndn::time::milliseconds(1));

  BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::SENT_HELLO_INTEREST), 1);
  
  BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::RCV_HELLO_INTEREST), 1);
  BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::SENT_HELLO_DATA), 1);

  // Receive Hello Data
  ndn::Name dataName = other.getName();
  dataName.append("NLSR");
  dataName.append("INFO");
  dataName.append(conf.getRouterPrefix().wireEncode());

  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(dataName);
  hello.onContentValidated(data);

  BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::RCV_HELLO_DATA), 1);

  nlsr.getStatistics().printStatistics();
}

BOOST_AUTO_TEST_SUITE_END()

} //namespace test
} //namespace nlsr