# Upstream Baseline and Fork Policy

Block Zero Core is a fork of Bitcoin Core. This document pins the exact upstream
baseline and defines how we track upstream security and maintenance updates.

## Pinned Baseline

- Upstream: `bitcoin/bitcoin` (https://github.com/bitcoin/bitcoin)
- Release: `v31.0`
- Released: 2026-04-19
- Tag object: `84b62e1e8dbc9bbb393d0cb50c863b161b378f35`
- Tag commit (`v31.0^{}`): `6574cb40869b96b9ffc79c19dc8f4e467d60f321`

## Why 31.0

- Latest stable Bitcoin Core release at fork time.
- Includes the fix for CVE-2024-52911 (fixed in 29.0; 31.0 is well past it).
- Modern codebase with current mempool and validation improvements.

## Security Tracking

- We must track upstream point releases and security backports.
- CVE-2024-52911 root-cause cleanup was backported to the `31.x` branch
  (upstream PR #35210). We should follow `31.x` maintenance updates and
  apply relevant fixes promptly.
- Any consensus-relevant upstream change requires explicit review before merge.

## Git Remotes

- `origin` -> Block Zero Core repository (our fork).
- `upstream` -> bitcoin/bitcoin (fetch only; push is disabled).

Push to `upstream` is intentionally disabled to prevent accidental pushes to
Bitcoin Core.

## Update Workflow (planned)

1. `git fetch upstream --tags`
2. Review upstream changes in the `31.x` maintenance line.
3. Cherry-pick or merge security/maintenance fixes onto our branch.
4. Re-run network-separation and smoke tests (see docs repo).
5. Document the applied upstream changes in release notes.

## License and Attribution

- Bitcoin Core is MIT licensed.
- The upstream `COPYING` license file and required copyright notices must be
  preserved in this fork.
- Block Zero is an independent, Bitcoin-inspired network. It is not Bitcoin and
  must not be presented as Bitcoin.

## Source Import (next step)

The actual upstream source at `v31.0` is imported as a deliberate, reviewed step.
Until then, this file is the authoritative record of the pinned baseline so the
fork point is reproducible.
