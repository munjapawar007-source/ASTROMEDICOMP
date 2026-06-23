del OGL.exe
del OGL.obj

rc.exe OGL.rc
nvcc.exe -I"c:\glew-2.1.0\include" -L"C:\glew-2.1.0\lib\Release\x64" -o OGL.exe OGL.res User32.lib GDI32.lib OGL.cu 
OGL.exe
