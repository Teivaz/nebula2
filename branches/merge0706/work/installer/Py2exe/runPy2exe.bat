@echo off
REM -- Run py2exe
python.exe setup.py py2exe

cd dist

REM -- Rezip library.zip using better compression 
7za.exe -aoa x library.zip -olibrary\ 
del library.zip 
 
cd library\ 
7za.exe a -tzip -mx9 ..\library.zip -r 
cd.. 
rd library /s /q 

REM -- pack the executables 
REM upx.exe --best --force *.dll *.pyd

REM -- remove the w9x file as anybody who runs w9x is silly
del w9xpopen.exe

cd ..
