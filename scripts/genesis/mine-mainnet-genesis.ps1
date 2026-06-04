# Mine the Block Zero genesis blocks (RandomX). bz-genesis-miner mines both
# testnet and mainnet; this wrapper logs the output for apply-mainnet-genesis.ps1.
#
# Usage (PowerShell, from blockzero-core):
#   .\scripts\genesis\mine-mainnet-genesis.ps1
#   .\scripts\genesis\mine-mainnet-genesis.ps1 -BinDir ".\build\bin\Release"

param(
    [string]$BinDir = "",
    [string]$LogFile = "genesis-mine.log"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent

function Find-Miner {
    if ($BinDir) {
        $p = Join-Path $BinDir "bz-genesis-miner.exe"
        if (Test-Path $p) { return $p }
    }
    foreach ($c in @(
        (Join-Path $RepoRoot "build\bin\Release\bz-genesis-miner.exe")
        (Join-Path $RepoRoot "build\bin\bz-genesis-miner.exe")
        (Join-Path $env:LOCALAPPDATA "BlockZero\bin\bz-genesis-miner.exe")
    )) {
        if (Test-Path $c) { return $c }
    }
    throw "Cannot find bz-genesis-miner.exe. Build it: cmake --build build --config Release --target bz-genesis-miner"
}

$miner = Find-Miner
$logPath = Join-Path (Get-Location) $LogFile
Write-Host "Mining genesis (testnet + mainnet) with $miner"
Write-Host "Logging to: $logPath"
Write-Host ""
& $miner 2>&1 | Tee-Object -FilePath $logPath

if ((Get-Content $logPath -Raw) -notmatch '\[mainnet\] FOUND') {
    throw "Mainnet genesis was not found. Check $logPath"
}

Write-Host ""
Write-Host "Done. Next: .\scripts\genesis\apply-mainnet-genesis.ps1 -LogFile $LogFile"
