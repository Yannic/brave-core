/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/history/filters/history_filter_factory.h"

#include "bat/ads/internal/history/filters/history_confirmation_filter.h"

namespace ads {

std::unique_ptr<HistoryFilterInterface> HistoryFilterFactory::Build(
    const HistoryFilterType type) {
  switch (type) {
    case HistoryFilterType::kNone: {
      return nullptr;
    }

    case HistoryFilterType::kConfirmationType: {
      return std::make_unique<HistoryConfirmationFilter>();
    }
  }
}

}  // namespace ads
