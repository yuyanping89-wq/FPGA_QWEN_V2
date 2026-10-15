param([string]$PythonExe = 'C:\Users\ben\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe')
$ErrorActionPreference = 'Stop'
$moduleDir = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$repoDir = Split-Path -Parent $moduleDir
$fixture = Join-Path $moduleDir ('build\capture_test_' + [guid]::NewGuid().ToString('N'))
$testModule = Join-Path $fixture 'module'
$standalone = Join-Path $testModule 'standalone'
$package = Join-Path $testModule 'reports\board_package_20261009'
$trace = Join-Path $fixture 'artifacts\module14_v2_assets\trace\step_00'
foreach ($directory in @($standalone, $package, $trace, (Join-Path $standalone 'scripts'))) {
    New-Item -ItemType Directory -Path $directory -Force | Out-Null
}
foreach ($name in @('run_step.ps1', 'prepare_mailbox.py', 'parse_mailbox.py')) {
    Copy-Item -LiteralPath (Join-Path $moduleDir "standalone\$name") -Destination $standalone
}
foreach ($name in @('hidden.bf16.bin', 'history_bitmap.u32.bin')) {
    Copy-Item -LiteralPath (Join-Path $repoDir "artifacts\module14_v2_assets\trace\step_00\$name") -Destination $trace
}
Copy-Item -LiteralPath (Join-Path $repoDir 'artifacts\module14_v2_assets\manifest.json') -Destination (Split-Path -Parent (Split-Path -Parent $trace))
Set-Content -LiteralPath (Join-Path $package 'm14_v2_test.elf') -Value 'mock ELF: never downloaded'
$mockElfHash = (Get-FileHash -LiteralPath (Join-Path $package 'm14_v2_test.elf') -Algorithm SHA256).Hash.ToLowerInvariant()
Set-Content -LiteralPath (Join-Path $package 'ARTIFACTS.sha256') -Value "$mockElfHash  m14_v2_test.elf"
Set-Content -LiteralPath (Join-Path $standalone 'scripts\run_step.tcl') -Value '# mock only'
Copy-Item -LiteralPath (Join-Path $moduleDir 'reports\board_runs\20260930_122201_step_00\mailbox_output.bin') -Destination (Join-Path $fixture 'failed_mailbox.bin')
Copy-Item -LiteralPath (Join-Path $moduleDir 'reports\numeric_20261009\legacy_golden\trace\step_00\raw_logits.fp32.bin') -Destination (Join-Path $fixture 'wrong_logits.bin')
$mock = Join-Path $fixture 'mock_xsct.ps1'
@'
param($Tcl, $Hidden, $History, $Mailbox, $Elf, $Dump, $Logits, $TimeoutSeconds)
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'failed_mailbox.bin') -Destination $Dump
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'wrong_logits.bin') -Destination $Logits
Write-Output 'M14_BOARD_RUN_STEP=PASS'
$global:LASTEXITCODE = 0
'@ | Set-Content -LiteralPath $mock -Encoding utf8
$caught = $false
try {
    & (Join-Path $standalone 'run_step.ps1') -Step step_00 -WriteLogits -Xsct $mock -PythonExe $PythonExe
} catch {
    $caught = $true
    if ($_.Exception.Message -notmatch 'raw logits hash mismatch') { throw }
    Write-Output "EXPECTED_CAPTURE_FAILURE=$($_.Exception.Message)"
}
if (-not $caught) { throw 'Failed mailbox and wrong logits unexpectedly passed' }
$resultsPath = Join-Path $testModule 'reports\board_runs\results.json'
$results = Get-Content -Raw -LiteralPath $resultsPath | ConvertFrom-Json
if ($results.records.Count -ne 1 -or $results.records[0].perf_cycles_total -ne 22486841) {
    throw 'Failed hardware counters were not preserved exactly once'
}
$decoded = @(Get-ChildItem -LiteralPath (Join-Path $testModule 'reports\board_runs') -Recurse -Filter 'mailbox_decoded.json')
if ($decoded.Count -ne 1) { throw 'Per-run decoded mailbox missing' }
Write-Output 'RESULT_CAPTURE_TEST=PASS failed_mailbox_saved=1 wrong_hash_rejected=1 counters_preserved=1'
