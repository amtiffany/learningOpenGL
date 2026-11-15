# learningOpenGL
### parseSimpleObj.cpp
#### program that uses tinyobjloader to parse a mesh from a .obj file. heavily based on  tinyobjloader's sample code

### tetrahedron.obj
#### sample .obj file containing a tetrahedron

### main.cpp
#### program that draws the tetrahedron from tetrahedron.obj using parseSimpleObj.cpp to parse. controls:
- wasd: change position of camera and where it looks (x/y)
- mousewheel: change the distance of camera from the eye
- left mouse button: rotate the object (x/y/z)

### viewer
#### sample executable from main.cpp