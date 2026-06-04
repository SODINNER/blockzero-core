# Block Zero genesis artifacts

Published, reproducible genesis parameters for each network.

| File | Network | Status |
|------|---------|--------|
| `testnet-v2.json` | Testnet v2 (2026-06-04 reset) | **Pending mine** — run `scripts/genesis/mine-testnet-genesis.ps1` |
| *(none yet)* | Mainnet | Frozen at launch — unchanged until mainnet countdown |

## Testnet v2 reset (2026-06-04)

The first public testnet (v1) had a timestamp bug: the coinbase text said **2026** but
`nTime` decoded to **2025-05-30**. v2 fixes that and replaces the plain manifesto with
a Satoshi-style headline.

**Genesis message (coinbase scriptSig):**

```text
The Times 04/Jun/2026 Block Zero - a second chance at Genesis, fair launch, no premine
```

**Fixed parameters (before mining):**

| Field | Value |
|-------|-------|
| `nTime` | `1780531200` (= 2026-06-04 00:00:00 UTC) |
| `nBits` | `0x1e3fffff` |
| `nVersion` | `1` |
| Reward | 50 TBLOZ |

**After mining**, fill `nonce`, `hashGenesisBlock`, `hashMerkleRoot`, and `powHash` in
`testnet-v2.json`, update `src/kernel/chainparams.cpp`, tag a release, and reset the VPS seed.

See [testnet-v2-reset.md](https://github.com/Rexemre/blockzero-docs/blob/main/testnet-v2-reset.md).

## Reproduce (Windows, native — recommended)

```powershell
cd blockzero-core
.\scripts\genesis\mine-testnet-genesis.ps1
.\scripts\genesis\apply-testnet-genesis.ps1 -LogFile .\genesis-mine.log
```

Mine on **bare-metal Windows**, not WSL2 — RandomX is much faster there.

## Verify

Anyone can re-run `bz-genesis-miner` and confirm the printed `hashGenesis` matches
`testnet-v2.json` once values are published.
