@ECHO OFF
IF "%~1" == "" (
	>&2 ECHO PLEASE PROVIDE DRIVE LETTER AS ARGUMENT
	EXIT /B
)
IF NOT "%~2" == "" (
	>&2 ECHO ONE ARGUMENT ONLY PLEASE
	EXIT /B
)

SET drive=%~1:

IF EXIST %drive% (
	ECHO %drive% FORMATING
	format %drive% /q /y /fs:FAT /v:GAMEBUINO >NUL 2>NUL
	IF ERRORLEVEL 1 (
		>&2 ECHO %drive% ERROR FORMATING
		PAUSE
		EXIT /B
	)
	ECHO %drive% COPYING FILES
	xcopy %cd%\Binaries\*.* %drive%\ /s /e
	IF ERRORLEVEL 1 (
		>&2 ECHO %drive% ERROR COPYING
		PAUSE
		EXIT /B
	)
	ECHO %drive% FINISHED
	EXIT
) ELSE (
	>&2 ECHO %drive% DOES NOT EXIST
	EXIT /B
)