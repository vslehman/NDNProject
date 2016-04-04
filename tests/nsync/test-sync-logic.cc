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

#include "sync-logic.h"
#include "statistics.hpp"

#include <ndn-cxx/security/validator-null.hpp>
#include <ndn-cxx/util/dummy-client-face.hpp>

namespace Sync {
namespace test {

class SyncLogicFixture
{
public:
  SyncLogicFixture()
    : syncPrefix("/ndn/NLSR/sync")
    , validator(std::make_shared<ndn::ValidatorNull>())
    , onUpdate([] (const std::string&) { ; })
    , face(std::make_shared<ndn::util::DummyClientFace>())
    , logic(syncPrefix, validator, face, onUpdate, stats)
  {
  }

  ndn::Name
  getInterestName() const
  {
    FullStatePtr state(new FullState);
    std::ostringstream os;
    os << *state->getDigest();

    ndn::Name interestName(syncPrefix);
    interestName.append(os.str());

    return interestName;
  }

  ndn::Name
  getRecoveryInterestName() const
  {
    FullStatePtr state(new FullState);
    std::ostringstream os;
    os << *state->getDigest();

    ndn::Name interestName(syncPrefix);
    interestName.append("recovery");
    interestName.append(os.str());

    return interestName;
  }

public:
  const ndn::Name syncPrefix;
  std::shared_ptr<ndn::Validator> validator;
  std::function<void(const std::string &)> onUpdate;
  std::shared_ptr<ndn::util::DummyClientFace> face;
  nlsr::Statistics stats;
  SyncLogic logic;
};

BOOST_FIXTURE_TEST_SUITE(TestSyncLogic, SyncLogicFixture)

BOOST_AUTO_TEST_CASE(SendSyncInterest)
{
  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::SENT_SYNC_INTEREST), 0);

  face->processEvents(ndn::time::milliseconds(1));

  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::SENT_SYNC_INTEREST), 1);
}

BOOST_AUTO_TEST_CASE(ReceiveSyncInterest)
{
  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_INTEREST), 0);

  ndn::Interest interest(getInterestName());
  logic.onSyncInterest(syncPrefix, interest);

  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_INTEREST), 1);
}

BOOST_AUTO_TEST_CASE(ReceiveRecoveryInterest)
{
  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_RECOVERY_INTEREST), 0);

  ndn::Interest interest(getRecoveryInterestName());
  logic.onSyncInterest(syncPrefix, interest);

  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_RECOVERY_INTEREST), 1);
}

BOOST_AUTO_TEST_CASE(ReceiveSyncData)
{
  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_DATA), 0);

  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(getInterestName());
  logic.onSyncDataValidated(data);

  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_DATA), 1);
}

BOOST_AUTO_TEST_CASE(ReceiveRecoveryData)
{
  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_RECOVERY_DATA), 0);

  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(getRecoveryInterestName());
  logic.onSyncDataValidated(data);

  BOOST_CHECK_EQUAL(stats.get(nlsr::Statistics::PacketType::RCV_SYNC_RECOVERY_DATA), 1);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace Sync
