#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "camera.hpp"
#include "mesh.hpp"
#include "model.hpp"
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

int main()
{
    if (!glfwInit()){
        std::cerr << "Error at GLFW initialization\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Mesh loader", monitor, NULL);
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

    glViewport(0, 0, mode->width, mode->height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    stbi_set_flip_vertically_on_load(true);

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

    // -----------------------------------

    unsigned int cubeVBO, cubeVAO, cubeEBO, quadVBO, quadVAO, quadEBO;
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // -----------------------------------
    
    Model backpack_model("../../../assets/backpack/backpack.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3( 0.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3( 3.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3( 0.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3( 3.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
    objectPositions.push_back(glm::vec3( 0.0, -0.5, 3.0));
    objectPositions.push_back(glm::vec3( 3.0, -0.5, 3.0));
    
    // -----------------------------------
    // OBJECT SHADER

    Shader objectShader("../shaders/object.vs", "../shaders/object.fs");
    objectShader.Use();

    int modelLocation = glGetUniformLocation(objectShader.m_id, "model");
    int viewLocation = glGetUniformLocation(objectShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(objectShader.m_id, "projection");

    // -----------------------------------
    // LIGHT CUBE SHADER

    Shader lightCubeShader("../shaders/lightCube.vs", "../shaders/lightCube.fs");
    lightCubeShader.Use();
    int modelLocationLightCube = glGetUniformLocation(lightCubeShader.m_id, "model");
    int viewLocationLightCube = glGetUniformLocation(lightCubeShader.m_id, "view");
    int projectionLocationLightCube = glGetUniformLocation(lightCubeShader.m_id, "projection");

    // -----------------------------------
    // LIGHTING SHADER

    Shader lightingShader("../shaders/lighting.vs", "../shaders/lighting.fs");
    lightingShader.Use();
    lightingShader.SetInt("gTextures.position", 0);
    lightingShader.SetInt("gTextures.normal", 1);
    lightingShader.SetInt("gTextures.colorSpec", 2);

    // LIGHT CUBES
    const unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(42);
    for (unsigned int i = 0 ; i < NR_LIGHTS ; i++) {
        // Random position
        float x = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float y = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
        float z = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        lightPositions.push_back(glm::vec3(x, y, z));
        
        // Random color
        float r = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // [0.5, 1.] 
        float g = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // [0.5, 1.] 
        float b = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // [0.5, 1.] 
        lightColors.push_back(glm::vec3(r, g, b));
    }

    const float constant = 1.f;
    const float linear = 0.7;
    const float quadratic = 1.8f;

    for (unsigned int i = 0 ; i < NR_LIGHTS ; i++) {
        const float lightMax = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
        const float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (lightMax * 256./5.)))) / 2*quadratic;
        lightingShader.SetVec3("lights[" + std::to_string(i) + "].position", lightPositions[i]);
        lightingShader.SetVec3("lights[" + std::to_string(i) + "].color", lightColors[i]);
        lightingShader.SetFloat("lights[" + std::to_string(i) + "].linear", linear);
        lightingShader.SetFloat("lights[" + std::to_string(i) + "].quadratic", quadratic);
        lightingShader.SetFloat("lights[" + std::to_string(i) + "].radius", radius);
    }

    // -----------------------------------
    // G-BUFFER

    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Position, normal and color (albedo+specular) will be stored in the G-Buffer
    unsigned int gPosition, gNormal, gColorSpec;

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mode->width, mode->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mode->width, mode->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // Color (3 floats) and specular intensity (1 float) are combine in a single RGBA texture
    glGenTextures(1, &gColorSpec);
    glBindTexture(GL_TEXTURE_2D, gColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mode->width, mode->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments); // Specify which color buffers are to be drawn into

    unsigned int depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mode->width, mode->height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!\n";
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // -----------------------------------

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window)){
        processInput(window, deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // DRAW OBJECTS INTO TEXTURES FROM G-BUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        objectShader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mode->width/(float)mode->height, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // -----------------------------------
        // OBJECT
        
        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int i = 0 ; i < objectPositions.size() ; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
            backpack_model.Draw(objectShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gColorSpec);
        lightingShader.SetVec3("viewPos", camera.Position);

        glDisable(GL_DEPTH_TEST); // Depth test must be disabled when rendering the quad, otherwise light cube will be rendered behind
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        // -----------------------------------
        // Copy the content of the depth buffer from gBuffer into the depth buffer of the default framebuffer
        // Light cubes will be only rendered when on top of the previously rendered geometry 
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, mode->width, mode->height, 0, 0, mode->width, mode->height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // -----------------------------------
        // RENDER LIGHT CUBES

        lightCubeShader.Use();

        glUniformMatrix4fv(projectionLocationLightCube, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocationLightCube, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0 ; i < NR_LIGHTS ; i++){
            glm::mat4 lightCubeModel = glm::mat4(1.f);
            lightCubeModel = glm::translate(lightCubeModel, lightPositions[i]);
            lightCubeModel = glm::scale(lightCubeModel, glm::vec3(0.2f));
            glUniformMatrix4fv(modelLocationLightCube, 1, GL_FALSE, glm::value_ptr(lightCubeModel));
            lightCubeShader.SetVec3("lightColor", lightColors[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}