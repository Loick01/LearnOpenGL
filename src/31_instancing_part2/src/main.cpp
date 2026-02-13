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

    stbi_set_flip_vertically_on_load(true);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // -----------------------------------
    // SHADERS

    Shader rockShader("../shaders/rock.vs", "../shaders/object.fs");
    Shader planetShader("../shaders/planet.vs", "../shaders/object.fs");
    
    int modelLocation = glGetUniformLocation(rockShader.m_id, "model");
    int viewLocation = glGetUniformLocation(rockShader.m_id, "view");
    int projectionLocation = glGetUniformLocation(rockShader.m_id, "projection");

    int modelLocationPlanet = glGetUniformLocation(planetShader.m_id, "model");
    int viewLocationPlanet = glGetUniformLocation(planetShader.m_id, "view");
    int projectionLocationPlanet = glGetUniformLocation(planetShader.m_id, "projection");;

    unsigned int rock_amount = 100000;
    glm::mat4 *rockModelMatrices;
    rockModelMatrices = new glm::mat4[rock_amount];
    srand(glfwGetTime());
    float radius = 50.f;
    float offset = 25.f;

    // Planet model matrix
    glm::mat4 planetModel = glm::mat4(1.f);
    planetModel = glm::translate(planetModel, glm::vec3(0.f, -3.f, 0.f));
    planetModel = glm::scale(planetModel, glm::vec3(2.f));

    // Rock model matrices
    for(unsigned int i = 0; i < rock_amount; i++){
        glm::mat4 model = glm::mat4(1.0f);
        
        float angle = (float)i / (float)rock_amount*360.0f;
        float displacement = (rand()%(int)(2*offset*100)) / 100.0f - offset;
        float x = sin(angle)*radius+displacement;
        displacement = (rand()%(int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller than x/z
        displacement = (rand()%(int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle)*radius+displacement;
        model = glm::translate(model, glm::vec3(x,y,z));
        
        float scale = (rand()%20)/100.0f+0.05f;
        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = (rand()%360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        rockModelMatrices[i] = model;
    }

    // -----------------------------------

    Model planet("../../../assets/planet/planet.obj");
    Model rock("../../../assets/rock/rock.obj");

    unsigned int rockTexture = loadTexture("../../../assets/rock/rock.png");

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, rock_amount * sizeof(glm::mat4), rockModelMatrices, GL_STATIC_DRAW);

    for (unsigned int i = 0 ; i < rock.m_meshes.size() ; i++){
        unsigned int currentRockVAO = rock.m_meshes[i].VAO;
        glBindVertexArray(currentRockVAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4)));
        
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }
    glBindVertexArray(0);

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

        // -----------------------------------
        // RENDER SCENE

        planetShader.Use();
        glUniformMatrix4fv(projectionLocationPlanet, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocationPlanet, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLocationPlanet, 1, GL_FALSE, glm::value_ptr(planetModel));
        planet.Draw(planetShader);
        
        rockShader.Use();
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        rockShader.SetInt("texure_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        for (unsigned int i = 0; i < rock.m_meshes.size(); i++){
            glBindVertexArray(rock.m_meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.m_meshes[i].m_indices.size()), GL_UNSIGNED_INT, 0, rock_amount);
        }
        glBindVertexArray(0);

        // -----------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}