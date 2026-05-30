// Copyright (c) 2026 The Block Zero developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_RANDOMX_H
#define BITCOIN_POW_RANDOMX_H

#include <uint256.h>

#include <cstddef>
#include <span>

/**
 * Block Zero proof-of-work hashing using RandomX.
 *
 * RandomX requires a key K (the "seed") which initializes a memory-hard cache.
 * For verification we use light mode (cache only, ~256 MiB). The cache/VM for a
 * given key is created lazily and reused across calls with the same key, which
 * is essential for performance because key changes are infrequent by design.
 *
 * This function is thread-safe.
 *
 * @param key   32-byte RandomX key (seed). For the early chain this is the
 *              genesis-derived bootstrap key; later it rotates with height.
 * @param input The data to hash (the serialized 80-byte block header).
 * @return      The 32-byte RandomX output interpreted as a uint256.
 */
uint256 RandomXComputeHash(const uint256& key, std::span<const unsigned char> input);

/**
 * The fixed bootstrap RandomX key used for the early chain (before height-based
 * seed rotation activates). Deterministic and public; contains no secrets.
 */
uint256 RandomXBootstrapKey();

#endif // BITCOIN_POW_RANDOMX_H
