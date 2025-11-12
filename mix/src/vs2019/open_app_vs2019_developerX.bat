@echo off

REM define environment variables, do not change the variable name, just it's value!
REM set GTEST_INC=C:\pkg\googletest\vs2019-install\include
REM  set GTEST_LIB=C:\pkg\googletest\vs2019-install\lib

REM set BOOST_INC=C:\pkg\boost\boost_1_71_0
REM set BOOST_LIB=C:\pkg\boost\boost_1_71_0\lib64-msvc-14.2


set APP_SRC=C:\Users\wus1\Projects\swu-personal\mix\src
set PROJ_BUILD=C:\Users\wus1\Projects\swu-personal\mix\build-vs2019-x64\int
set PROJ_EXPORT=C:\Users\wus1\Projects\swu-personal\mix\build-vs2019-x64\bin

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe" %APP_SRC%\vs2019\app_vs2019.sln

REM ---eof---
