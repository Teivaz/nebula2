@echo off
echo Compile the csharp sample(csc.exe must be in your path).
pause
copy ..\*.dll .
csc /out:csharp.exe /target:exe hello.cs /r:NebulaDotNet.dll /debug+ /debug:full /warn:4
echo Run the csharp sample.
pause
csharp.exe
pause
del *.exe *.pdb *.dll