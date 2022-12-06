@echo off
pushd _build\
      cl /FC /nologo /EHsc /Zi /Od /fp:fast ..\src\main.cpp && main.exe > test.ppm && start test.ppm
popd
