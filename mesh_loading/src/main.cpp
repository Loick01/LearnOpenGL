#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

    glEnable(GL_DEPTH_TEST); // Disabled by default

    float vertices[] = {
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

    unsigned int indices[] = {
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

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 0.2f, 2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3( 0.0f, 0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(1.f)
    };

    // -----------------------------------
    // LOAD THE CUBE OBJECT TEXTURES

    int width, height, nrChannels;
    unsigned char* data = stbi_load("../../assets/container2.png", &width, &height, &nrChannels, 0);

    // Diffuse map
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture1\n";
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    // Specular map
    data = stbi_load("../../assets/container2_specular.png", &width, &height, &nrChannels, 0);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture2\n";
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    // -----------------------------------

    // CUBE OBJECT
    unsigned int VBO, cubeVAO, lightVAO, EBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // -----------------------------------

    // LIGHT CUBE
    glm::vec3 lightPos(1.2f, 1.f, 2.f);
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // WARNING : Need to set Element_Array_Buffer, even if it's the exactly same than the previous one (used for cubeVAO) 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // -----------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // -----------------------------------
    // OBJECT SHADER

    Shader objectShader("../shaders/object.vs", "../shaders/object.fs");
    objectShader.Use();

    // http://devernay.free.fr/cours/opengl/materials.html
    objectShader.SetInt("material.diffuse", 0);
    objectShader.SetInt("material.specular", 1);
    objectShader.SetVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    objectShader.SetFloat("material.shininess", 32.f);

    // Directional light
    objectShader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    objectShader.SetVec3("dirLight.ambient", glm::vec3(0.3f));
    objectShader.SetVec3("dirLight.diffuse", glm::vec3(0.9f));
    objectShader.SetVec3("dirLight.specular", glm::vec3(1.f));
    
    // Point light with attenuation
    objectShader.SetVec3("pointLights[0].ambient", glm::vec3(0.3f));
    objectShader.SetVec3("pointLights[0].diffuse", pointLightColors[0]);
    objectShader.SetVec3("pointLights[0].specular", glm::vec3(1.f));
    objectShader.SetVec3("pointLights[0].position", pointLightPositions[0]);
    objectShader.SetFloat("pointLights[0].constant", 1.0f);
    objectShader.SetFloat("pointLights[0].linear", 0.09f);
    objectShader.SetFloat("pointLights[0].quadratic", 0.032f);

    objectShader.SetVec3("pointLights[1].ambient", glm::vec3(0.3f));
    objectShader.SetVec3("pointLights[1].diffuse", pointLightColors[1]);
    objectShader.SetVec3("pointLights[1].specular", glm::vec3(1.f));
    objectShader.SetVec3("pointLights[1].position", pointLightPositions[1]);
    objectShader.SetFloat("pointLights[1].constant", 1.0f);
    objectShader.SetFloat("pointLights[1].linear", 0.09f);
    objectShader.SetFloat("pointLights[1].quadratic", 0.032f);

    objectShader.SetVec3("pointLights[2].ambient", glm::vec3(0.3f));
    objectShader.SetVec3("pointLights[2].diffuse", pointLightColors[2]);
    objectShader.SetVec3("pointLights[2].specular", glm::vec3(1.f));
    objectShader.SetVec3("pointLights[2].position", pointLightPositions[2]);
    objectShader.SetFloat("pointLights[2].constant", 1.0f);
    objectShader.SetFloat("pointLights[2].linear", 0.09f);
    objectShader.SetFloat("pointLights[2].quadratic", 0.032f);

    objectShader.SetVec3("pointLights[3].ambient", glm::vec3(0.3f));
    objectShader.SetVec3("pointLights[3].diffuse", pointLightColors[3]);
    objectShader.SetVec3("pointLights[3].specular", glm::vec3(1.f));
    objectShader.SetVec3("pointLights[3].position", pointLightPositions[3]);
    objectShader.SetFloat("pointLights[3].constant", 1.0f);
    objectShader.SetFloat("pointLights[3].linear", 0.09f);
    objectShader.SetFloat("pointLights[3].quadratic", 0.032f);

    // Spotlight
    // Setting cutOff with the cosine of the angle, don't need to compute cos-1 in shader
    objectShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    objectShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    objectShader.SetVec3("spotLight.ambient", glm::vec3(0.3f));
    objectShader.SetVec3("spotLight.diffuse", glm::vec3(0.9f));
    objectShader.SetVec3("spotLight.specular", glm::vec3(1.f));

    int modelLocation = glGetUniformLocation(objectShader.m_id, "model");
    int viewLocation = glGetUniformLocation(objectShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(objectShader.m_id, "projection");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // -----------------------------------
    // LIGHT SHADER

    Shader lightShader("../shaders/light.vs", "../shaders/light.fs");
    lightShader.Use();
    int modelLocationLight = glGetUniformLocation(lightShader.m_id, "model");
    int viewLocationLight = glGetUniformLocation(lightShader.m_id, "view");
    int projectionLocationLight = glGetUniformLocation(lightShader.m_id, "projection");

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
        // CUBE OBJECT

        objectShader.Use();
        objectShader.SetVec3("viewPos", camera.Position);
        objectShader.SetVec3("spotLight.position", camera.Position);
        objectShader.SetVec3("spotLight.direction", camera.Front);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0 ; i < 10 ; i++){
            glm::mat4 cube_model = glm::mat4(1.f);
            cube_model = glm::translate(cube_model, cubePositions[i]);
            float angle = 20.f * i;
            cube_model = glm::rotate(cube_model, glm::radians(angle), glm::vec3(1.f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cube_model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // -----------------------------------
        // LIGHT CUBE

        lightShader.Use();

        glUniformMatrix4fv(projectionLocationLight, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocationLight, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(lightVAO);
        for (unsigned int i = 0 ; i < 4 ; i++){
            glm::mat4 light_model = glm::mat4(1.f);
            light_model = glm::translate(light_model, pointLightPositions[i]);
            light_model = glm::scale(light_model, glm::vec3(0.2f));
            glUniformMatrix4fv(modelLocationLight, 1, GL_FALSE, glm::value_ptr(light_model));
            lightShader.SetVec3("lightColor", pointLightColors[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}