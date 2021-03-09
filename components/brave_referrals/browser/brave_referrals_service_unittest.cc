/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_referrals/browser/brave_referrals_service.h"
#include "testing/gtest/include/gtest/gtest.h"

class TestBraveReferralsService : public BraveReferralsService {
 public:
  // intercept/proxy calls here
};

class BraveReferralsServiceTest : public testing::Test {
 public:
  BraveReferralsServiceTest() {}

  void SetUp() override {
    auto* registry = pref_service_.registry();
    brave::RegisterPrefsForBraveReferralsService(registry);
  }

  void Init() {
    service_.reset(new TestBraveReferralsService(nullptr, &pref_service_));
    service_->Init();
  }

  base::test::TaskEnvironment env_;
  TestingPrefServiceSimple pref_service_;
  std::unique_ptr<TestBraveReferralsService> service_;
}

// TEST(ReferralsServiceTest, OnReadPromoCodeComplete) {
// }
