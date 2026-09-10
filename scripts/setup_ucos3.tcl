# FlightControl μC/OS-III BSP 配置与构建脚本。
# 本脚本由 scripts/setup_ucos3.ps1 调用，也可以在已经配置好环境的 XSCT 中执行。

if {$argc < 3} {
    puts stderr "参数不足：需要工作区、Micrium ucos 软件仓库和 SDK 路径。"
    exit 2
}

set workspace_path [file normalize [lindex $argv 0]]
set micrium_repo_path [file normalize [lindex $argv 1]]
set sdk_path [file normalize [lindex $argv 2]]
set should_build 1

if {$argc >= 4} {
    set should_build [lindex $argv 3]
}

setws $workspace_path
repo -set $micrium_repo_path

# 在全新克隆目录中注册现有 SDK 工程。工程已存在时，忽略重复导入提示。
if {[catch {importprojects $workspace_path} import_message]} {
    puts "工程导入提示：$import_message"
}

configbsp -bsp FlightControl_ucos_bsp stdout ps7_uart_0
configbsp -bsp FlightControl_ucos_bsp stdin ps7_uart_0
configbsp -bsp FlightControl_ucos_bsp OS_CFG_STAT_TASK_EN true
configbsp -bsp FlightControl_ucos_bsp OS_CFG_STAT_TASK_STK_CHK_EN true
configbsp -bsp FlightControl_ucos_bsp OS_CFG_TASK_PROFILE_EN true
configbsp -bsp FlightControl_ucos_bsp OS_CFG_SCHED_LOCK_TIME_MEAS_EN true
configbsp -bsp FlightControl_ucos_bsp OS_CFG_TS_EN true

regenbsp -bsp FlightControl_ucos_bsp

if {$should_build} {
    projects -build -type bsp -name FlightControl_ucos_bsp

    set app_build_path [file join $workspace_path FlightControl_ucos Debug]
    set app_makefile [file join $app_build_path makefile]
    set prebuild_path [file join $sdk_path bin a9-linaro-pre-build-step.bat]
    set make_path [file join $sdk_path gnuwin bin make.exe]
    set bsp_lib_path [file join $workspace_path FlightControl_ucos_bsp ps7_cortexa9_0 lib libucos.a]

    if {![file exists $bsp_lib_path]} {
        puts stderr "μC/OS-III BSP 库未生成：$bsp_lib_path"
        exit 3
    }

    # 让 SDK/CDT 刷新 Debug Makefile和递归源码清单。SDK 后台进程可能仍保留
    # 旧 PATH，因此最终应用构建显式调用随 SDK 安装的预构建步骤和 GNU Make。
    projects -build -type app -name FlightControl_ucos

    if {![file exists $app_makefile]} {
        puts stderr "应用 Makefile 未生成：$app_makefile"
        exit 4
    }

    set original_path [pwd]
    cd $app_build_path

    if {[catch {exec cmd.exe /d /c $prebuild_path 2>@1} prebuild_message]} {
        cd $original_path
        puts stderr "ARM 预构建步骤失败：$prebuild_message"
        exit 5
    }

    if {[catch {exec $make_path clean all 2>@1} build_message]} {
        cd $original_path
        puts stderr "μC/OS-III 应用构建失败：$build_message"
        exit 6
    }

    cd $original_path
    puts $build_message
}

puts "FlightControl μC/OS-III 工程配置完成。"
exit
