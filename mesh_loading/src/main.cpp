#include <glad/glad.h> // Must be include before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>

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

    glViewport(0, 0, 800, 600);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST); // Disabled by default

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

    // -----------------------------------
    // LIGHTS

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.f, 0.f, 3.f),
        glm::vec3(-1.f, 0.f, -2.f),
        glm::vec3(1.f, 0.f, -2.f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 0.f, 1.f)
    };

    unsigned int VBO, lightVAO, EBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // -----------------------------------
    // OBJECT SHADER

    Shader objectShader("../shaders/object.vs", "../shaders/object.fs");
    objectShader.Use();

    int modelLocation = glGetUniformLocation(objectShader.m_id, "model");
    int viewLocation = glGetUniformLocation(objectShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(objectShader.m_id, "projection");

    objectShader.SetFloat("material.shininess", 32.f);

    // Point light with attenuation
    objectShader.SetVec3("pointLights[0].ambient", glm::vec3(0.1f));
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

    // Spotlight
    // Setting cutOff with the cosine of the angle, don't need to compute cos-1 in shader
    objectShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    objectShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    objectShader.SetVec3("spotLight.ambient", glm::vec3(0.3f));
    objectShader.SetVec3("spotLight.diffuse", glm::vec3(0.9f));
    objectShader.SetVec3("spotLight.specular", glm::vec3(1.f));

    // -----------------------------------
    // LIGHT SHADER

    Shader lightShader("../shaders/light.vs", "../shaders/light.fs");
    lightShader.Use();
    int modelLocationLight = glGetUniformLocation(lightShader.m_id, "model");
    int viewLocationLight = glGetUniformLocation(lightShader.m_id, "view");
    int projectionLocationLight = glGetUniformLocation(lightShader.m_id, "projection");

    // -----------------------------------

    Model backpack_model("../../assets/backpack/backpack.obj");
    
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
        objectShader.SetVec3("spotLight.position", camera.Position);
        objectShader.SetVec3("spotLight.direction", camera.Front);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.f/600.f, 0.1f, 100.f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position+camera.Front, camera.Up); 
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        // -----------------------------------
        // OBJECT
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        backpack_model.Draw(objectShader);

        // -----------------------------------
        // LIGHT CUBES

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
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}