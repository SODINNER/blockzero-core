# Apply the [mainnet] bz-genesis-miner output to chainparams.cpp and mainnet.json.
#
# Usage:
#   .\scripts\genesis\apply-mainnet-genesis.ps1 -LogFile genesis-mine.log

param(
    [string]$LogFile = "genesis-mine.log",
    [switch]$WhatIf
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$logPath = if ([System.IO.Path]::IsPathRooted($LogFile)) { $LogFile } else { Join-Path (Get-Location) $LogFile }

if (-not (Test-Path $logPath)) {
    throw "Log not found: $logPath - run mine-mainnet-genesis.ps1 first."
}

$full = Get-Content $logPath -Raw
$idx = $full.IndexOf("[mainnet] FOUND")
if ($idx -lt 0) {
    throw "Log does not contain '[mainnet] FOUND'. Mining may still be running or failed."
}
# Only parse the mainnet portion so testnet values are not picked up.
$text = $full.Substring($idx)

function Get-Field([string]$name) {
    if ($text -match "${name}\s*=\s*(\S+)") { return $Matches[1].Trim() }
    throw "Could not parse $name from mainnet log section."
}

$nonce = Get-Field "nNonce"
$nTime = Get-Field "nTime"
$hashGenesis = Get-Field "hashGenesis"
$merkle = Get-Field "hashMerkleRoot"
$powHash = Get-Field "powHash"

$specPath = Join-Path $RepoRoot "artifacts\genesis\mainnet.json"
$spec = Get-Content $specPath -Raw | ConvertFrom-Json
if ([int]$nTime -ne [int]$spec.nTime) {
    throw "Mined nTime ($nTime) != spec nTime ($($spec.nTime)). Rebuild bz-genesis-miner from current source."
}

Write-Host "Parsed mainnet genesis from log:"
Write-Host "  nonce=$nonce"
Write-Host "  hashGenesis=$hashGenesis"
Write-Host "  merkle=$merkle"
Write-Host ""

$chainparams = Join-Path $RepoRoot "src\kernel\chainparams.cpp"
$lines = [System.Collections.Generic.List[string]](Get-Content $chainparams)
$marker = "// Block Zero mainnet genesis (RandomX proof-of-work)."
$start = $lines.IndexOf($marker)
if ($start -lt 0) { throw "Mainnet genesis marker not found in chainparams.cpp" }

$end = $start
while ($end -lt $lines.Count -and $lines[$end] -notmatch '^\s*\}\s*$') { $end++ }
if ($end -ge $lines.Count) { throw "Could not find end of mainnet genesis block" }

$scriptLine = '            const CScript bz_genesis_script = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;'
$createLine = "            genesis = CreateGenesisBlock(bz_genesis_msg, bz_genesis_script, $nTime, $nonce, 0x1e3fffff, 1, 50 * COIN);"

$newBlock = @(
    $marker
    '        {'
    "            const char* bz_genesis_msg = `"$($spec.message)`";"
    $scriptLine
    $createLine
    '        }'
)

$hashLine = $end + 1
$merkleLine = $end + 2
if ($lines[$hashLine] -notmatch 'hashGenesisBlock') { throw "Unexpected chainparams layout at hash assert" }
if ($lines[$merkleLine] -notmatch 'hashMerkleRoot') { throw "Unexpected chainparams layout at merkle assert" }

if ($WhatIf) {
    Write-Host "[WhatIf] Would update mainnet genesis in $chainparams"
    exit 0
}

$lines.RemoveRange($start, ($end - $start + 1))
for ($i = $newBlock.Count - 1; $i -ge 0; $i--) {
    $lines.Insert($start, $newBlock[$i])
}

$hashLine = $start + $newBlock.Count + 1
$merkleLine = $hashLine + 1
$lines[$hashLine] = '        assert(consensus.hashGenesisBlock == uint256{"' + $hashGenesis + '"});'
$lines[$merkleLine] = '        assert(genesis.hashMerkleRoot == uint256{"' + $merkle + '"});'

Set-Content -Path $chainparams -Value $lines -Encoding utf8

$spec.status = "mined"
$spec.nonce = [int]$nonce
$spec.hashGenesisBlock = $hashGenesis
$spec.hashMerkleRoot = $merkle
$spec.powHash = $powHash
$spec | ConvertTo-Json -Depth 6 | Set-Content -Path $specPath -Encoding utf8

Write-Host "Updated:"
Write-Host "  $chainparams"
Write-Host "  $specPath"
Write-Host ""
Write-Host "Next:"
Write-Host "  1. Commit and push blockzero-core"
Write-Host "  2. Tag the launch release"
Write-Host "  3. Set blockzero-ops/scripts/mainnet/chain-identity.ps1: OfficialGenesis = $hashGenesis"
Write-Host "  4. Stage the VPS mainnet seed - see blockzero-docs/mainnet-launch.md"
