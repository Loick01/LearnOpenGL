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

    // -----------------------------------
    // TBN
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
    // bitangent1.x = f1 * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    // bitangent1.y = f1 * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    // bitangent1.z = f1 * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // Second triangle
    glm::vec3 edge3 = pos3-pos1;
    glm::vec3 edge4 = pos4-pos1;
    glm::vec2 deltaUV3 = uv3 - uv1;
    glm::vec2 deltaUV4 = uv4 - uv1;

    float f2 = 1./(deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    glm::vec3 tangent2, bitangent2;
    tangent2.x = f2 * (deltaUV4.y * edge3.x - deltaUV3.y * edge4.x);
    tangent2.y = f2 * (deltaUV4.y * edge3.y - deltaUV3.y * edge4.y);
    tangent2.z = f2 * (deltaUV4.y * edge3.z - deltaUV3.y * edge4.z);
    // bitangent2.x = f2 * (-deltaUV4.x * edge3.x + deltaUV3.x * edge4.x);
    // bitangent2.y = f2 * (-deltaUV4.x * edge3.y + deltaUV3.x * edge4.y);
    // bitangent2.z = f2 * (-deltaUV4.x * edge3.z + deltaUV3.x * edge4.z);

    // std::cout << "tangent1 = " << tangent1.x << ", " << tangent1.y << ", " << tangent1.z << "\n";
    // std::cout << "tangent2 = " << tangent2.x << ", " << tangent2.y << ", " << tangent2.z << "\n";
    // std::cout << "bitangent1 = " << bitangent1.x << ", " << bitangent1.y << ", " << bitangent1.z << "\n";
    // std::cout << "bitangent2 = " << bitangent2.x << ", " << bitangent2.y << ", " << bitangent2.z << "\n";
    
    // Toward +z 
    // Because the 2 triangles are aligned, they have the same tangent
    float wall_vertices[] = {
        -5.f, 5.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, tangent1.x, tangent1.y, tangent1.z,
        -5.f, -5.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, tangent1.x, tangent1.y, tangent1.z,
        5.f, -5.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, tangent1.x, tangent1.y, tangent1.z,
        5.f, 5.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, tangent1.x, tangent1.y, tangent1.z
    };

    unsigned int wall_indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    // -----------------------------------
    // LOAD THE CUBE OBJECT TEXTURES

    unsigned int wallTexture = loadTexture("../../assets/brickwall.jpg");
    unsigned int normalTexture = loadTexture("../../assets/brickwall_normal.jpg");

    // -----------------------------------
    unsigned int wallVAO, wallVBO, wallEBO;

    // wall OBJECT
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
    objectShader.SetVec3("material.specular", glm::vec3(0.5f));
    objectShader.SetFloat("material.shininess", 32.f);

    // -----------------------------------
    // LIGHT
    glm::vec3 lightPosition = glm::vec3(0.5f, 1.0f, 1.0f);
    objectShader.SetFloat("light.constant", 1.0f);
    objectShader.SetFloat("light.linear", 0.07f);
    objectShader.SetFloat("light.quadratic", 0.017f);
    objectShader.SetVec3("light.position", lightPosition);
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

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        objectShader.SetVec3("viewPos", camera.Position);

        // -----------------------------------
        // WALL OBJECT

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        glBindVertexArray(wallVAO);
        glm::mat4 wall_model = glm::mat4(1.f);
        wall_model = glm::rotate(wall_model, (float)glfwGetTime(), glm::normalize(glm::vec3(1.f, 0.f, 1.f)));
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