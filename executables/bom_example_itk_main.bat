set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
cd C:\Users\Administrator\Desktop\PS_upload
call C:\Apps\plm\tcroot\sample\compile -DIPLIB=none bom_example_itk_main.c
pause
call C:\Apps\plm\tcroot\sample\linkitk -o  bom_example_itk_main bom_example_itk_main.obj


rem %TC_ROOT%\sample\linkitk -o test test.obj