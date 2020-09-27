@rem author: Angrad.Yang
@rem software: File To Auto Build Keil Project
@rem file: build_keil.bat
@rem time: 2018/3/19 14:09
@rem desc:
@echo off
color 0A

@rem ����ű��ļ�����·��
cd %~dp0%
setlocal EnableDelayedExpansion
set uv=C:\Keil_v5\UV4\UV4.exe
set prj_dir=..\..\example
set uv_prj_path=
set file=
set rows=0

title Keil�Զ�����
echo.
echo ������������������������������������������������������������������������������������������������
echo ��                                        Keil�����Զ�����                                    ��
echo ������������������������������������������������������������������������������������������������
echo ��  KEIL����: C:\Keil_v5\UV4\UV4.exe                                                          ��
echo ��  ����Ŀ¼��..\..\example                                                                   ��
echo ��                                                                                            ��
echo ��device.txt��                                                                                ��       
echo �� [keil���̰���]:���Ա�����1/���Ա�����2/...                                                 ��
echo �� eg:  GM6721BLQFP144:rtc                                                                    ��
echo ��      ѡ�����GM6721BLQFP144�������Ա���..\..\example                                       ��
echo ��                                                                                            ��
echo �� ��ɺ󽫴�ӡ��������ɡ� ,�����ĵȴ�                                                       ��
echo ������������������������������������������������������������������������������������������������
echo.
echo. %date% %time%
echo.
@rem ��ʾ������
set /a line=3
set /a last=0
set logdir=
set /a index=0
set /a prj_cnt=0
@rem ɨ����Ҫ�Զ������device
for /f "tokens=1,2 delims=:" %%b in (device.txt) do (
	echo Compile deivce '%%b'
	echo.
	set logdir=%~dp0%%%b
	for /f "delims=" %%i in ('dir /ad/b %prj_dir% 2^>nul') do (set /a prj_cnt+=1)
	@rem ���ÿ��device��ɨ�蹤���ļ��У�������device��ָ���Ĺ���
	for /f "delims=" %%a in ('dir /ad/b %prj_dir%') do (
		@rem %%cΪ:֮����ַ������˴����prj_dir/xxx�Ƿ���Ҫ����
		echo %%c | findstr "%%a" > nul && set ignore=1||set ignore=0
		set status=OK
		set /a error_cnt=0
		set /a warning_cnt=0
		@rem !��ʱ����!�����������ʹ��
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
			@rem �л�����Ŀ¼
			%uv% !uv_prj_path! -n %%b
			@rem echo "***************Compile project**************"
			%uv% -j0 -r !uv_prj_path! -o !file!
			@rem ���¶��ļ��õ��ļ�����
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
			@rem �ӵ�%last%����ʾ�ļ�
			@rem
			@rem more +!last! !file!
		)
		call:display_progress !index! !prj_cnt! %%a !status! !error_cnt! !warning_cnt!
		set /a index+=1
	)
)
echo. 
echo. %date% %time%
echo "�������!"
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
		set x="��"
	) else (
		if %%i equ %current_unit% (
			set x="��"
		) else (
			set x="��"
		)
	)
	@rem �ӵ�1���ַ���ʼ����ȡ1���ַ�
	set var=!x:~1,1!
	set str=!str!!var!
)
@rem ��ʽ���ٷֱ�
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

@rem ��err_info���Error(s), ֮����ı�
@rem warn_info=!err_info:*Error^(s^), = !
