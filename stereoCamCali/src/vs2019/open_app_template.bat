@echo off

REM define environment variables, do not change the variable name, just it's value!
set GTEST_INC=C:\pkg\googletest\vs2019-install\include
set GTEST_LIB=C:\pkg\googletest\vs2019-install\lib

set CV_INC=C:\pkg\opencv\opencv-4.1.1\install-local-w-contrib-vc2019-x64\include
set CV_LIB=C:\pkg\opencv\opencv-4.1.1\install-local-w-contrib-vc2019-x64\x64\vc16\lib

set BOOST_INC=C:\pkg\boost\boost_1_71_0
set BOOST_LIB=C:\pkg\boost\boost_1_71_0\lib64-msvc-14.2

set APP_SRC=C:\Users\wus1\Projects\swu-personal\stereoCamCali\src
set APP_BLD=C:\Users\wus1\Projects\swu-personal\stereoCamCali\build-vs2019-x64\int
set APP_BIN=C:\Users\wus1\Projects\swu-personal\stereoCamCali\build-vs2019-x64\bin

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe" %APP_SRC%\vs2019\app.sln

REM ---eof---
