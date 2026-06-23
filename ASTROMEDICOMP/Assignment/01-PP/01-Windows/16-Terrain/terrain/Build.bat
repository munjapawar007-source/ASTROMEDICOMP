del OGL.exe
del OGL.obj

cl.exe /c /EHsc /I C:\glew-2.1.0\include OGL.cpp terrain.cpp
rc.exe OGL.rc
link.exe OGL.obj terrain.obj OGL.res /LIBPATH:C:\glew-2.1.0\lib\Release\x64 User32.lib GDI32.lib /SUBSYSTEM:WINDOWS
OGL.exe
