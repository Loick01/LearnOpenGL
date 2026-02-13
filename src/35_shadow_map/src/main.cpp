#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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

    const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    float cube_vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f, 1.f, 0.f,
        0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f, 0.f, 1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f, 1.f, 1.f,
        // Front face
        -0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f, 0.f, 1.f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f, 1.f, 1.f,
        -0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f, 0.f, 1.f,
        // Top face
        -0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 1.f,
        0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f, 1.f, 1.f,
        0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
        // Left face
        -0.5f, -0.5f, -0.5f, -1.f, 0.f, 0.f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, -1.f, 0.f, 0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, -1.f, 0.f, 0.f, 1.f, 1.f,
        -0.5f, 0.5f, -0.5f, -1.f, 0.f, 0.f, 0.f, 1.f,
        // Right face
        0.5f, -0.5f, 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f, 1.f,
        0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f, 0.f, 1.f
    };

    unsigned int cube_indices[] = {
        1, 0, 2,
        0, 3, 2,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 14, 13,
        12, 15, 14,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

    float floor_vertices[] = {
        -25.f, -0.5f, -25.f, 0.f, 1.f, 0.f, 0.f, 25.f,
        25.f, -0.5f, -25.f, 0.f, 1.f, 0.f, 25.f, 25.f,
        -25.f, -0.5f, 25.f, 0.f, 1.f, 0.f, 0.f, 0.f,
        25.f, -0.5f, 25.f, 0.f, 1.f, 0.f, 25.f, 0.f,
    };

    unsigned int floor_indices[] = {
        0, 3, 1,
        0, 2, 3
    };

    const unsigned int nrCube = 3;
    glm::mat4 cubeTransformations[nrCube];
    // Cube 1
    glm::mat4 transfo = glm::mat4(1.f);
    transfo = glm::translate(transfo, glm::vec3(0.0f, 1.5f, 0.0));
    transfo = glm::scale(transfo, glm::vec3(0.5f));
    cubeTransformations[0] = transfo;
    
    // Cube 2
    transfo = glm::mat4(1.0f);
    transfo = glm::translate(transfo, glm::vec3(2.0f, 0.0f, 1.0));
    transfo = glm::scale(transfo, glm::vec3(0.5f));
    cubeTransformations[1] = transfo;

    // Cube 3
    transfo = glm::mat4(1.0f);
    transfo = glm::translate(transfo, glm::vec3(-1.0f, 0.0f, 2.0));
    transfo = glm::rotate(transfo, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    transfo = glm::scale(transfo, glm::vec3(0.25));
    cubeTransformations[2] = transfo;
    
    // -----------------------------------
    // LOAD TEXTURES

    unsigned int woodTexture = loadTexture("../../../assets/wood.png");
    
    // -----------------------------------

    // CUBE 
    unsigned int cubeVBO, cubeVAO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // FLOOR 
    unsigned int floorVBO, floorVAO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(floorVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);

    // -----------------------------------
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // -----------------------------------
    // LIGHT

    glm::vec3 lightPosition = glm::vec3(-2.f, 4.f, -1.f); // Directionnal light doesn't need a position, but we need it for the view matrix
    glm::vec3 lightLookAt = glm::vec3(0.f);
    glm::vec3 lightUpVector = glm::vec3(0.f, 1.f, 0.f);

    // Because a directionnal light is used, projection matrix must be orthographic
    float near_plane = 1.f, far_plane = 7.5f;
    glm::mat4 depthMap_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

    glm::mat4 depthMap_view = glm::lookAt(lightPosition, lightLookAt, lightUpVector); 
    
    glm::mat4 lightSpaceMatrix = depthMap_projection * depthMap_view;
    
    // -----------------------------------
    // SHADERS

    Shader objectShader("../shaders/object.vs", "../shaders/object.fs");
    objectShader.Use();
    
    int modelLocation = glGetUniformLocation(objectShader.m_id, "model");
    int viewLocation = glGetUniformLocation(objectShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(objectShader.m_id, "projection");
    int lightSpaceMatrixLocation = glGetUniformLocation(objectShader.m_id, "lightSpaceMatrix");

    objectShader.SetInt("material.diffuse", 0);
    objectShader.SetInt("shadowMap", 1);
    objectShader.SetVec3("material.specular", glm::vec3(0.5f));
    objectShader.SetFloat("material.shininess", 32.f);

    objectShader.SetFloat("light.constant", 0.5f);
    objectShader.SetFloat("light.linear", 0.02f);
    objectShader.SetFloat("light.quadratic", 0.01f);

    // Light
    objectShader.SetVec3("lightPos", lightPosition);
    objectShader.SetVec3("light.ambient", glm::vec3(0.3f));
    objectShader.SetVec3("light.diffuse", glm::vec3(0.9f));
    objectShader.SetVec3("light.specular", glm::vec3(1.f));

    glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    
    Shader depthMapShader("../shaders/depth_map.vs", "../shaders/depth_map.fs");
    depthMapShader.Use();
    
    int depthMap_lightSpaceMatrixLocation = glGetUniformLocation(depthMapShader.m_id, "lightSpaceMatrix");
    int depthMap_modelLocation = glGetUniformLocation(depthMapShader.m_id, "model");

    glUniformMatrix4fv(depthMap_lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    
    // -----------------------------------
    // DEPTH MAP
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    glGenTextures(1, &depthMap);

    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // Could also use GL_CLAMP_TO_EDGE instead of setting Gl_TEXTURE_BORDER_COLOR
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE); // Will not render any color data
    glReadBuffer(GL_NONE); // Will not render any color data
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // -----------------------------------
    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window)){
        processInput(window, deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // -----------------------------------
        // RENDER THE DEPTH MAP (RENDER THE SCENE FROM THE LIGHT POV)

        depthMapShader.Use();
        
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0); // ???
        glBindTexture(GL_TEXTURE_2D, woodTexture); // ???
        glCullFace(GL_FRONT);
        
        // NOW THE SCENE
        // -----------------------------------
        // FLOOR
        
        glBindVertexArray(floorVAO);
        glm::mat4 floor_model = glm::mat4(1.f);
        glUniformMatrix4fv(depthMap_modelLocation, 1, GL_FALSE, glm::value_ptr(floor_model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // -----------------------------------
        // CUBES
        
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0 ; i < nrCube ; i++){
            glm::mat4 cube_model = cubeTransformations[i];
            glUniformMatrix4fv(depthMap_modelLocation, 1, GL_FALSE, glm::value_ptr(cube_model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // -----------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // -----------------------------------
        // RENDER THE SCENE

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        objectShader.Use();
        objectShader.SetVec3("viewPos", camera.Position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glCullFace(GL_BACK);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // -----------------------------------
        // FLOOR
        
        glBindVertexArray(floorVAO);
        floor_model = glm::mat4(1.f);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(floor_model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // -----------------------------------
        // CUBES
        
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0 ; i < nrCube ; i++){
            glm::mat4 cube_model = cubeTransformations[i];
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cube_model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}