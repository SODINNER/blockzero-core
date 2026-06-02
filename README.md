# BLOCK ZERO

**Modern BTC code. A second chance at Genesis.**

CPU-mineable · Fair launch · Proof-of-work

No presale. No insiders. No premine.

---

## blockzero-core

The reference node for Block Zero — an independent chain built on Bitcoin Core v31, with RandomX proof-of-work so ordinary CPUs can mine from block one.

This is not Bitcoin. It is a fresh chain with its own genesis, its own rules, and a launch model that puts miners first.

### What makes it different

| | Block Zero | Typical altcoin |
|---|---|---|
| **Launch** | Fair — mine from genesis | Presale, team allocation, VC |
| **Mining** | RandomX on your CPU | ASIC farms or GPU pools |
| **Codebase** | Modern Bitcoin Core v31 | Fork-and-forget spaghetti |
| **Insiders** | None | Founders, advisors, VCs |

### Start mining (testnet)

Prebuilt binaries: **[Releases](https://github.com/Rexemre/blockzero-core/releases)** (latest: `v0.1.0-testnet.5`)

One-click setup: [blockzero-ops/scripts/testnet](https://github.com/Rexemre/blockzero-ops/tree/main/scripts/testnet)

Full docs: [blockzero-docs](https://github.com/Rexemre/blockzero-docs)

### Build from source

```bash
git clone --recurse-submodules https://github.com/Rexemre/blockzero-core.git
cd blockzero-core
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

See [doc/build-unix.md](doc/build-unix.md) and [doc/build-windows-msvc.md](doc/build-windows-msvc.md).

### Repositories

| Repo | Purpose |
|------|---------|
| **blockzero-core** (here) | Node, consensus, RandomX PoW |
| [blockzero-docs](https://github.com/Rexemre/blockzero-docs) | Guides, specs, status |
| [blockzero-ops](https://github.com/Rexemre/blockzero-ops) | Mining scripts, seed nodes |
| [blockzero-wallet](https://github.com/Rexemre/blockzero-wallet) | Wallet (in development) |

### Status

- **Testnet live** — genesis mined, public seed at `217.160.46.61:18210`, block 1+ mineable
- **Mainnet** — genesis defined, launch pending
- Upstream baseline: Bitcoin Core v31.0 ([UPSTREAM.md](UPSTREAM.md))

Ticker: **BLOZ** (mainnet) · **TBLOZ** (testnet)

### Disclaimer

Block Zero is open-source software. BLOZ/TBLOZ carry no promised value, liquidity or return. You participate at your own risk.
