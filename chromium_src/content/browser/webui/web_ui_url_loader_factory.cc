/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "content/public/browser/web_ui_url_loader_factory.h"

#define CreateWebUIURLLoader CreateWebUIURLLoader_ChromiumImpl
#include "../../../../../content/browser/webui/web_ui_url_loader_factory.cc"
#undef CreateWebUIURLLoader

namespace {
constexpr char kBraveUIResourceHost[] = "brave-resources";
}  // namespace

namespace content {

std::unique_ptr<network::mojom::URLLoaderFactory> CreateWebUIURLLoader(
    RenderFrameHost* render_frame_host,
    const std::string& scheme,
    base::flat_set<std::string> allowed_hosts) {
  if (allowed_hosts.find(kChromeUIResourcesHost) != allowed_hosts.end()) {
    allowed_hosts.emplace(kBraveUIResourceHost);
  }

  return CreateWebUIURLLoader_ChromiumImpl(render_frame_host, scheme,
                                           allowed_hosts);
}

}  // namespace content
