// Copyright (c) 2026 The Block Zero developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Block Zero genesis miner.
//
// Reproducibly mines genesis blocks under the RandomX proof-of-work using the
// same hashing the node uses (GetBlockPoWHash / RandomXBootstrapKey). It prints
// the values needed for kernel/chainparams.cpp: nNonce, the identity hash
// (SHA256d) and the Merkle root.

#include <arith_uint256.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <pow.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <uint256.h>
#include <util/strencodings.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static void MineOne(const char* net, const char* pszTimestamp, uint32_t nTime, uint32_t nBits)
{
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;

    auto target = DeriveTarget(nBits, uint256{"7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"});
    if (!target) { std::printf("[%s] invalid nBits\n", net); return; }

    unsigned nthreads = std::thread::hardware_concurrency();
    if (nthreads == 0) nthreads = 4;

    std::printf("[%s] mining (nBits=0x%08x, nTime=%u) with %u threads ...\n", net, nBits, nTime, nthreads);
    std::fflush(stdout);

    std::atomic<bool> found{false};
    std::atomic<uint32_t> found_nonce{0};

    std::vector<std::thread> threads;
    for (unsigned t = 0; t < nthreads; ++t) {
        threads.emplace_back([&, t]() {
            const arith_uint256 tgt = *target;
            for (uint64_t nonce = t; nonce <= 0xffffffffULL; nonce += nthreads) {
                if (found.load(std::memory_order_relaxed)) return;
                CBlock g = CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, (uint32_t)nonce, nBits, 1, 50 * COIN);
                if (UintToArith256(GetBlockPoWHash(g)) <= tgt) {
                    bool expected = false;
                    if (found.compare_exchange_strong(expected, true)) {
                        found_nonce.store((uint32_t)nonce);
                    }
                    return;
                }
            }
        });
    }
    for (auto& th : threads) th.join();

    if (!found.load()) { std::printf("[%s] no nonce found in range\n", net); return; }

    CBlock genesis = CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, found_nonce.load(), nBits, 1, 50 * COIN);
    uint256 pow = GetBlockPoWHash(genesis);
    std::printf("[%s] FOUND\n", net);
    std::printf("  nNonce        = %u\n", found_nonce.load());
    std::printf("  nTime         = %u\n", nTime);
    std::printf("  nBits         = 0x%08x\n", nBits);
    std::printf("  powHash       = %s\n", pow.GetHex().c_str());
    std::printf("  hashGenesis   = %s\n", genesis.GetHash().GetHex().c_str());
    std::printf("  hashMerkleRoot= %s\n", genesis.hashMerkleRoot.GetHex().c_str());
    std::fflush(stdout);
}

int main()
{
    const char* msg = "Block Zero 30/May/2026 fair launch no premine no ICO";

    // Distinct nTime per network so each genesis block is unique.
    // (regtest genesis is already fixed at nNonce=0, nTime=1748563200.)

    // testnet and mainnet: RandomX-appropriate floor (0x1f00ffff).
    MineOne("testnet", msg, 1748563201, 0x1f00ffff);
    MineOne("mainnet", msg, 1748563200, 0x1f00ffff);

    return 0;
}
