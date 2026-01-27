#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <math.h>

#include "camera.hpp"
#include "shader.hpp"
#include "stb_image.h"

Camera camera;
float lastX = 400, lastY = 300; // Center of the screen
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, const float deltaTime){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE))
        camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
        camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float cube_vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f, -0.5f, -0.5f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f, 0.f, 1.f,
        -0.5f, 0.5f, -0.5f, 1.f, 1.f,
        // Front face
        -0.5f, -0.5f, 0.5f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 1.f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.f, 1.f,
        -0.5f, -0.5f, 0.5f, 0.f, 1.f,
        // Top face
        -0.5f, 0.5f, -0.5f, 0.f, 1.f,
        0.5f, 0.5f, -0.5f, 1.f, 1.f,
        0.5f, 0.5f, 0.5f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, 0.f, 0.f,
        // Left face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, 1.f, 1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 1.f,
        // Right face
        0.5f, -0.5f, 0.5f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 1.f,
        0.5f, 0.5f, 0.5f, 0.f, 1.f
    };

    unsigned int cube_indices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

    float floor_vertices[] = {
        -5.f, -0.5f, -5.f, 0.f, 1.f,
        5.f, -0.5f, -5.f, 1.f, 1.f,
        -5.f, -0.5f, 5.f, 0.f, 0.f,
        5.f, -0.5f, 5.f, 1.f, 0.f,
    };

    unsigned int floor_indices[] = {
        0, 3, 1,
        0, 2, 3
    };

    float grass_vertices[] = {
        0.f, -0.5f, 0.0f, 0.f, 1.f,
        1.f, -0.5f, 0.0f, 1.f, 1.f,
        0.f, 0.5f, 0.0f, 0.f, 0.f,
        1.f, 0.5f, 0.0f, 1.f, 0.f,
    };

    unsigned int grass_indices[] = {
        0, 3, 1,
        0, 2, 3
    };

    // -----------------------------------
    // LOAD THE TEXTURES

    unsigned int cubeTexture = loadTexture("../../assets/marble.jpg");
    unsigned int floorTexture = loadTexture("../../assets/metal.png");
    unsigned int grassTexture = loadTexture("../../assets/grass.png");
    // Because grassTexture has a fourth component (alpha), glTexImage2D (in loadTexture) will use GL_RGBA instead of GL_RGB
    unsigned int windowTexture = loadTexture("../../assets/window.png");
    // Same for windowTexture
    
    // -----------------------------------
    unsigned int cubeVBO, cubeVAO, cubeEBO, floorVAO, floorVBO, floorEBO;
    // CUBE OBJECT
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // FLOOR OBJECT
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(floorVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);
    
    // GRASS OBJECT
    unsigned int grassVBO, grassVAO, grassEBO;
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glGenBuffers(1, &grassEBO);

    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grass_vertices), grass_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(grassVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grass_indices), grass_indices, GL_STATIC_DRAW);

    // -----------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // -----------------------------------
    // OBJECT SHADER

    Shader objectShader("../shaders/object.vs", "../shaders/object.fs");
    objectShader.Use();

    int modelLocation = glGetUniformLocation(objectShader.m_id, "model");
    int viewLocation = glGetUniformLocation(objectShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(objectShader.m_id, "projection");
    objectShader.SetInt("objectTexture", 0);

    // -----------------------------------

    std::vector<glm::vec3> planePosition; // Location of grass leaves
    planePosition.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    planePosition.push_back(glm::vec3( 1.5f, 0.0f, 0.51f));
    planePosition.push_back(glm::vec3( 0.0f, 0.0f, 0.7f));
    planePosition.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    planePosition.push_back(glm::vec3( 0.5f, 0.0f, -0.6f));

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window)){
        processInput(window, deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        objectShader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // -----------------------------------
        // CUBE OBJECT
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture); 	

        glBindVertexArray(cubeVAO);
        glm::mat4 cube_model = glm::mat4(1.f);
        cube_model = glm::translate(cube_model, glm::vec3(-1.0f, 0.01f, -1.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cube_model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        cube_model = glm::mat4(1.f);
        cube_model = glm::translate(cube_model, glm::vec3(2.0f, 0.01f, 0.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cube_model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // -----------------------------------
        // FLOOR OBJECT
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glBindVertexArray(floorVAO);
        glm::mat4 floor_model = glm::mat4(1.f);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor_model));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // -----------------------------------
        // GRASS/WINDOW OBJECT
        // glBindTexture(GL_TEXTURE_2D, grassTexture);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        glBindVertexArray(grassVAO);

        // Sort the objects vefore rendering (because of blending)
        std::map<float, glm::vec3> sorted; // key=distanceToView, value=planePosition[i]
        for (unsigned int i = 0 ; i < planePosition.size() ; i++){
            const float distance = glm::length(camera.Position - planePosition[i]);
            sorted[distance] = planePosition[i]; 
        }

        glm::mat4 window_model;
        for(std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it){
            window_model = glm::mat4(1.f);
            window_model = glm::translate(window_model, it->second);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(window_model));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        
        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &grassVAO);
    glDeleteBuffers(1, &grassVBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}