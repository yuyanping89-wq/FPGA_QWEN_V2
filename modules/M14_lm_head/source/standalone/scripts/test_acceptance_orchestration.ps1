param([string]$PythonExe = 'C:\Users\ben\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe')
$ErrorActionPreference = 'Stop'
$moduleDir = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$repoDir = Split-Path -Parent $moduleDir
$fixture = Join-Path $moduleDir ('build\orchestration_test_' + [guid]::NewGuid().ToString('N'))
$testModule = Join-Path $fixture 'module'
$standalone = Join-Path $testModule 'standalone'
New-Item -ItemType Directory -Path $standalone -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $moduleDir 'standalone\run_acceptance.ps1') -Destination $standalone
$configPath = Join-Path $standalone 'mock_config.json'
$config = @{ asset_root = (Join-Path $repoDir 'artifacts\module14_v2_assets'); fail_step = '' }
$config | ConvertTo-Json | Set-Content -LiteralPath $configPath -Encoding utf8
$mockBody = @'
param($Step='step_00', $RepeatCount=1, $Xsct, $PythonExe, $PackageDir, $TimeoutSeconds, [switch]$WriteLogits, $Test)
$ErrorActionPreference = 'Stop'
$config = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'mock_config.json') | ConvertFrom-Json
$kind = $MyInvocation.MyCommand.Name
if ($config.fail_step -and $Step -eq $config.fail_step -and $kind -eq 'run_step.ps1') {
    throw 'EXPECTED_MOCK_STOP'
}
$manifest = Get-Content -Raw -LiteralPath (Join-Path $config.asset_root 'manifest.json') | ConvertFrom-Json
$golden = $manifest.trace | Where-Object { $_.step -eq $Step }
$run = Join-Path (Split-Path -Parent $PSScriptRoot) ('mock_runs\' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $run -Force | Out-Null
$isNegative = $kind -eq 'run_negative.ps1'
$isFailure = $kind -eq 'run_failure_regression.ps1'
$status = if ($isNegative) { @{bad_abi=1;bad_flags=11;bad_dimension=2;numeric_error=9}[$Test] } else {0}
$writes = if ($WriteLogits) {37984} else {0}
$record = @{step=$(if($isNegative){$Test}else{$Step});source_run=$run;
    state=$(if($isFailure){'0x4641494c'}else{'0x444f4e45'});
    driver_status=$(if($isFailure){7}else{0});status_code=$status;expected_status=$status;
    job_id=1;repeat_count=$RepeatCount;result_job_id=$RepeatCount;build_config=0x10008011;capabilities=0x182;
    selected_token=$golden.selected_token;selected_score_bits=$golden.selected_score_bits;output_crc=$golden.output_crc;
    processed_rows=$(if($isNegative){0}else{1});vocab_rows_done=$(if($isNegative){0}else{151936});
    weight_read_beats_128=$(if($isNegative){0}else{17016832});dot_active_cycles=17016832;
    history_read_words=4748;perf_read_beats=17018131;run_mode=1;enable_penalty=1;
    perf_write_beats=$writes;logits_write_beats_128=$writes;logits_written=[int][bool]$WriteLogits;
    write_logits=[int][bool]$WriteLogits;perf_cycles_total=22486802;wall_ticks=7495515L*$RepeatCount;
    timer_counts_per_second=33333000}
@{records=@($record)} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $run 'mailbox_decoded.json') -Encoding utf8
if ($WriteLogits) {
    Copy-Item -LiteralPath (Join-Path $config.asset_root "trace\$Step\raw_logits.fp32.bin") -Destination $run
}
if ($isNegative) { Write-Output "M14_BOARD_NEGATIVE_DIR=$run" }
elseif ($isFailure) { Write-Output "M14_FAILURE_REGRESSION_DIR=$run" }
else { Write-Output "M14_BOARD_STEP_DIR=$run" }
'@
foreach ($name in @('run_step.ps1','run_negative.ps1','run_failure_regression.ps1')) {
    $mockBody | Set-Content -LiteralPath (Join-Path $standalone $name) -Encoding utf8
}
$output = @(& (Join-Path $standalone 'run_acceptance.ps1') -PythonExe $PythonExe -Xsct 'MOCK_NO_HARDWARE')
$output | Set-Content -LiteralPath (Join-Path $fixture 'full_campaign_mock.log') -Encoding utf8
$campaigns = @(Get-ChildItem -LiteralPath (Join-Path $testModule 'reports\board_runs') -Recurse -Filter campaign.json)
if ($campaigns.Count -ne 1) { throw 'Mock campaign count mismatch' }
$complete = Get-Content -Raw -LiteralPath $campaigns[0].FullName | ConvertFrom-Json
if ($complete.state -ne 'PASS' -or $complete.records.Count -ne 55) { throw 'Mock orchestration did not complete all 55 runs' }
& $PythonExe (Join-Path $moduleDir 'scripts\summarize_acceptance.py') $campaigns[0].FullName `
    --manifest (Join-Path $repoDir 'artifacts\module14_v2_assets\manifest.json') `
    --output (Join-Path $fixture 'mock_summary.json') | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'Mock orchestration summary failed' }
$config.fail_step = 'step_01'
$config | ConvertTo-Json | Set-Content -LiteralPath $configPath -Encoding utf8
$caught = $false
try {
    & (Join-Path $standalone 'run_acceptance.ps1') -PythonExe $PythonExe -Xsct 'MOCK_NO_HARDWARE' | Out-Null
} catch {
    if ($_.Exception.Message -notmatch 'EXPECTED_MOCK_STOP') { throw }
    $caught = $true
}
if (-not $caught) { throw 'Mock injected failure did not stop campaign' }
$campaigns = @(Get-ChildItem -LiteralPath (Join-Path $testModule 'reports\board_runs') -Recurse -Filter campaign.json)
$failed = @($campaigns | ForEach-Object { Get-Content -Raw -LiteralPath $_.FullName | ConvertFrom-Json } | Where-Object { $_.state -eq 'FAIL' })
if ($failed.Count -ne 1 -or $failed[0].records.Count -ne 3) { throw 'Campaign did not checkpoint the three completed runs before failure' }
Write-Output "ACCEPTANCE_ORCHESTRATION_MOCK=PASS completed_runs=55 stop_on_failure=1 hardware_access=0 fixture=$fixture"
