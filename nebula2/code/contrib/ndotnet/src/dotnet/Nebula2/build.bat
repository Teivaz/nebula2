@echo off
echo Call nwrapper.exe -backend ndotnetlanguagewrapper
pause
cd ..\..\..\
..\..\..\bin\win32d\nwrapper.exe -backend ndotnetlanguagewrapper
copy ..\..\..\bin\win32d\ndotnetlib.dll src\dotnet\Nebula2\
echo Generate NebulaDotNet.dll (csc.exe must be in your path).
pause
cd src\dotnet\Nebula2\
csc /out:NebulaDotNet.dll /target:library *.cs /addmodule:ndotnetlib.dll /debug+ /debug:full /warn:4
pause