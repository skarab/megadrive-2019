@echo off
set SME_PATH=%CD:\=/%
cd ..
cd sgdk
set CUR_CD=%CD:\=/%
set GDK=%CUR_CD%
set GDK_WIN=%CUR_CD%
set OLD_PATH=%PATH%
set PATH=%GDK_WIN%\bin;%PATH%
cd %SME_PATH%
make -f makefile.gen
set PATH=%OLD_PATH%
