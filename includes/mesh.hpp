#pragma once 

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type; // "texture_diffuse" or "texture_specular"
};

class Mesh
{
    public:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<Texture> m_textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
        std::vector<Texture> textures) {
            m_vertices = vertices;
            m_indices = indices;
            m_textures = textures;
            setupMesh();
        }

        void Draw(Shader& shader){
            unsigned int diffuse_nr = 1;
            unsigned int specular_nr = 1;
            for (unsigned int i = 0 ; i < m_textures.size() ; i++){
                glActiveTexture(GL_TEXTURE0+i);
                std::string number;
                std::string name = m_textures[i].type;
                if (name =="texture_diffuse")
                    number = std::to_string(diffuse_nr++);
                if (name =="texture_specular")
                    number = std::to_string(specular_nr++);
                shader.SetInt((name+number).c_str(), i); // SetFloat ? (is used in the tutorial)
                glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
            }
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh(){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
                &m_vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
                &m_indices[0], GL_STATIC_DRAW);
            
            // https://en.cppreference.com/w/cpp/types/offsetof.html
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            
            glBindVertexArray(0);
        }
};