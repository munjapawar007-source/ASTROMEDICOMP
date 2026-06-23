# Optional: Print status
rm -f ogl.o ogl

# Compile the source file
g++ -c -o ogl.o ogl.cpp 

# Link and create the executable
g++ -o ogl ogl.o -lGL -lX11 -lGLEW -L. -lSphere

# Run the program with runtime library path
LD_LIBRARY_PATH=. ./ogl
