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

    // -----------------------------------
    // TBN matrix
    glm::vec3 pos1(-1.0, 1.0, 0.0);
    glm::vec3 pos2(-1.0, -1.0, 0.0);
    glm::vec3 pos3( 1.0, -1.0, 0.0);
    glm::vec3 pos4( 1.0, 1.0, 0.0);
    // texture coordinates
    glm::vec2 uv1(0.0, 1.0);
    glm::vec2 uv2(0.0, 0.0);
    glm::vec2 uv3(1.0, 0.0);
    glm::vec2 uv4(1.0, 1.0);
    // normal vector
    glm::vec3 nm(0.0, 0.0, 1.0);

    // First triangle
    glm::vec3 edge1 = pos2-pos1;
    glm::vec3 edge2 = pos3-pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f1 = 1./(deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    glm::vec3 tangent1, bitangent1;
    tangent1.x = f1 * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f1 * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f1 * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    // Plane triangles are aligned, can reuse tangent1
    
    // -----------------------------------

    float wall_vertices[] = {
        pos1.x, pos1.y, pos1.z, 0.f, 0.f, 1.f, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
        pos2.x, pos2.y, pos2.z, 0.f, 0.f, 1.f, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
        pos3.x, pos3.y, pos3.z, 0.f, 0.f, 1.f, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,
        pos4.x, pos4.y, pos4.z, 0.f, 0.f, 1.f, uv4.x, uv4.y, tangent1.x, tangent1.y, tangent1.z
    };

    unsigned int wall_indices[] = {
        1, 2, 0,
        0, 2, 3
    };

    // -----------------------------------
    // LOAD THE CUBE OBJECT TEXTURES

    // unsigned int wallTexture = loadTexture("../../assets/toy_box_diffuse.png");
    // unsigned int normalTexture = loadTexture("../../assets/toy_box_normal.png");
    // unsigned int heightMap = loadTexture("../../assets/toy_box_disp.png");

    unsigned int wallTexture = loadTexture("../../assets/bricks2.jpg");
    unsigned int normalTexture = loadTexture("../../assets/bricks2_normal.jpg");
    unsigned int heightMap = loadTexture("../../assets/bricks2_disp.jpg");

    // -----------------------------------
    unsigned int wallVAO, wallVBO, wallEBO;
    
    // WALL OBJECT
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glGenBuffers(1, &wallEBO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vertices), wall_vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(wallVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(8*sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wall_indices), wall_indices, GL_STATIC_DRAW);
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

    objectShader.SetInt("material.diffuse", 0);
    objectShader.SetInt("material.normalMap", 1);
    objectShader.SetInt("material.heightMap", 2);
    objectShader.SetVec3("material.specular", glm::vec3(0.2f));
    objectShader.SetFloat("material.shininess", 32.f);
    objectShader.SetFloat("heightScale", 0.1f);
    //objectShader.SetFloat("heightScale", 0.05f);

    // -----------------------------------
    // LIGHT
    glm::vec3 lightPosition = glm::vec3(0.5f, 1.0f, 0.5f);
    objectShader.SetVec3("lightPos", lightPosition);
    objectShader.SetVec3("light.ambient", glm::vec3(0.5f));
    objectShader.SetVec3("light.diffuse", glm::vec3(1.f));
    objectShader.SetVec3("light.specular", glm::vec3(1.f));
    // -----------------------------------

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window)){
        processInput(window, deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        objectShader.Use();
        objectShader.SetVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // -----------------------------------
        // WALL OBJECT
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightMap);

        glBindVertexArray(wallVAO);
        glm::mat4 wall_model = glm::mat4(1.f);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(wall_model));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &wallVAO);
    glDeleteBuffers(1, &wallVBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}