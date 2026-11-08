/*
parse obj containing tetrahedron data
*/

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // tells timyobj header to include func implementations not just declarations
#include "../tinyobjloader/tiny_obj_loader.h"

#include "viewer.hpp"


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


