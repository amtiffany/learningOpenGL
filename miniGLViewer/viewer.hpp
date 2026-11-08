#include <GL/glew.h>
#include <GL/freeglut.h>

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>
#include <../glm/gtc/type_ptr.hpp>

// for use with tiny_obj_loader
// to convert from attrib.vertices
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    // we are not using texture so we are not including texture coords
};


