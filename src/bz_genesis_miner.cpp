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

#include <cstdint>
#include <cstdio>
#include <string>

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

    std::printf("[%s] mining (nBits=0x%08x, nTime=%u) ...\n", net, nBits, nTime);
    std::fflush(stdout);

    for (uint64_t nonce = 0; nonce <= 0xffffffffULL; ++nonce) {
        CBlock genesis = CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, (uint32_t)nonce, nBits, 1, 50 * COIN);
        uint256 pow = GetBlockPoWHash(genesis);
        if (UintToArith256(pow) <= *target) {
            std::printf("[%s] FOUND\n", net);
            std::printf("  nNonce        = %u\n", (uint32_t)nonce);
            std::printf("  nTime         = %u\n", nTime);
            std::printf("  nBits         = 0x%08x\n", nBits);
            std::printf("  powHash       = %s\n", pow.GetHex().c_str());
            std::printf("  hashGenesis   = %s\n", genesis.GetHash().GetHex().c_str());
            std::printf("  hashMerkleRoot= %s\n", genesis.hashMerkleRoot.GetHex().c_str());
            std::fflush(stdout);
            return;
        }
        if ((nonce & 0xffff) == 0 && nonce > 0) { std::printf("[%s] ... nonce=%llu\n", net, (unsigned long long)nonce); std::fflush(stdout); }
    }
    std::printf("[%s] no nonce found in range\n", net);
}

int main()
{
    const char* msg = "Block Zero 30/May/2026 fair launch no premine no ICO";

    // regtest: easiest target, mines almost instantly.
    MineOne("regtest", msg, 1748563200, 0x207fffff);

    return 0;
}
