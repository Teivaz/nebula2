@echo off
echo Compile the visual basic sample(vbc.exe must be in your path).
pause
copy ..\*.dll .
vbc /out:visualbasic.exe /target:exe hello.vb /r:NebulaDotNet.dll /debug+ /debug:full
echo Run the visual basic sample.
pause
visualbasic.exe
pause
del *.exe *.pdb *.dll