param(
    [string]$ToolBin = "G:\Xilinx\Vitis\2021.1\gnu\aarch64\nt\aarch64-none\bin"
)

$ErrorActionPreference = "Stop"
$standaloneDir = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$moduleDir = Split-Path -Parent $standaloneDir
$bspDir = Join-Path $standaloneDir "workspace\m14_v2_platform\psu_cortexa53_0\standalone_domain\bsp\psu_cortexa53_0"
$includeDir = Join-Path $bspDir "include"
$libDir = Join-Path $bspDir "lib"
$linkerScript = Join-Path $standaloneDir "src\lscript.ld"
$buildDir = Join-Path $standaloneDir "build"
$gcc = Join-Path $ToolBin "aarch64-none-elf-gcc.exe"
$nm = Join-Path $ToolBin "aarch64-none-elf-nm.exe"
$size = Join-Path $ToolBin "aarch64-none-elf-size.exe"
$elf = Join-Path $buildDir "m14_v2_test.elf"
$map = Join-Path $buildDir "m14_v2_test.map"

foreach ($required in @($gcc, $nm, $size, $includeDir, $libDir, $linkerScript)) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Standalone build input is missing: $required"
    }
}
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$sources = @(
    (Join-Path $moduleDir "drivers\lm_head_v2_driver.c"),
    (Join-Path $moduleDir "drivers\lm_head_v2_mmio.c"),
    (Join-Path $standaloneDir "src\main.c")
)
$objects = @()
foreach ($source in $sources) {
    $object = Join-Path $buildDir (([IO.Path]::GetFileNameWithoutExtension($source)) + ".o")
    $compileArgs = @(
        "-Wall", "-Wextra", "-O2", "-g", "-DARMA53_64", "-c",
        "-I$includeDir", "-I$(Join-Path $moduleDir 'drivers')",
        "-I$(Join-Path $standaloneDir 'src')", $source, "-o", $object
    )
    & $gcc @compileArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed: $source"
    }
    $objects += $object
}

$linkArgs = @(
    "-Wl,-T", "-Wl,$linkerScript", "-L$libDir", "-Wl,-Map=$map",
    "-Wl,--cref", "-o", $elf
) + $objects + @("-Wl,--start-group,-lxil,-lgcc,-lc,--end-group")
& $gcc @linkArgs
if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $elf) -or
    (Get-Item -LiteralPath $elf).Length -eq 0) {
    throw "ELF link failed or produced an empty file: $elf"
}
if (-not (Test-Path -LiteralPath $map)) {
    throw "Linker map is missing: $map"
}

$symbols = (& $nm $elf) -join "`n"
if ($LASTEXITCODE -ne 0) {
    throw "nm failed for $elf"
}
foreach ($symbol in @("lmh_v2_submit", "lmh_v2_submit_unchecked",
                      "lmh_v2_wait", "lmh_v2_read_result",
                      "XLm_head_v2_Set_hidden_offset", "XLm_head_v2_Start")) {
    if ($symbols -notmatch "(?m)\b$([regex]::Escape($symbol))\s*$") {
        throw "ELF is missing required symbol: $symbol"
    }
}

$undefined = (& $nm -u $elf) -join "`n"
if ($LASTEXITCODE -ne 0 -or -not [string]::IsNullOrWhiteSpace($undefined)) {
    throw "ELF has undefined symbols: $undefined"
}

& $size $elf
if ($LASTEXITCODE -ne 0) {
    throw "size failed for $elf"
}
Write-Output "M14_V2_STANDALONE_ELF=$elf"
Write-Output "M14_V2_STANDALONE_MAP=$map"
Write-Output "M14_V2_STANDALONE_BUILD=PASS"
