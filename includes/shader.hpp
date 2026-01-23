#pragma once 

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
    public:
        unsigned int m_id; // program id

        Shader(const char* vertexPath, const char* fragmentPath)
        {
            std::string vertexCode, fragmentCode;
            std::ifstream vShaderFile, fShaderFile;
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            try{
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();
                vShaderFile.close();
                fShaderFile.close();
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            }
            catch(std::ifstream::failure e){
                std::cout << "Can't read shader file\n";
            }
            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

            unsigned int vertex, fragment;
            int success;
            char infoLog[512];

            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success){
                glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                std::cerr << "Error in vertex shader : " << infoLog << "\n";
            }

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success){
                glGetShaderInfoLog(fragment, 512, NULL, infoLog);
                std::cerr << "Error in fragment shader : " << infoLog << "\n";
            }

            m_id = glCreateProgram();
            glAttachShader(m_id, vertex);
            glAttachShader(m_id, fragment);
            glLinkProgram(m_id);
            glGetProgramiv(m_id, GL_LINK_STATUS, &success);
            if (!success){
                glGetProgramInfoLog(m_id, 512, NULL, infoLog);
                std::cerr << "Error while linking shaders : " << infoLog << "\n";
            }

            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }

        void Use()
        {
            glUseProgram(m_id);
        }

        void SetBool(const std::string& name, const bool value) const
        {
            glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
        }

        void SetInt(const std::string& name, const int value) const
        {
            glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
        }

        void SetFloat(const std::string& name, const float value) const
        {
            glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
        }

        void SetVec3(const std::string& name, const glm::vec3& value) const
        {
            glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
        }
};
