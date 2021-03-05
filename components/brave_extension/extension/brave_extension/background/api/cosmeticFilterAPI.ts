/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// A very early implementation of cosmetic filtering used extension-local
// storage to store CSS selectors of elements to apply hide rules to on a
// per-site basis.
//
// If any of these legacy cosmetic filters are still present, this function
// will migrate them to share the adblock-rust infrastructure used by the
// custom filters engine from brave://adblock.
const tryMigratingLegacyCosmeticFilters = () => {
  chrome.storage.local.get('cosmeticFilterList', (storeData = {}) => {
    if (storeData.cosmeticFilterList !== undefined) {
      const cosmeticFilterList = storeData.cosmeticFilterList
      console.error(JSON.stringify(cosmeticFilterList))
      chrome.braveShields.migrateLegacyCosmeticFilters(cosmeticFilterList, (success) => {
        if (success) {
          chrome.storage.local.remove('cosmeticFilterList')
        } else {
          console.error('Legacy cosmetic filter migration failed!')
        }
      })
    }
  })
}

export const addSiteCosmeticFilter = async (host: string, cssSelector: string) => {
  chrome.braveShields.addSiteCosmeticFilter(host, cssSelector)
}

// Attempt to run the legacy filters migration during brave_extension
// initialization.
tryMigratingLegacyCosmeticFilters()
