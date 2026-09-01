[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

$workspacePath = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$sourcePath = Join-Path $workspacePath 'FlightControl_ucos\src'

$forbiddenCalls = @(
    'XGpioPs_WritePin\s*\(',
    'XGpio_DiscreteWrite\s*\(',
    'XQspiPs_PolledTransfer\s*\(',
    'SetPD_(Enable|Status)\s*\(',
    'SetSX_(Enable|Status)\s*\(',
    'Servo_init\s*\(',
    'wp40_Init\s*\(',
    'XScuWdt_Start\s*\(',
    'XScuTimer_Start\s*\('
)

$violations = @()
$sourceFiles = Get-ChildItem -LiteralPath $sourcePath -Recurse -File -Filter '*.c'

foreach ($sourceFile in $sourceFiles) {
    foreach ($pattern in $forbiddenCalls) {
        $matches = Select-String -LiteralPath $sourceFile.FullName -Pattern $pattern
        foreach ($match in $matches) {
            $violations += '{0}:{1}: {2}' -f $sourceFile.FullName, $match.LineNumber, $match.Line.Trim()
        }
    }
}

if ($violations.Count -gt 0) {
    Write-Error ("μC/OS-III 安全检查失败：发现未经批准的危险硬件调用。`n" +
                 ($violations -join "`n"))
    exit 1
}

Write-Output 'μC/OS-III 安全检查通过：未发现危险输出、Flash 写入或硬件看门狗启动调用。'
exit 0
