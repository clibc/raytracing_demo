@echo off
pushd ..\_build\
cl /FC /nologo /EHsc /Zi /Od ..\src\main.cpp
popd
