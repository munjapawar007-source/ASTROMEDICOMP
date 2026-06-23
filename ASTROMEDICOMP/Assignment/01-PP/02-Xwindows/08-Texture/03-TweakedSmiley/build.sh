# Optional: Print status
rm -f ogl.o ogl

# Compile the source file
g++ -c -o ogl.o ogl.cpp

# Link and create the executable
g++ -o ogl ogl.o -lGL -lX11 -lGLEW -lSOIL
# Run the program
./ogl
