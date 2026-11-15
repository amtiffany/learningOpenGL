/*
render cube (no shading or normals) with camera movement capablity
*/

#include "viewer.hpp"


GLuint VAO, VBO, EBO;
GLuint shaderProgram;

GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

float rotationX = 0.0f;
float rotationY = 0.0f;

int lastMouseX = 0;
int lastMouseY = 0;

float cameraDistance = 4.0f;
glm::vec3 cameraPosition(0.0f, 0.0f, cameraDistance);
glm::vec3 cameraView(0.0f,0.0f,0.0f);

bool mousePressed = false;

std::vector<Vertex> vertices;
std::vector<int> indices;


/*
// hard-coded verts for square of side length 1, cnetered at origin
GLfloat vertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // Back face
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

     0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

    // Left face
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,

    // Right face
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,

    // Top face
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,

    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Bottom face
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f
};

*/

// shader files

// vertex shader (transforms vertices)
const char* vertexShaderSource = R"(
#version 450 core

layout (location = 0) in vec3 aPos; // position data
layout (location = 1) in vec3 aNormal; // normal data 
out vec3 FragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragNormal = mat3(model) * aNormal;
}
)";

// fragment shader (sets pixel value)
const char* fragmentShaderSource = R"(
#version 450 core

in vec3 FragNormal;
out vec4 FragColor;

void main() {
    vec3 N = normalize(FragNormal);
    vec3 L = normalize(vec3(2.0, 2.0, 1.0));
    float diff = max(dot(N, L), 0.0);
    vec3 color = vec3(0.3, 0.0, 0.1);
    FragColor = vec4(color * (0.15 + diff), 1.0);
}
)";


// compile shader files
GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cerr << "Shader compilation failed:\n"
                  << infoLog << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cerr << "Shader linking failed:\n"
                  << infoLog << std::endl;

        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}



void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Construct the model matrix (4x4 transformation matrix for our vertices)
    // inits as identity matrix
    model = glm::mat4(1.0f);

    // rotate model by current change in x, y angle
    model = glm::rotate(
        model,
        glm::radians(rotationX), // angle of rotation
        glm::vec3(1.0f, 0.0f, 0.0f) // axis of rotation (x-axis)
    );

    model = glm::rotate(
        model,
        glm::radians(rotationY),
        glm::vec3(0.0f, 1.0f, 0.0f) // y-axis
    );

    view = glm::lookAt(
        cameraPosition, // position of camera
        cameraView, // where the camera looks
        glm::vec3(0.0f, 1.0f, 0.0f)// vector corresponding to the up direction
    );

    // Activate shader program
    glUseProgram(shaderProgram);

    // Upload transformation matrices
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draw cube
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glutSwapBuffers();
}

void updateView() {
    view = glm::lookAt(cameraPosition, cameraView, glm::vec3(0.0f, 1.0f, 0.0f));
}


void keyboard(unsigned char key, int x, int y) 
{
    const float step = 0.1f;
    switch (key) {
        case 'w': cameraPosition.z -= step; cameraView.z -= step; break; // closer
        case 's': cameraPosition.z += step; cameraView.z += step; break; // farther
        case 'a': cameraPosition.x -= step; cameraView.x -= step; break;
        case 'd': cameraPosition.x += step; cameraView.x += step; break;
        default: return;
    }
    cameraDistance = cameraPosition.z;
    updateView();
    glutPostRedisplay();
}


// save current x and y position of mouse when left mouse button pressed
void mouse(int button, int state, int x, int y)
{
    if((button == 3) || (button == 4))  // wheel event
    {
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_DOWN)
        {
            
        cameraDistance += (button == 3) ? -0.25f : 0.25f;
        if (cameraDistance < 0.5f) cameraDistance = 0.5f; // distance should not be zero
        cameraPosition[2] = cameraDistance;

        updateView();
        glutPostRedisplay();

        }
    } 
    
    else if (button == GLUT_LEFT_BUTTON) // left button pressed
    {
        if (state == GLUT_DOWN)
        {
            // update global variables related to mouse state
            mousePressed = true;

            lastMouseX = x;
            lastMouseY = y;
        }

        else if (state == GLUT_UP) // button no longer pressed
        {
            mousePressed = false;
        }
    }
}


// calculate x and y displacement of mouse while mouse button pressed
void mouseMotion(int x, int y)
{
    if (mousePressed)
    {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        rotationY += deltaX * 0.5f;
        rotationX += deltaY * 0.5f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}



// refactors projection matrix when window is resized
void reshape(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, width, height);

    float aspectRatio = static_cast<float>(width) /
                        static_cast<float>(height);

    projection = glm::perspective(
        glm::radians(45.0f),
        aspectRatio,
        0.1f,
        100.0f
    );

    glutPostRedisplay();
}



int main(int argc, char* argv[])
{

    // parse object data from "tetrahedron.obj"
    std::string filename = "tetrahedron.obj";

    int num_verts = parseSingleObj(filename, vertices, indices);
    if(num_verts < 0) { 
        std::cout << "failed to retrieve info from " << filename << "\n"; 
        return 0;
    }

    // init glut
    glutInit(&argc, argv);

    glutInitContextVersion(4, 5);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("MiniGLViewer");


    // Initialize GLEW
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed." << std::endl;
        return 1;
    }


    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Define cube vertices
    // (Use the 108 GLfloat values from section 3)

    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(Vertex), 
        vertices.data(), 
        GL_STATIC_DRAW
    );

    glBufferData(GL_ARRAY_BUFFER, 
        vertices.size() * sizeof(Vertex), 
        vertices.data(), 
        GL_STATIC_DRAW
    );

    // add position attribute to VAO
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );
    glEnableVertexAttribArray(0);

    // add normal attribute to VAO
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );
    
    glEnableVertexAttribArray(0);

    // Create shader program
    shaderProgram = createShaderProgram(
        vertexShaderSource,
        fragmentShaderSource
    );

    if (shaderProgram == 0)
    {
        return 1;
    }

    // Get uniform locations
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Initialize view matrix
    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Set background color to grey
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    // Initialize projection matrix and viewport
    reshape(800, 600);

    // Start event loop
    glutMainLoop();

    return 0;
}