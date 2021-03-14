/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/brave_wallet_utils.h"

#include <algorithm>
#include <sstream>
#include <utility>

#include "base/feature_list.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "brave/components/brave_wallet/features.h"
#include "brave/third_party/ethash/src/include/ethash/keccak.h"
#include "brave/vendor/bip39wally-core-native/include/wally_bip39.h"
#include "crypto/random.h"
#include "third_party/boringssl/src/include/openssl/evp.h"

namespace brave_wallet {

namespace {
std::string GenerateMnemonicInternal(uint8_t* entropy, size_t size) {
  char* words = nullptr;
  std::string result;
  if (bip39_mnemonic_from_bytes(nullptr, entropy, size, &words) != WALLY_OK) {
    LOG(ERROR) << __func__ << ": bip39_mnemonic_from_bytes failed";
    return result;
  }
  result = words;
  wally_free_string(words);
  return result;
}
}  // namespace

bool IsNativeWalletEnabled() {
  return base::FeatureList::IsEnabled(
      brave_wallet::features::kNativeBraveWalletFeature);
}

std::string ToHex(const std::string& data) {
  if (data.empty()) {
    return "0x0";
  }
  return base::StringPrintf(
      "0x%s",
      base::ToLowerASCII(base::HexEncode(data.data(), data.size())).c_str());
}

std::string KeccakHash(const std::string& input) {
  auto hash = ethash_keccak256(
      reinterpret_cast<uint8_t*>(const_cast<char*>(input.data())),
      input.size());
  std::string result(hash.str, sizeof(hash.str) / sizeof(hash.str[0]));
  return ToHex(result);
}

std::string GetFunctionHash(const std::string& input) {
  auto result = KeccakHash(input);
  return result.substr(0, std::min(static_cast<size_t>(10), input.length()));
}

// Pads a hex encoded parameter to 32-bytes
// i.e. 64 hex characters.
bool PadHexEncodedParameter(const std::string& hex_input, std::string* out) {
  if (!out) {
    return false;
  }
  if (!IsValidHexString(hex_input)) {
    return false;
  }
  if (hex_input.length() >= 64) {
    *out = hex_input;
    return true;
  }
  std::string hex_substr = hex_input.substr(2, hex_input.length() - 2);
  size_t padding_len = 64 - hex_substr.length();
  std::string padding(padding_len, '0');
  *out = base::StringPrintf("0x%s%s", padding.c_str(), hex_substr.c_str());
  return true;
}

// Determines if the passed in hex string is valid
bool IsValidHexString(const std::string& hex_input) {
  if (hex_input.length() < 3) {
    return false;
  }
  if (hex_input.substr(0, 2) != "0x") {
    return false;
  }
  return true;
}

// Takes 2 inputs prefixed by 0x and combines them into an output with a single
// 0x. For example 0x1 and 0x2 would return 0x12
bool ConcatHexStrings(const std::string& hex_input1,
                      const std::string& hex_input2,
                      std::string* out) {
  if (!out) {
    return false;
  }
  if (!IsValidHexString(hex_input1) || !IsValidHexString(hex_input2)) {
    return false;
  }
  *out =
      base::StringPrintf("%s%s", hex_input1.c_str(),
                         hex_input2.substr(2, hex_input2.length() - 2).c_str());
  return true;
}

// Takes a hex string as input and converts it to a uint256_t
bool HexValueToUint256(const std::string& hex_input, uint256_t* out) {
  if (!out) {
    return false;
  }
  if (!IsValidHexString(hex_input)) {
    return false;
  }
  *out = 0;
  std::string hex_substr =
      base::ToLowerASCII(hex_input.substr(2, hex_input.length() - 2));
  for (size_t i = 0; i < hex_substr.length(); i++) {
    (*out) <<= 4;
    if (hex_substr[i] >= '0' && hex_substr[i] <= '9') {
      (*out) += static_cast<uint256_t>(hex_substr[i] - '0');
    } else if (hex_substr[i] >= 'a' && hex_substr[i] <= 'f') {
      (*out) += static_cast<uint256_t>(10 + hex_substr[i] - 'a');
    } else {
      return false;
    }
  }
  return true;
}

// Takes a uint256_t and converts it to a hex string
std::string Uint256ValueToHex(uint256_t input) {
  std::ostringstream ss;
  while (input > static_cast<uint256_t>(0)) {
    char i = static_cast<char>(input & static_cast<uint256_t>(0xF));
    char sz[2] = {'\0'};
    if (i <= 9) {
      sz[0] = '0' + i;
    } else {
      sz[0] = 'a' + (i - 10);
    }
    ss << sz;
    input >>= 4;
  }
  if (ss.str().length() == 0) {
    ss << "0";
  }
  ss << "x0";
  std::string reversed_hex = ss.str();
  std::string out(reversed_hex.rbegin(), reversed_hex.rend());
  return out;
}

std::string GenerateMnemonic(size_t entropy_size) {
  // entropy size should be 128, 160, 192, 224, 256 bits
  if (entropy_size < 16 || entropy_size > 32 || entropy_size % 4 != 0) {
    LOG(ERROR) << __func__ << ": Entropy should be 16, 20, 24, 28, 32 bytes";
    return "";
  }

  std::vector<uint8_t> entropy(entropy_size);
  crypto::RandBytes(&entropy[0], entropy.size());

  return GenerateMnemonicInternal(entropy.data(), entropy.size());
}

std::string GenerateMnemonicForTest(const std::vector<uint8_t>& entropy) {
  return GenerateMnemonicInternal(const_cast<uint8_t*>(entropy.data()),
                                  entropy.size());
}

std::unique_ptr<std::vector<uint8_t>> MnemonicToSeed(
    const std::string& mnemonic,
    const std::string& passphrase) {
  std::unique_ptr<std::vector<uint8_t>> seed =
      std::make_unique<std::vector<uint8_t>>(64);
  const std::string salt = "mnemonic" + passphrase;
  int rv = PKCS5_PBKDF2_HMAC(mnemonic.data(), mnemonic.length(),
                             reinterpret_cast<const uint8_t*>(salt.data()),
                             salt.length(), 2048, EVP_sha512(), seed->size(),
                             seed->data());
  return rv == 1 ? std::move(seed) : nullptr;
}

}  // namespace brave_wallet
