/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_CREATIVES_INLINE_CONTENT_ADS_INLINE_CONTENT_AD_OBSERVER_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_CREATIVES_INLINE_CONTENT_ADS_INLINE_CONTENT_AD_OBSERVER_H_

#include <string>

#include "base/observer_list_types.h"
#include "bat/ads/public/interfaces/ads.mojom.h"

namespace ads {

struct InlineContentAdInfo;

class InlineContentAdObserver : public base::CheckedObserver {
 public:
  // Invoked when an inline content ad is served
  virtual void OnInlineContentAdServed(const InlineContentAdInfo& ad) {}

  // Invoked when an inline content ad is viewed
  virtual void OnInlineContentAdViewed(const InlineContentAdInfo& ad) {}

  // Invoked when an inline content ad is clicked
  virtual void OnInlineContentAdClicked(const InlineContentAdInfo& ad) {}

  // Invoked when an inline content ad event fails
  virtual void OnInlineContentAdEventFailed(
      const std::string& placement_id,
      const std::string& creative_instance_id,
      const mojom::InlineContentAdEventType event_type) {}

 protected:
  ~InlineContentAdObserver() override = default;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_CREATIVES_INLINE_CONTENT_ADS_INLINE_CONTENT_AD_OBSERVER_H_
