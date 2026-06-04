# Mine the Block Zero testnet v2 genesis block (RandomX, native Windows).
#
# Usage (PowerShell — NOT WSL):
#   cd blockzero-core
#   .\scripts\genesis\mine-testnet-genesis.ps1
#   .\scripts\genesis\mine-testnet-genesis.ps1 -BinDir "C:\path\to\build\bin\Release"
#
# Requires bz-genesis-miner.exe (build from source or a release that ships it).
# Typical runtime on a modern desktop CPU: 1–5 minutes native Windows vs 30+ min in WSL2.

param(
    [string]$BinDir = "",
    [string]$LogFile = "genesis-mine.log"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent

function Find-GenesisMiner {
    if ($BinDir) {
        $p = Join-Path $BinDir "bz-genesis-miner.exe"
        if (Test-Path $p) { return $p }
    }
    $candidates = @(
        (Join-Path $RepoRoot "build\bin\Release\bz-genesis-miner.exe")
        (Join-Path $RepoRoot "build\bin\bz-genesis-miner.exe")
        (Join-Path $env:LOCALAPPDATA "BlockZero\bin\bz-genesis-miner.exe")
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { return $c }
    }
    throw @"
Cannot find bz-genesis-miner.exe.

Build natively on Windows (fast RandomX):
  git clone --recurse-submodules https://github.com/Rexemre/blockzero-core.git
  cd blockzero-core
  cmake -B build --preset vs2026-static
  cmake --build build --config Release --target bz-genesis-miner

Or download a release that includes bz-genesis-miner.exe in the zip.

Do NOT use WSL2 for genesis mining — it is 10–20x slower.
"@
}

$specPath = Join-Path $RepoRoot "artifacts\genesis\testnet-v2.json"
if (-not (Test-Path $specPath)) {
    throw "Missing $specPath"
}
$spec = Get-Content $specPath -Raw | ConvertFrom-Json

Write-Host "Block Zero testnet v2 genesis miner"
Write-Host "Message: $($spec.message)"
Write-Host "nTime:   $($spec.nTime) ($($spec.nTimeHuman))"
Write-Host ""

$miner = Find-GenesisMiner
Write-Host "Using: $miner"
Write-Host "Logging to: $LogFile"
Write-Host ""

$logPath = Join-Path (Get-Location) $LogFile
& $miner 2>&1 | Tee-Object -FilePath $logPath

Write-Host ""
Write-Host "Done. Next step:"
Write-Host "  .\scripts\genesis\apply-testnet-genesis.ps1 -LogFile $LogFile"
