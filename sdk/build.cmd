@ECHO OFF

setlocal ENABLEDELAYEDEXPANSION

set "withinArgs=y"

set passArgsCount=0

set "argDebug="
set "errorCode="

for %%x in (%*) do (
	set "isArg="
	set "currentArg=%%x"

	if defined withinArgs (

		rem Check if argument is a valid argument (starts with / or --)
		if /I "!currentArg:~0,1!"=="/" (
			set "isArg=y"
			set "currentArg=!currentArg:~1!"
		)^
		else if /I "!currentArg:~0,2!"=="--" (
			set "isArg=y"
			set "currentArg=!currentArg:~2!"
		)

		if defined isArg (
			if "!currentArg!"=="" (
				set "within_args="
			)^
			else if /I "!currentArg!"=="debug" (
				set "argDebug=y"
				set "passArgsArray[!passArgsCount!]=--debug"
				set /A passArgsCount+=1
			)^
			else (
				>&2 echo Unknown Argument: '%%x'
				set "errorCode=1"
			)
		)^
		else (
			set "passArgsArray[!passArgsCount!]=!currentArg!"
			set /A passArgsCount+=1
		)
	)^
	else (
		set "passArgsArray[!passArgsCount!]=!currentArg!"
		set /A passArgsCount+=1
	)
)

if defined errorCode (
	goto :error
)

rem if defined argDebug (
rem	echo debug: yes
rem )^
rem else (
rem 	echo debug: no
rem )
rem echo count: %passArgsCount%

rem for /F "tokens=%passArgsCount% delims==" %%s in ('set sources[') do echo %%s

set "passArgsConcat="
set /A passArgsCount-=1
for /L %%a in (0,1,%passArgsCount%) DO (
	if "!passArgsConcat!"=="" (
		set "passArgsConcat=^"!passArgsArray[%%a]!^""
	)^
	else (
		set "passArgsConcat=!passArgsConcat! ^"!passArgsArray[%%a]!^""
	)
)

rem echo '%passArgsConcat%'

set "rubyArgs=%~dp0rb.build\build.rb"

if defined argDebug (
	set "rubyArgs=--debug %rubyArgs%"
)^
else (
	set "rubyArgs=--enable=frozen-string-literal %rubyArgs%"
)

if "!passArgsConcat!"=="" (
	set "rubyFullArgs=%rubyArgs%"
)^
else (
	set "rubyFullArgs=%rubyArgs% -- !passArgsConcat!"
)

echo [ ruby !rubyFullArgs! ]
ruby !rubyFullArgs!

if "%ERRORLEVEL%" NEQ "0" (
	set "errorCode=%ERRORLEVEL%"
)^
else (
	echo Build Succeeded!
)

:error
	if defined errorCode (
		>&2 echo Build failed with error: !errorCode!

		set "ERRORLEVEL=!errorCode!"
	)

	cmd /C exit %ERRORLEVEL%
