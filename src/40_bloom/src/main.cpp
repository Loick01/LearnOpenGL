#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
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
    glEnable(GL_CULL_FACE);

    float cube_vertices[] = {
        // Back face
        -1.f, -1.f, -1.f, 0.f, 0.f, -1.f, 1.f, 0.f,
        1.f, -1.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f,
        1.f, 1.f, -1.f, 0.f, 0.f, -1.f, 0.f, 1.f,
        -1.f, 1.f, -1.f, 0.f, 0.f, -1.f, 1.f, 1.f,
        // Front face
        -1.f, -1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
        1.f, -1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f,
        // Bottom face
        -1.f, -1.f, -1.f, 0.f, -1.f, 0.f, 0.f, 0.f,
        1.f, -1.f, -1.f, 0.f, -1.f, 0.f, 1.f, 0.f,
        1.f, -1.f, 1.f, 0.f, -1.f, 0.f, 1.f, 1.f,
        -1.f, -1.f, 1.f, 0.f, -1.f, 0.f, 0.f, 1.f,
        // Top face
        -1.f, 1.f, -1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
        1.f, 1.f, -1.f, 0.f, 1.f, 0.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f,
        -1.f, 1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f,
        // Left face
        -1.f, -1.f, -1.f, -1.f, 0.f, 0.f, 0.f, 0.f,
        -1.f, -1.f, 1.f, -1.f, 0.f, 0.f, 1.f, 0.f,
        -1.f, 1.f, 1.f, -1.f, 0.f, 0.f, 1.f, 1.f,
        -1.f, 1.f, -1.f, -1.f, 0.f, 0.f, 0.f, 1.f,
        // Right face
        1.f, -1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f,
        1.f, -1.f, -1.f, 1.f, 0.f, 0.f, 1.f, 0.f,
        1.f, 1.f, -1.f, 1.f, 0.f, 0.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f
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

    float quad_vertices[] = {
        -1.f, 1.f, 0.f, 0.f, 1.f,
        -1.f, -1.f, 0.f, 0.f, 0.f,
        1.f, 1.f, 0.f, 1.f, 1.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
    };

    unsigned int quad_indices[] = {
        0, 1, 2,
        2, 1, 3
    };

    std::vector<glm::vec3> cubePositions;
    cubePositions.push_back(glm::vec3(0.f, 1.5f, 0.f));
    cubePositions.push_back(glm::vec3(2.f, 0.f, 1.f));
    cubePositions.push_back(glm::vec3(-1.f, -1.f, 2.f));
    cubePositions.push_back(glm::vec3(0.f, 2.7f, 4.f));
    cubePositions.push_back(glm::vec3(-2.f, 1.f, -3.f));
    cubePositions.push_back(glm::vec3(-3.f, 0.f, 0.f));

    std::vector<float> cubeRotations;
    cubeRotations.push_back(0.f);
    cubeRotations.push_back(0.f);
    cubeRotations.push_back(60.f);
    cubeRotations.push_back(23.f);
    cubeRotations.push_back(124.f);
    cubeRotations.push_back(0.f);

    std::vector<float> cubeScaleFactors;
    cubeScaleFactors.push_back(0.5f);
    cubeScaleFactors.push_back(0.5f);
    cubeScaleFactors.push_back(1.f);
    cubeScaleFactors.push_back(1.25f);
    cubeScaleFactors.push_back(1.f);
    cubeScaleFactors.push_back(0.5f);

    // -----------------------------------
    // LOAD THE CUBE OBJECT TEXTURES

    unsigned int woodTexture = loadTexture("../../../assets/wood.png");
    unsigned int containerTexture = loadTexture("../../../assets/container2.png");

    // -----------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    
    unsigned int colorBuffer[2];
    glGenTextures(2, colorBuffer);
    for (unsigned int i = 0 ; i < 2 ; i++){
        glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, colorBuffer[i], 0);
    }

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    unsigned int depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int blurFBO[2]; // "Ping-pong" framebuffer (each has his own color buffer texture)
    unsigned int blurColorBuffer[2];
    glGenFramebuffers(2, blurFBO);
    glGenTextures(2, blurColorBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
        glBindTexture(GL_TEXTURE_2D, blurColorBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorBuffer[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // -----------------------------------
    // LIGHTS (POSITIONS AND COLORS)
    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
    lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
    lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
    lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
    lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
    lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));

    unsigned int cubeVBO, cubeVAO, cubeEBO, quadVBO, quadVAO, quadEBO;
    // CUBE OBJECT
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

    // QUAD OBJECT
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(quadVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
    
    // -----------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // -----------------------------------
    // OBJECT SHADER, HDR SHADER, LIGHT SHADER

    Shader objectShader("../shaders/object.vs", "../shaders/object.fs");
    Shader hdrShader("../shaders/hdr.vs", "../shaders/hdr.fs");
    Shader lightShader("../shaders/object.vs", "../shaders/light.fs");
    Shader blurShader("../shaders/blur.vs", "../shaders/blur.fs");

    objectShader.Use();
    int modelLocation = glGetUniformLocation(objectShader.m_id, "model");
    int viewLocation = glGetUniformLocation(objectShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(objectShader.m_id, "projection");
    objectShader.SetInt("material.diffuse", 0);

    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        objectShader.SetVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        objectShader.SetVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
    }
    
    hdrShader.Use();
    hdrShader.SetInt("scene", 0);
    hdrShader.SetInt("blurScene", 1);
    hdrShader.SetFloat("exposure", 0.8);

    lightShader.Use();
    int lightModelLocation = glGetUniformLocation(lightShader.m_id, "model");
    int lightViewLocation = glGetUniformLocation(lightShader.m_id, "view");
    int lightProjectionLocation = glGetUniformLocation(lightShader.m_id, "projection");

    blurShader.Use();
    blurShader.SetInt("image", 0);
    // -----------------------------------

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window)){
        processInput(window, deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // -----------------------------------
        // RENDER THE SCENE ON THE FRAMEBUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        objectShader.Use();
        objectShader.SetVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);

        glBindVertexArray(cubeVAO);
        
        // DRAW A CUBE AS THE FLOOR
        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, -1.f, 0.f));
        model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // DRAW SOME CUBES
        glBindTexture(GL_TEXTURE_2D, containerTexture);
        for (unsigned int i = 0 ; i < cubePositions.size() ; i++){
            model = glm::mat4(1.f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(cubeRotations[i]), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
            model = glm::scale(model, glm::vec3(cubeScaleFactors[i]));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // -----------------------------------
        // DRAW LIGHT SOURCE AS CUBE
        lightShader.Use();
        glUniformMatrix4fv(lightProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(lightViewLocation, 1, GL_FALSE, glm::value_ptr(view));

        for (unsigned int i = 0 ; i < lightPositions.size() ; i++){
            model = glm::mat4(1.f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.25f));
            glUniformMatrix4fv(lightModelLocation, 1, GL_FALSE, glm::value_ptr(model));
            lightShader.SetVec3("lightColor", lightColors[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // -----------------------------------
        // BLUR THE COLOR BUFFER 
        bool isHorizontal = true, first_iteration = true;
        unsigned int amount = 5;
        blurShader.Use();

        for (unsigned int i = 0; i < amount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[isHorizontal]);
            blurShader.SetInt("isHorizontal", isHorizontal);
            // First iteration -> use the color buffer where we render the brightness texture, then ping pong between blurColorBuffer[0] and [1]
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffer[1] : blurColorBuffer[!isHorizontal]);
            glBindVertexArray(quadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            isHorizontal = !isHorizontal;
            first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // -----------------------------------
        // RENDER HDR COLOR BUFFER ON A 2D QUAD (USING TONE-MAPPING)
        
        hdrShader.Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, blurColorBuffer[0]);
        glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurColorBuffer[!isHorizontal]);

        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}