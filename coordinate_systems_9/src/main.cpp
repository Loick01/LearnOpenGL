#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>

#include "shader.h"
#include "stb_image.h"

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 1.f,
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
        -0.5f, 0.5f, -0.5f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 1.f,
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
    
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../../assets/container.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture\n";
    }
    
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture1
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("../../assets/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture\n";
    }
    
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture2
    stbi_image_free(data);
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs");
    shader.Use();
    shader.SetInt("texture1", 0);
    shader.SetInt("texture2", 1);

    glm::mat4 model = glm::mat4(1.f);
    // model = glm::rotate(model, glm::radians(-55.f), glm::vec3(1.f,0.f,0.f));

    glm::mat4 view = glm::mat4(1.f);
    view = glm::translate(view, glm::vec3(0.f,0.f,-3.f));

    glm::mat4 projection = glm::perspective(glm::radians(45.f), 800.f/600.f, 0.1f, 100.f);

    int modelLocation = glGetUniformLocation(shader.m_id, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    int viewLocation = glGetUniformLocation(shader.m_id, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    int projectionLocation = glGetUniformLocation(shader.m_id, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window)){
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.Use();

        model = glm::rotate(model, (float)glfwGetTime()*glm::radians(.2f), glm::vec3(0.5f,1.f,0.f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}