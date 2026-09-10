[CmdletBinding()]
param(
    [string]$SdkRoot = 'D:\Xilinx\SDK\2019.1',

    [Parameter(Mandatory = $true)]
    [string]$MicriumRepository,

    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'

$workspacePath = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$sdkPath = (Resolve-Path -LiteralPath $SdkRoot).Path
$micriumPath = (Resolve-Path -LiteralPath $MicriumRepository).Path
$xsctPath = Join-Path $sdkPath 'bin\xsct.bat'
$tclPath = Join-Path $PSScriptRoot 'setup_ucos3.tcl'
$safetyCheckPath = Join-Path $PSScriptRoot 'check_ucos3_safety.ps1'
$javaHomePath = Join-Path $workspacePath '.metadata\xilinx-user-home'

if (-not (Test-Path -LiteralPath $xsctPath -PathType Leaf)) {
    throw "没有找到 XSCT：$xsctPath"
}

if (-not (Test-Path -LiteralPath (Join-Path $micriumPath 'sw_apps\helloworld_osiii'))) {
    throw 'MicriumRepository 必须指向包含 sw_apps、components 和 drivers 的 ucos 子目录。'
}

& $safetyCheckPath
if ($LASTEXITCODE -ne 0) {
    throw 'μC/OS-III 源码安全检查失败。'
}

New-Item -ItemType Directory -Path $javaHomePath -Force | Out-Null

# 避免 SDK 2019.1 在部分 Windows 环境中错误地把 Eclipse 配置写到 C:\.Xilinx。
$javaHomeForJava = $javaHomePath.Replace('\', '/')
$env:JAVA_TOOL_OPTIONS = "-Duser.home=$javaHomeForJava"

# Eclipse 应用构建器读取启动时的环境，所以必须在启动 XSCT 前补齐工具链路径。
$toolPaths = @(
    (Join-Path $sdkPath 'bin'),
    (Join-Path $sdkPath 'gnuwin\bin'),
    (Join-Path $sdkPath 'gnu\aarch32\nt\gcc-arm-none-eabi\bin')
)
$env:Path = ($toolPaths -join ';') + ';' + $env:Path

$buildFlag = if ($SkipBuild) { '0' } else { '1' }
$buildStartedAt = Get-Date
$xsctOutput = @(& $xsctPath $tclPath $workspacePath $micriumPath $sdkPath $buildFlag 2>&1)
$xsctExitCode = $LASTEXITCODE
$xsctOutput | ForEach-Object { Write-Host $_ }

if (($xsctExitCode -ne 0) -or ($xsctOutput -match '^ERROR:')) {
    throw "μC/OS-III 工程配置失败，XSCT 退出码：$xsctExitCode"
}

if (-not $SkipBuild) {
    $elfPath = Join-Path $workspacePath 'FlightControl_ucos\Debug\FlightControl_ucos.elf'
    if (-not (Test-Path -LiteralPath $elfPath -PathType Leaf)) {
        throw "μC/OS-III 应用 ELF 未生成：$elfPath"
    }

    if ((Get-Item -LiteralPath $elfPath).LastWriteTime -lt $buildStartedAt) {
        throw "μC/OS-III 应用 ELF 不是本次构建生成：$elfPath"
    }
}
