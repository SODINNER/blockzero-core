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
 * Prepare the shared fast-mode dataset for `key`.
 *
 * Fast mode allocates a ~2 GiB dataset (shared across all mining threads) and
 * computes RandomX hashes roughly 5-10x faster than light mode. Building the
 * dataset takes a few seconds and is parallelized across `init_threads`. The
 * dataset is reused for subsequent calls with the same key (seed epoch) and is
 * only rebuilt when the key rotates.
 *
 * Verification keeps using light mode (see RandomXComputeHash) so ordinary
 * nodes never pay the 2 GiB cost; this is for miners only.
 *
 * Safe to call from multiple threads. Returns true if fast mode is ready, or
 * false if allocation failed (e.g. insufficient RAM) so the caller can fall
 * back to light-mode hashing.
 */
bool RandomXInitMiningDataset(const uint256& key, unsigned init_threads);

/**
 * A fast-mode RandomX hasher for a single mining thread.
 *
 * Construct one per thread (cheap) after RandomXInitMiningDataset() has run for
 * the same key, then call Hash() in the grind loop with no locking. If fast
 * mode is unavailable, IsFast() is false and Hash() transparently falls back to
 * light-mode hashing so mining still works.
 */
class RandomXFastHasher
{
public:
    explicit RandomXFastHasher(const uint256& key);
    ~RandomXFastHasher();

    RandomXFastHasher(const RandomXFastHasher&) = delete;
    RandomXFastHasher& operator=(const RandomXFastHasher&) = delete;

    bool IsFast() const { return m_vm != nullptr; }
    uint256 Hash(std::span<const unsigned char> input) const;

private:
    void* m_vm{nullptr};
    uint256 m_key;
};

/**
 * The fixed bootstrap RandomX key used for the early chain (before height-based
 * seed rotation activates). Deterministic and public; contains no secrets.
 */
uint256 RandomXBootstrapKey();

#endif // BITCOIN_POW_RANDOMX_H
