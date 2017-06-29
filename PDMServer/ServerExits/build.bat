set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
call %TC_ROOT%\sample\compile -DIPLIB=none GTAC_register_action_handler_callbacks.c
call %TC_ROOT%\sample\link_custom_exits GTAC
