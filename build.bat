@echo off
cl /FC /nologo /Zi /O2 main.cpp && main.exe > test.ppm && start test.ppm

