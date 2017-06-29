@echo off
set TC_ROOT=C:\Apps\plm\tcroot
set _DEVROOT=%~dp0

ECHO.
ECHO.
Echo "Type POD to create POD Assembly / Type CAD to create CAD Assembly   OR"
ECHO.
ECHO "TYPE PROJ TO ADD POD/CAD ASSY to EXISTING PROJECT"
Echo "To POD/CAD Assy to Existing Project : Make sure project is already created, and project id is updated in  ADD_POD_CAD_TO_PROJ.csv"

set /p one=Enter Type(POD or CAD OR PROJ):
echo %one%
set one=%one%   
    IF %one%==POD (
       echo Creating POD assembly 
	   call %TC_ROOT%\bin\ps_upload -u=infodba -p=infodba -g=dba  -o=yes -c=Item -t=KP2_POD -i="%_DEVROOT%\Create_POD_Assy.csv"
    ) ELSE IF %one%==CAD (
       echo Creating CAD assembly
	   call %TC_ROOT%\bin\ps_upload -u=infodba -p=infodba -g=dba  -o=yes -c=Item -t=KP2_CADDesign -i="%_DEVROOT%\Create_CAD_Assy.csv"
    ) ELSE (
	echo  Adding POD/CAD to Project
        call %TC_ROOT%\bin\ps_upload -u=infodba -p=infodba -g=dba  -o=yes -c=Item -t=KP2_Project -i="%_DEVROOT%ADD_POD_CAD_TO_PROJ.csv"
    )

