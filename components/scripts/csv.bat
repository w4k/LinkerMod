:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: This script should automatically call any child scripts to setup all assets needed for the tools  ::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off

set GAME_DIR=..\..
set ASSET_UTIL=%GAME_DIR%\bin\asset_util.exe

set ASSET_UTIL_FLAGS=

set OVERWRITE=1
if [%OVERWRITE%] == [1] (set ASSET_UTIL_FLAGS=-o)

"%ASSET_UTIL%" csvgen %ASSET_UTIL_FLAGS% --aitype --character --xmodelalias *

set OVERWRITE=
