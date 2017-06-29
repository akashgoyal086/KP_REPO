set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
rem  cd C:\Users\Administrator\Desktop\PS_upload\dump

set TC_ROOT=C:\Apps\plm\tcroot
set _DEVROOT=%~dp0
cd %_DEVROOT%
rem call C:\Apps\plm\tcroot\sample\compile -DIPLIB=none CREATE_CAD.cpp
rem pause
rem call C:\Apps\plm\tcroot\sample\linkitk -o  CREATE_CAD CREATE_CAD.obj


 call C:\Apps\plm\tcroot\sample\compile -DIPLIB=none KP_EMAIL_NOTIFY_Creators.cpp
 pause
 call C:\Apps\plm\tcroot\sample\linkitk -o  KP_EMAIL_NOTIFY_Creators KP_EMAIL_NOTIFY_Creators.obj






rem %TC_ROOT%\sample\linkitk -o test test.obj