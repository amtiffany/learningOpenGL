/*
parse obj containing tetrahedron data
based heavily on tinyobjloader sample code
*/

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // tells timyobj header to include func implementations not just declarations
#include "../../tinyobjloader/tiny_obj_loader.h"

#include "viewer.hpp"

// parse a single obj from a .obj file
// return: vector of type Vertex containing the data from the .obj file
std::vector<Vertex> parseSingleObj() {

    std::string inputfile = "tetrahedron.obj";

    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    printf("initing reader.....\n");
    tinyobj::ObjReader reader;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int num_verts = -1;
    printf("init complete\n");
    

    if (!reader.ParseFromFile(inputfile, config))
    {
        std::cerr << reader.Error() << "\n";
        return vertices;
    }

    if (!reader.Warning().empty()) { std::cerr << reader.Warning() << "\n"; }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();
    printf("got info from reader\n");



    // loop over shapes (we should only have one but i left it in for the future)
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {

                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                
                printf("vertex index = %d\n", idx.vertex_index);
                printf("normal index = %d\n", idx.normal_index);
                
                vertices.push_back(Vertex(0,0,0,0,0,0));
                num_verts++;
                
                // add vertex
                vertices[num_verts].position[0] = attrib.vertices[3*size_t(idx.vertex_index)+0];
                vertices[num_verts].position[1] = attrib.vertices[3*size_t(idx.vertex_index)+1];
                vertices[num_verts].position[2] = attrib.vertices[3*size_t(idx.vertex_index)+2];
            
            

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    // add normal data
                    vertices[num_verts].normal[0] = attrib.normals[3*size_t(idx.normal_index)+0];
                    vertices[num_verts].normal[1] = attrib.normals[3*size_t(idx.normal_index)+1];
                    vertices[num_verts].normal[2] = attrib.normals[3*size_t(idx.normal_index)+2];
                }

                // ignore texcoord


                
            }

            index_offset += fv;
            // per-face material
            shapes[s].mesh.material_ids[f];
        }

        
    }

    return vertices;


}


int main() {

    std::vector<Vertex> v = parseSingleObj();

    printf("\n~~~printing vertices~~~\n\n");

    if (v.size() == 0) {
        printf("no vertices to display\n");
    }

    for (int i = 0; i< v.size(); i++) {
        printf("vertex %d position: (%f, %f, %f)\n", 
            i,
            v[i].position[0],
            v[i].position[1],
            v[i].position[2]
        );        
        
        
        printf("normals: (%f, %f, %f)\n\n", 
            v[i].normal[0],
            v[i].normal[1],
            v[i].normal[2]
        );
    }




}