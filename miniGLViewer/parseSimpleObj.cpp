/*
parse obj containing tetrahedron data
based heavily on tinyobjloader sample code
*/

#define TINYOBJLOADER_IMPLEMENTATION // tells timyobj header to include func implementations not just declarations
#include "../../tinyobjloader/tiny_obj_loader.h"

#include "viewer.hpp"

// parse a single obj from .obj file
// param inputfile: path to file to parse
// param vertices:  empty vector of type Vertex, stores the position/texture/normal of each vertex in the .obj
// param indices:   empty vector of type int, stores the indices of the p/t/n for the verts in each face 
//       format:    indices[i] -> index of position in vertices array
//                  indices[i + 1] -> index of texture in vertices array (left as -1 if no texture)
//                  indices[i + 2] -> index of normals in vertices array
// return: number of vertices
int parseSingleObj(std::string inputfile, std::vector<Vertex> &vertices, std::vector<int> &indices) {

    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    printf("initing reader.....\n");
    tinyobj::ObjReader reader;

    int num_verts = -1;
    printf("init complete\n");
    

    if (!reader.ParseFromFile(inputfile, config))
    {
        std::cerr << reader.Error() << "\n";
        return -1;
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
                
                indices.push_back(idx.vertex_index);
                indices.push_back(-1); // no tex index
                indices.push_back(idx.normal_index);
                

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

    return num_verts;


}


/*

int main() {

    std::vector<Vertex> v;
    std::vector<int> i;
    std::string filename = "tetrahedron.obj";
    int ret = parseSingleObj(filename, v, i);
    if(ret < 0) printf("failed to retrieve info from %s", filename);


    printf("\n~~~printing face indices~~~\n");
    printf("pos/tex/normal\n\n");

    if (i.size() == 0) {
        printf("no indices to display\n\n");
    }

    for (size_t j = 0; j < i.size(); j += 3) {
        printf("%d/%d/%d\n", i[j], i[j + 1], i[j + 2]);
    }

    printf("~~~printing vertices~~~\n\n");

    if (v.size() == 0) {
        printf("no vertices to display\n");
    }

    for (int j = 0; j< v.size(); j++) {
        printf("vertex %d\n position: (%f, %f, %f)\n", 
            j,
            v[j].position[0],
            v[j].position[1],
            v[j].position[2]
        );        
        
        
        printf("normals: (%f, %f, %f)\n\n", 
            v[j].normal[0],
            v[j].normal[1],
            v[j].normal[2]
        );
    }


}

*/