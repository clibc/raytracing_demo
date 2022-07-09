@echo off
cl /FC /nologo /EHsc /Zi /O2 main.cpp && main.exe > test.ppm && start test.ppm

