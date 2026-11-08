#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>
#include <../glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION // tells timyobj header to include func implementations not just declarations
#include "../tinyobjloader/tiny_obj_loader.h"


// to convert from attrib.vertices
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    // we are not using texture so we are not including texture coords
};

std::string inputfile = ".obj";

tinyobj::ObjReaderConfig config;
config.triangulate = true;
tinyobj::ObjReader reader;

if (!reader.ParseFromFile(filename, config))
{
    std::cerr << reader.Error() << ’\n’;
    return false;
}

if (!reader.Warning().empty()) { std::cerr << reader.Warning() << ’\n’; }

const auto& attrib = reader.GetAttrib();
const auto& shapes = reader.GetShapes();
const auto& materials = reader.GetMaterials();


