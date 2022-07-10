@echo off
pushd _build\
cl /FC /nologo /EHsc /Zi /O2 ..\src\main.cpp && main.exe > test.ppm && start test.ppm
popd
