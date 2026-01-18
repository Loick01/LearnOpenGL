#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    if (!glfwInit()){
        std::cerr << "Error at GLFW initialization\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", NULL, NULL);
    if (window == NULL){
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Warning : gladLoadGLLoader must be called after glfwMakeContextCurrent
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << nrAttributes << "\n";

    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    /*
    float vertices[] = { // Normalized Device Coordinates
        0.5f, 0.5f, 0.f, // First triangle
        0.5f, -0.5f, 0.f,
        -0.5f, 0.5f, 0.f,

        0.5f, -0.5f, 0.f, // Second triangle
        -0.5f, -0.5f, 0.f,
        -0.5f, 0.5f, 0.f
    };
    */

    float vertices[] = {
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f,
        -0.5f, 0.5f, 0.f
    };

    unsigned int indices[] = {
        0, 1, 2
    };

    const char* vertexShaderSource = 
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "out vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "vertexColor = vec4(0.5, 0., 0., 1.);\n"
    "}\0";

    const char* fragmentShaderSource = 
    "#version 460 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vertexColor;\n"
    "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int successVertexShader;
    int successFragmentShader;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successVertexShader);
    if (!successVertexShader){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Error in vertex shader : " << infoLog << "\n";
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successFragmentShader);
    if (!successFragmentShader){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Error in fragment shader : " << infoLog << "\n";
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    while (!glfwWindowShouldClose(window)){
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}