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

export const removeAllFilters = () => {
  chrome.storage.local.set({ 'cosmeticFilterList': {} })
}

// TODO - remove all remaining reads/writes to cosmeticFilterList
export const addSiteCosmeticFilter = async (origin: string, cssfilter: string) => {
  chrome.storage.local.get('cosmeticFilterList', (storeData = {}) => {
    let storeList = Object.assign({}, storeData.cosmeticFilterList)
    if (storeList[origin] === undefined || storeList[origin].length === 0) { // nothing in filter list for origin
      storeList[origin] = [cssfilter]
    } else { // add entry
      storeList[origin].push(cssfilter)
    }
    chrome.storage.local.set({ 'cosmeticFilterList': storeList })
  })
}

export const removeSiteFilter = (origin: string) => {
  chrome.storage.local.get('cosmeticFilterList', (storeData = {}) => {
    let storeList = Object.assign({}, storeData.cosmeticFilterList)
    delete storeList[origin]
    chrome.storage.local.set({ 'cosmeticFilterList': storeList })
  })
}

// Attempt to run the legacy filters migration during brave_extension
// initialization.
tryMigratingLegacyCosmeticFilters()
