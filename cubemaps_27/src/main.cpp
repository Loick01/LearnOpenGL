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

unsigned int loadCubemap(std::vector<std::string> faces_path){
    unsigned int cubemapTextureID;
    glGenTextures(1, & cubemapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

    int width, height, nrChannels;
    unsigned char* data;
    for (unsigned int i = 0 ; i < faces_path.size() ; i++){
        data = stbi_load(faces_path[i].c_str(), &width, &height, &nrChannels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed while loading the cubemap textures\n";
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return cubemapTextureID;
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
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f,
        0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f,
        // Front face
        -0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
        0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
        0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f,
        0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f,
        0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f,
        -0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f,
        // Top face
        -0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f,
        // Left face
        -0.5f, -0.5f, -0.5f, -1.f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, -1.f, 0.f, 0.f,
        -0.5f, 0.5f, 0.5f, -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -0.5f, -1.f, 0.f, 0.f,
        // Right face
        0.5f, -0.5f, 0.5f, 1.f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f
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

    float skybox_vertices[] = {
        // Back face
        -1.f, -1.f, -1.f,
        1.f, -1.f, -1.f,
        1.f, 1.f, -1.f,
        -1.f, 1.f, -1.f,
        // Front face
        -1.f, -1.f, 1.f,
        1.f, -1.f, 1.f,
        1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f,
        // Bottom face
        -1.f, -1.f, -1.f,
        1.f, -1.f, -1.f,
        1.f, -1.f, 1.f,
        -1.f, -1.f, 1.f,
        // Top face
        -1.f, 1.f, -1.f,
        1.f, 1.f, -1.f,
        1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f,
        // Left face
        -1.f, -1.f, -1.f,
        -1.f, -1.f, 1.f,
        -1.f, 1.f, 1.f,
        -1.f, 1.f, -1.f,
        // Right face
        1.f, -1.f, 1.f,
        1.f, -1.f, -1.f,
        1.f, 1.f, -1.f,
        1.f, 1.f, 1.f
    };

    unsigned int skybox_indices[] = {
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

    // -----------------------------------
    // unsigned int cubeTexture = loadTexture("../../assets/container.jpg");

    // CUBEMAP
    std::vector<std::string> faces_path = {
        "../../assets/skybox/right.jpg",
        "../../assets/skybox/left.jpg",
        "../../assets/skybox/top.jpg",
        "../../assets/skybox/bottom.jpg",
        "../../assets/skybox/front.jpg",
        "../../assets/skybox/back.jpg"
    };

    unsigned int cubemapTextureID = loadCubemap(faces_path);

    // -----------------------------------
    unsigned int cubeVBO, cubeVAO, cubeEBO;
    // CUBE OBJECT
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    
    // CUBEMAP
    unsigned int skyboxVBO, skyboxVAO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(skyboxVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indices), skybox_indices, GL_STATIC_DRAW);

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
    objectShader.SetInt("skyboxTexture", 0);

    Shader skyboxShader("../shaders/skybox.vs", "../shaders/skybox.fs");
    skyboxShader.SetInt("skyboxTexture", 0);
    int viewLocationSkybox = glGetUniformLocation(skyboxShader.m_id, "view");
    int projectionLocationSkybox = glGetUniformLocation(skyboxShader.m_id, "projection");

    // -----------------------------------

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window)){
        processInput(window, deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // Removing the translations in the view matrix

        // -----------------------------------
        // CUBE OBJECT
        objectShader.Use();
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection)); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        objectShader.SetVec3("cameraPos", camera.Position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID); 	

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
        // CUBEMAP
        glCullFace(GL_FRONT);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();
        // glDepthMask(GL_FALSE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID); 
        glUniformMatrix4fv(projectionLocationSkybox, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocationSkybox, 1, GL_FALSE, glm::value_ptr(skyboxView));
        glBindVertexArray(skyboxVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // glDepthMask(GL_TRUE);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}