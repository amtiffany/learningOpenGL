/*
Stand-alone program to open a window and generate a static 2-d triangle 
*/


#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

// shader files

// feed vertices of triangle into gl
const char* vertexShaderSource = R"(
#version 450 core

layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

// make triangle red
const char* fragmentShaderSource = R"(
#version 450 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

GLuint VAO, VBO;
GLuint shaderProgram;

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

// display triangle (no camera or object movement)
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}



int main(int argc, char* argv[]) 
{

    // init window
    glutInit(&argc, argv);

    glutInitContextVersion(4, 5);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("MiniGLViewer");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("~failed to init GLEW\n");
        return 1;
    }

    // print version info
    printf("~~~VERSION INFO~~~\n");
    printf("OpenGL: %s\n",glGetString(GL_VERSION));
    printf("GLSL: %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Renderer: %s\n",glGetString(GL_RENDERER));


    // vertices for static triangle
    GLfloat vertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };
    

    // create vertex array and vertex buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // activates VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // copies vertex data into GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // specifies layout of vertex data:
    //      vertex positions start at attribute 0
    //      vertexes have 3  components
    //      vertexes have type GL_FLOAT
    //      the distance between vertexes in memory is 3 * sizeof(GLfloat)
    //      the first component has offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);


    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        return 1;
    }


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // specifies which function to display
    glutDisplayFunc(display);

    // keeps window open
    glutMainLoop();

    return 0;
}

