@echo off
echo Compile the jsharp sample(vjc.exe must be in your path).
pause
copy ..\*.dll .
vjc /out:jsharp.exe /target:exe hello.jsl /r:NebulaDotNet.dll /debug+ /debug:full /warn:4
echo Run the jsharp sample.
pause
jsharp.exe
pause
del *.exe *.pdb *.dll