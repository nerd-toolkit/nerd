@echo off

set ENV=environment.xml
set FIT=fitness.js
set NET=network.onn
set VALS=values.val
set EVO_PARAMS=evoParameters.val
set ORCS=..\OrcsEvaluation.exe

pushd "%CD%"
cd /d "%~dp0"
%ORCS% -env "%ENV%" -net Alice "%NET%" -net BadGuy1 badGuyNet.onn -net BadGuy2 badGuyNet.onn -net Friend1 friendNet.onn -net Friend2 friendNet.onn -fit Alice Script Script -fsf Script "%FIT%" -gui -val "%VALS%" -val "%EVO_PARAMS%" -val selectionParameters.val -val essentials.val %*

rem Undo changes to the shell environment.
popd
set ENV=
set FIT=
set NET=
set VALS=
set ORCS=

