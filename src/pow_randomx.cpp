// Copyright (c) 2026 The Block Zero developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow_randomx.h>

#include <randomx/src/randomx.h>

#include <cstring>

namespace {

// Per-thread RandomX state. RandomX VMs are not safe to share between threads,
// so each thread keeps its own cache + VM. The cache is rebuilt only when the
// key changes, which by design happens rarely (seed rotation epochs).
struct ThreadRandomX {
    randomx_cache* cache{nullptr};
    randomx_vm* vm{nullptr};
    uint256 current_key;
    bool initialized{false};

    ~ThreadRandomX()
    {
        if (vm) randomx_destroy_vm(vm);
        if (cache) randomx_release_cache(cache);
    }

    void EnsureKey(const uint256& key)
    {
        if (initialized && current_key == key) return;

        const randomx_flags flags = randomx_get_flags();

        if (!cache) {
            cache = randomx_alloc_cache(flags);
        }
        randomx_init_cache(cache, key.data(), key.size());

        if (vm) {
            randomx_vm_set_cache(vm, cache);
        } else {
            vm = randomx_create_vm(flags, cache, nullptr);
        }

        current_key = key;
        initialized = true;
    }
};

thread_local ThreadRandomX g_thread_randomx;

} // namespace

uint256 RandomXComputeHash(const uint256& key, std::span<const unsigned char> input)
{
    g_thread_randomx.EnsureKey(key);

    unsigned char output[RANDOMX_HASH_SIZE];
    randomx_calculate_hash(g_thread_randomx.vm, input.data(), input.size(), output);

    uint256 result;
    std::memcpy(result.data(), output, result.size());
    return result;
}

uint256 RandomXBootstrapKey()
{
    // Deterministic, public bootstrap key for the early chain. No secrets.
    return uint256{"426c6f636b5a65726f2d52616e646f6d582d626f6f7473747261702d6b657976"};
}
