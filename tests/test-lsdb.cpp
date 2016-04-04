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

#include "test-common.hpp"

#include "lsdb.hpp"
#include "nlsr.hpp"
#include "lsa.hpp"
#include "name-prefix-list.hpp"
#include <boost/test/unit_test.hpp>

#include <ndn-cxx/util/dummy-client-face.hpp>

namespace nlsr {
namespace test {

using ndn::shared_ptr;

class LsdbFixture : public BaseFixture
{
public:
  LsdbFixture()
    : face(make_shared<ndn::util::DummyClientFace>())
    , nlsr(g_ioService, g_scheduler, ndn::ref(*face))
    , sync(*face, nlsr.getLsdb(), nlsr.getConfParameter(), nlsr.getSequencingManager())
    , lsdb(nlsr.getLsdb())
    , conf(nlsr.getConfParameter())
    , REGISTER_COMMAND_PREFIX("/localhost/nfd/rib")
    , REGISTER_VERB("register")
  {
    conf.setNetwork("/ndn");
    conf.setSiteName("/site");
    conf.setRouterName("/%C1.router/this-router");

    nlsr.initialize();

    face->processEvents(ndn::time::milliseconds(1));
    face->sentInterests.clear();

    INIT_LOGGERS("/tmp", "DEBUG");
  }

  void
  extractParameters(ndn::Interest& interest, ndn::Name::Component& verb,
                    ndn::nfd::ControlParameters& extractedParameters)
  {
    const ndn::Name& name = interest.getName();
    verb = name[REGISTER_COMMAND_PREFIX.size()];
    const ndn::Name::Component& parameterComponent = name[REGISTER_COMMAND_PREFIX.size() + 1];

    ndn::Block rawParameters = parameterComponent.blockFromValue();
    extractedParameters.wireDecode(rawParameters);
  }

  void
  areNamePrefixListsEqual(NamePrefixList& lhs, NamePrefixList& rhs)
  {
    typedef std::list<ndn::Name> NameList;

    NameList& lhsList = lhs.getNameList();
    NameList& rhsList = rhs.getNameList();

    BOOST_REQUIRE_EQUAL(lhsList.size(), rhsList.size());

    NameList::iterator i = lhsList.begin();
    NameList::iterator j = rhsList.begin();

    for (; i != lhsList.end(); ++i, ++j) {
      BOOST_CHECK_EQUAL(*i, *j);
    }
  }

  void
  receiveInterestAndCheckSentStats(const std::string& interestPrefix,
                                   const std::string& lsaType,
                                   uint32_t seqNo,
                                   Statistics::PacketType receivedInterestType,
                                   Statistics::PacketType sentDataType)
  {
    size_t rcvBefore = nlsr.getStatistics().get(receivedInterestType);
    size_t sentBefore = nlsr.getStatistics().get(sentDataType);

    ndn::Name interestName = ndn::Name(ndn::Name(interestPrefix + lsaType).appendNumber(seqNo));
    lsdb.processInterest(ndn::Name(), ndn::Interest(interestName));
    face->processEvents(ndn::time::milliseconds(1));

    BOOST_CHECK_EQUAL(nlsr.getStatistics().get(receivedInterestType), rcvBefore + 1);
    BOOST_CHECK_EQUAL(nlsr.getStatistics().get(sentDataType), sentBefore + 1);
  }

  void
  sendInterestAndCheckStats(const std::string& interestPrefix,
                            const std::string& lsaType,
                            uint32_t seqNo,
                            Statistics::PacketType sentInterestType)
  {
    size_t sentBefore = nlsr.getStatistics().get(sentInterestType);

    lsdb.expressInterest(ndn::Name(interestPrefix + lsaType).appendNumber(seqNo), 0);
    face->processEvents(ndn::time::milliseconds(1));

    BOOST_CHECK_EQUAL(nlsr.getStatistics().get(sentInterestType), sentBefore + 1);
  }

  void
  receiveDataAndCheckStats(const std::string& interestPrefix,
                            const std::string& lsaType,
                            uint32_t seqNo,
                            Statistics::PacketType receivedDataType)
  {
    size_t rcvBefore = nlsr.getStatistics().get(receivedDataType);

    ndn::Name dataName(ndn::Name(interestPrefix + lsaType).appendNumber(seqNo).appendVersion());
    std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(dataName);
    lsdb.onContentValidated(data);

    BOOST_CHECK_EQUAL(nlsr.getStatistics().get(receivedDataType), rcvBefore + 1);
  }

public:
  shared_ptr<ndn::util::DummyClientFace> face;
  Nlsr nlsr;
  SyncLogicHandler sync;

  Lsdb& lsdb;
  ConfParameter& conf;

  ndn::Name REGISTER_COMMAND_PREFIX;
  ndn::Name::Component REGISTER_VERB;
};

BOOST_FIXTURE_TEST_SUITE(TestLsdb, LsdbFixture)

BOOST_AUTO_TEST_CASE(LsdbSync)
{
  ndn::Name interestName("/ndn/NLSR/LSA/cs/%C1.Router/router2/name");
  uint64_t oldSeqNo = 82;

  ndn::Name oldInterestName = interestName;
  oldInterestName.appendNumber(oldSeqNo);

  lsdb.expressInterest(oldInterestName, 0);
  face->processEvents(ndn::time::milliseconds(1));

  std::vector<ndn::Interest>& interests = face->sentInterests;

  BOOST_REQUIRE(interests.size() > 0);
  std::vector<ndn::Interest>::iterator it = interests.begin();

  BOOST_CHECK_EQUAL(it->getName(), oldInterestName);
  interests.clear();

  steady_clock::TimePoint deadline = steady_clock::now() +
                                     ndn::time::seconds(static_cast<int>(LSA_REFRESH_TIME_MAX));

  // Simulate an LSA interest timeout
  lsdb.processInterestTimedOut(oldInterestName, 0, deadline, interestName, oldSeqNo);
  face->processEvents(ndn::time::milliseconds(1));

  BOOST_REQUIRE(interests.size() > 0);
  it = interests.begin();

  BOOST_CHECK_EQUAL(it->getName(), oldInterestName);
  interests.clear();

  uint64_t newSeqNo = 83;

  ndn::Name newInterestName = interestName;
  newInterestName.appendNumber(newSeqNo);

  lsdb.expressInterest(newInterestName, 0);
  face->processEvents(ndn::time::milliseconds(1));

  BOOST_REQUIRE(interests.size() > 0);
  it = interests.begin();

  BOOST_CHECK_EQUAL(it->getName(), newInterestName);
  interests.clear();

  // Simulate an LSA interest timeout where the sequence number is outdated
  lsdb.processInterestTimedOut(oldInterestName, 0, deadline, interestName, oldSeqNo);
  face->processEvents(ndn::time::milliseconds(1));

  // Interest should not be expressed for outdated sequence number
  BOOST_CHECK_EQUAL(interests.size(), 0);
  lsdb.processInterest(ndn::Name(),ndn::Interest(interestName));
  face->processEvents(ndn::time::milliseconds(1));

  //BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::RCV_LSA_INTEREST), 1);
  //BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::SENT_LSA_INTEREST), 3);
  //nlsr.getStatistics().printStatistics();
}

BOOST_AUTO_TEST_CASE(LsdbRemoveAndExists)
{
  ndn::time::system_clock::TimePoint testTimePoint =  ndn::time::system_clock::now();
  NamePrefixList npl1;

  std::string s1 = "name1";
  std::string s2 = "name2";
  std::string routerName = "/ndn/site/%C1.Router/router/";

  npl1.insert(s1);
  npl1.insert(s2);

  //For NameLsa lsType is name.
  //12 is seqNo, randomly generated.
  //1800 is the default life time.
  NameLsa nlsa1(ndn::Name(routerName), NameLsa::TYPE_STRING, 12, testTimePoint, npl1);

  lsdb.installNameLsa(nlsa1);
  lsdb.writeNameLsdbLog();

  BOOST_CHECK(lsdb.doesLsaExist(ndn::Name(routerName + NameLsa::TYPE_STRING), NameLsa::TYPE_STRING));

  lsdb.removeNameLsa(nlsa1.getKey());
  BOOST_CHECK_EQUAL(lsdb.doesLsaExist(ndn::Name(routerName + NameLsa::TYPE_STRING), NameLsa::TYPE_STRING), false);
}

BOOST_AUTO_TEST_CASE(InstallNameLsa)
{
  // Install lsa with name1 and name2
  ndn::Name name1("/ndn/name1");
  ndn::Name name2("/ndn/name2");

  NamePrefixList prefixes;
  prefixes.insert(name1);
  prefixes.insert(name2);

  std::string otherRouter("/ndn/site/%C1.router/other-router");
  ndn::time::system_clock::TimePoint MAX_TIME = ndn::time::system_clock::TimePoint::max();

  NameLsa lsa(otherRouter, NameLsa::TYPE_STRING, 1, MAX_TIME, prefixes);
  lsdb.installNameLsa(lsa);

  BOOST_REQUIRE_EQUAL(lsdb.doesLsaExist(otherRouter + "/name", NameLsa::TYPE_STRING), true);
  NamePrefixList& nameList = lsdb.findNameLsa(otherRouter + "/name")->getNpl();

  areNamePrefixListsEqual(nameList, prefixes);

  // Add a prefix: name3
  ndn::Name name3("/ndn/name3");
  prefixes.insert(name3);

  NameLsa addLsa(otherRouter, NameLsa::TYPE_STRING, 2, MAX_TIME, prefixes);
  lsdb.installNameLsa(addLsa);

  // Lsa should include name1, name2, and name3
  areNamePrefixListsEqual(nameList, prefixes);

  // Remove a prefix: name2
  prefixes.remove(name2);

  NameLsa removeLsa(otherRouter, NameLsa::TYPE_STRING, 3, MAX_TIME, prefixes);
  lsdb.installNameLsa(removeLsa);

  // Lsa should include name1 and name3
  areNamePrefixListsEqual(nameList, prefixes);

  // Add and remove a prefix: add name2, remove name3
  prefixes.insert(name2);
  prefixes.remove(name3);

  NameLsa addAndRemoveLsa(otherRouter, NameLsa::TYPE_STRING, 4, MAX_TIME, prefixes);
  lsdb.installNameLsa(addAndRemoveLsa);

  // Lsa should include name1 and name2
  areNamePrefixListsEqual(nameList, prefixes);

  // Install a completely new list of prefixes
  ndn::Name name4("/ndn/name4");
  ndn::Name name5("/ndn/name5");

  NamePrefixList newPrefixes;
  newPrefixes.insert(name4);
  newPrefixes.insert(name5);

  NameLsa newLsa(otherRouter, NameLsa::TYPE_STRING, 5, MAX_TIME, newPrefixes);
  lsdb.installNameLsa(newLsa);

  // Lsa should include name4 and name5
  areNamePrefixListsEqual(nameList, newPrefixes);
  //nlsr.getStatistics().printStatistics();
}

BOOST_AUTO_TEST_CASE(SeqNo)
{
  std::string otherRouter("/ndn/site/%C1.router/other-router");
  ndn::time::system_clock::TimePoint MAX_TIME = ndn::time::system_clock::TimePoint::max();
  uint64_t prevSeqNo = lsdb.getSeqNo();

  // Adjacency LSA
  Adjacent adjacency("adjacency");
  adjacency.setStatus(Adjacent::STATUS_ACTIVE);

  AdjacencyList adjacencies;
  adjacencies.insert(adjacency);

  AdjLsa adjLsa(otherRouter, AdjLsa::TYPE_STRING, 12, MAX_TIME, 1, adjacencies);

  // Install
  lsdb.installAdjLsa(adjLsa);

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Update
  uint32_t lsSeqNo = adjLsa.getLsSeqNo();
  adjLsa.setLsSeqNo(++lsSeqNo);
  lsdb.installAdjLsa(adjLsa);

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Remove
  lsdb.removeAdjLsa(adjLsa.getKey());

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Name LSA
  NamePrefixList prefixes;
  prefixes.insert("/ndn/name");

  NameLsa nameLsa(otherRouter, NameLsa::TYPE_STRING, 1, MAX_TIME, prefixes);

  // Install
  lsdb.installNameLsa(nameLsa);

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Update
  lsSeqNo = nameLsa.getLsSeqNo();
  nameLsa.setLsSeqNo(++lsSeqNo);
  lsdb.installNameLsa(nameLsa);

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Remove
  lsdb.removeNameLsa(nameLsa.getKey());

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Coordinate LSA
  CoordinateLsa coordLsa(otherRouter, CoordinateLsa::TYPE_STRING, 12, MAX_TIME, 2.5, 30.0);

  // Install
  lsdb.installCoordinateLsa(coordLsa);

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Update
  lsSeqNo = coordLsa.getLsSeqNo();
  coordLsa.setLsSeqNo(++lsSeqNo);
  lsdb.installCoordinateLsa(coordLsa);

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();

  // Remove
  lsdb.removeCoordinateLsa(coordLsa.getKey());

  BOOST_CHECK_EQUAL(lsdb.getSeqNo(), prevSeqNo + 1);
  prevSeqNo = lsdb.getSeqNo();
}

BOOST_AUTO_TEST_CASE(StatisticsReceiveInterestSendData)
{
  std::string routerName("/ndn/site/%C1.Router/router");
  ndn::time::system_clock::TimePoint MAX_TIME = ndn::time::system_clock::TimePoint::max();
  uint32_t seqNo = 1;

  // Adjacency LSA
  Adjacent adjacency("adjacency");
  adjacency.setStatus(Adjacent::STATUS_ACTIVE);

  AdjacencyList adjacencies;
  adjacencies.insert(adjacency);

  AdjLsa adjLsa(routerName, AdjLsa::TYPE_STRING, seqNo, MAX_TIME, 1, adjacencies);
  lsdb.installAdjLsa(adjLsa);

  const std::string interestPrefix("/ndn/NLSR/LSA/site/%C1.Router/router/");

  // Receive Adjacency LSA Interest
  receiveInterestAndCheckSentStats(interestPrefix,
                                   AdjLsa::TYPE_STRING,
                                   seqNo,
                                   Statistics::PacketType::RCV_ADJ_LSA_INTEREST,
                                   Statistics::PacketType::SENT_ADJ_LSA_DATA);

  // Name LSA
  NamePrefixList prefixes;
  prefixes.insert("/ndn/name");

  NameLsa nameLsa(routerName, NameLsa::TYPE_STRING, seqNo, MAX_TIME, prefixes);
  lsdb.installNameLsa(nameLsa);

  // Receive Name LSA Interest
  receiveInterestAndCheckSentStats(interestPrefix,
                                   NameLsa::TYPE_STRING,
                                   seqNo,
                                   Statistics::PacketType::RCV_NAME_LSA_INTEREST,
                                   Statistics::PacketType::SENT_NAME_LSA_DATA);

  // Coordinate LSA
  CoordinateLsa coordLsa(routerName, CoordinateLsa::TYPE_STRING, seqNo, MAX_TIME, 2.5, 30.0);
  lsdb.installCoordinateLsa(coordLsa);

  // Receive Adjacency LSA Interest
  receiveInterestAndCheckSentStats(interestPrefix,
                                   CoordinateLsa::TYPE_STRING,
                                   seqNo,
                                   Statistics::PacketType::RCV_COORD_LSA_INTEREST,
                                   Statistics::PacketType::SENT_COORD_LSA_DATA);

  BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::RCV_ADJ_LSA_INTEREST) +
                    nlsr.getStatistics().get(Statistics::PacketType::RCV_COORD_LSA_INTEREST) +
                    nlsr.getStatistics().get(Statistics::PacketType::RCV_NAME_LSA_INTEREST),
                    nlsr.getStatistics().get(Statistics::PacketType::RCV_LSA_INTEREST));
}

BOOST_AUTO_TEST_CASE(StatisticsSendInterest)
{
  const std::string interestPrefix("/ndn/NLSR/LSA/site/%C1.Router/router/");
  uint32_t seqNo = 1;

  // Adjacency LSA
  sendInterestAndCheckStats(interestPrefix, AdjLsa::TYPE_STRING, seqNo, Statistics::PacketType::SENT_ADJ_LSA_INTEREST);

  // Coordinate LSA
  sendInterestAndCheckStats(interestPrefix, CoordinateLsa::TYPE_STRING, seqNo, Statistics::PacketType::SENT_COORD_LSA_INTEREST);

  // Name LSA
  sendInterestAndCheckStats(interestPrefix, NameLsa::TYPE_STRING, seqNo, Statistics::PacketType::SENT_NAME_LSA_INTEREST);

  BOOST_CHECK_EQUAL(nlsr.getStatistics().get(Statistics::PacketType::SENT_ADJ_LSA_INTEREST) +
                    nlsr.getStatistics().get(Statistics::PacketType::SENT_COORD_LSA_INTEREST) +
                    nlsr.getStatistics().get(Statistics::PacketType::SENT_NAME_LSA_INTEREST),
                    nlsr.getStatistics().get(Statistics::PacketType::SENT_LSA_INTEREST));
}

BOOST_AUTO_TEST_CASE(StatisticsReceiveData)
{
  const std::string interestPrefix("/ndn/NLSR/LSA/site/%C1.Router/router/");
  uint32_t seqNo = 1;

  // Adjacency LSA
  receiveDataAndCheckStats(interestPrefix, AdjLsa::TYPE_STRING, seqNo, Statistics::PacketType::RCV_ADJ_LSA_DATA);

  // Coordinate LSA
  receiveDataAndCheckStats(interestPrefix, CoordinateLsa::TYPE_STRING, seqNo, Statistics::PacketType::RCV_COORD_LSA_DATA);

  // Name LSA
  receiveDataAndCheckStats(interestPrefix, NameLsa::TYPE_STRING, seqNo, Statistics::PacketType::RCV_NAME_LSA_DATA);
}

BOOST_AUTO_TEST_SUITE_END()

} //namespace test
} //namespace nlsr
