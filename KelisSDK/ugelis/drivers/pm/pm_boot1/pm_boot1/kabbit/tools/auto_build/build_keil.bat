@rem author: Angrad.Yang
@rem software: File To Auto Build Keil Project
@rem file: build_keil.bat
@rem time: 2018/3/19 14:09
@rem desc:
@echo off
color 0A

@rem 进入脚本文件所在路径
cd %~dp0%
setlocal EnableDelayedExpansion
set uv=C:\Keil_v5\UV4\UV4.exe
set prj_dir=..\..\example
set uv_prj_path=
set file=
set rows=0

title Keil自动编译
echo.
echo ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
echo ┃                                        Keil工程自动编译                                    ┃
echo ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
echo ┃  KEIL程序: C:\Keil_v5\UV4\UV4.exe                                                          ┃
echo ┃  工程目录：..\..\example                                                                   ┃
echo ┃                                                                                            ┃
echo ┃device.txt：                                                                                ┃       
echo ┃ [keil工程板型]:忽略编译项1/忽略编译项2/...                                                 ┃
echo ┃ eg:  GM6721BLQFP144:rtc                                                                    ┃
echo ┃      选择板型GM6721BLQFP144，并忽略编译..\..\example                                       ┃
echo ┃                                                                                            ┃
echo ┃ 完成后将打印“编译完成” ,请耐心等待                                                       ┃
echo ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
echo.
echo. %date% %time%
echo.
@rem 显示多少行
set /a line=3
set /a last=0
set logdir=
set /a index=0
set /a prj_cnt=0
@rem 扫描需要自动编译的device
for /f "tokens=1,2 delims=:" %%b in (device.txt) do (
	echo Compile deivce '%%b'
	echo.
	set logdir=%~dp0%%%b
	for /f "delims=" %%i in ('dir /ad/b %prj_dir% 2^>nul') do (set /a prj_cnt+=1)
	@rem 针对每个device，扫描工程文件夹，编译在device中指定的工程
	for /f "delims=" %%a in ('dir /ad/b %prj_dir%') do (
		@rem %%c为:之后的字符串，此处检查prj_dir/xxx是否需要编译
		echo %%c | findstr "%%a" > nul && set ignore=1||set ignore=0
		set status=OK
		set /a error_cnt=0
		set /a warning_cnt=0
		@rem !延时变量!，复合语句中使用
		if !ignore! == 1 (
			@rem echo Ignore to compile %%a on %%b
		) else (
			@rem echo Bulding %%a
			if not exist "%~dp0%%%b\" (
				md "%~dp0%%%b\"
			)
			set file=!logdir!\%%a_build_log.txt
			@rem echo !file!
			set uv_prj_path=%prj_dir%\%%a
			pushd !uv_prj_path!
			for /f "delims=" %%i in ('dir /a/b/s *.uvprojx') do (
				set uv_prj_path=%%i
			)
			popd
			@rem echo !uv_prj_path!
			@rem 切换工程目录
			%uv% !uv_prj_path! -n %%b
			@rem echo "***************Compile project**************"
			%uv% -j0 -r !uv_prj_path! -o !file!
			@rem 重新读文件得到文件行数
			for /f "tokens=1 delims=~" %%j in (!file!) do (
				set /a rows+=1
			)
			for /f "tokens=2* delims=-" %%j in (!file!) do (
				set err_info=%%j
				@rem 0 Error(s), 0 Warning(s).
				echo "%%j" | findstr "Error" > nul && set err=1||set err=0
				if !err! == 1 (
					echo "%%j" | findstr "Warning" > nul && set wrn=1||set wrn=0
					if !wrn! == 1 (
						call:get_error_cnt !err_info!
						set /a error_cnt=!return!
						call:get_warning_cnt !err_info!
						set /a warning_cnt=!return!
						if /i !warning_cnt! gtr 0 (
							set status=WARN
						)
						if /i !error_cnt! gtr 0 (
							set status=FAIL
						)
					)
				)
			)
			@rem set /a last=!rows! - !line!
			@rem 从第%last%行显示文件
			@rem
			@rem more +!last! !file!
		)
		call:display_progress !index! !prj_cnt! %%a !status! !error_cnt! !warning_cnt!
		set /a index+=1
	)
)
echo. 
echo. %date% %time%
echo "编译完成!"
echo. 
pause

:display_progress
if "%3" == "" (
	echo "error"
	goto:eof
)
set /a unit=5
set /a total_unit=100/%unit%
set /a progress=%1*100/%2
set /a current_unit=%progress%/%unit%
set str=
set x=
for /l %%i in (1,1,%total_unit%) do (
	if %%i lss %current_unit% (
		set x="■"
	) else (
		if %%i equ %current_unit% (
			set x="■"
		) else (
			set x="□"
		)
	)
	@rem 从第1个字符开始，截取1个字符
	set var=!x:~1,1!
	set str=!str!!var!
)
@rem 格式化百分比
set prog=0%progress%
set progress=%prog:~-2,2%
if %status% == OK (
	echo %str%%progress%%%   ^(%3^)    ^[%4^]
) else (
	if %status% == NT (
		echo %str%%progress%%%   ^(%3^)    ^[%4^]
	) else (
		echo %str%%progress%%%   ^(%3^)    ^[%4^]    error^(%5^),warning^(%6^)
	)
)
goto:eof

:analysis_result
set return=OK
goto:eof

:get_error_cnt
set /a return=%1%
goto :eof

:get_warning_cnt
set /a return=%3%
goto :eof

@rem 从err_info获得Error(s), 之后的文本
@rem warn_info=!err_info:*Error^(s^), = !
