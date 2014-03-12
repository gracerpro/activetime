@echo off

del *.sdf
del /A:H *.suo
del *.depend
del *.layout
del bin\*.pdb
del bin\*.log
del bin\*.ilk
rmdir /S /Q ipch
rmdir /S /Q build\VC100\debug
rmdir /S /Q build\VC100\release
rmdir /S /Q build\codeblocks
mkdir build\codeblocks