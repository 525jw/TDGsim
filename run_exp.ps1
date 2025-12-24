# Run TDG multiple times in a row.
param(
    [int]$Runs = 1,
    [int]$SleepSeconds = 0
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($root)) {
    $root = (Get-Location).Path
}

Push-Location $root
try {
    if (Test-Path -Path "logs") {
        Get-ChildItem -Path "logs" -File | Remove-Item -Force
    }
    if (Test-Path -Path "data\\result.csv") {
        Clear-Content -Path "data\\result.csv"
    }
    for ($i = 1; $i -le $Runs; $i++) {
        Write-Host "Run $i/$Runs"
        & .\tdg.exe
        if ($LASTEXITCODE -ne 0) {
            throw "tdg.exe failed with exit code $LASTEXITCODE"
        }
        if ($SleepSeconds -gt 0) {
            Start-Sleep -Seconds $SleepSeconds
        }
    }
} finally {
    Pop-Location
}
