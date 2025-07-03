#pragma once

unsigned int createVertexShader(const char* file); 
unsigned int createFragmentShader(const char* file);
unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int createShaderProgramS(const char* vertexShaderPath, const char* fragmentShaderPath);

//make more efficient
#include <HandmadeMath.h>
#include <glad.h>
inline void setUniformMat4(const char* name, HMM_Mat4 m, unsigned int shaderProgram)
{
    GLint location = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, m.Elements[0]);
}

inline void setUniformInt1(const char* name, int i, unsigned int shaderProgram)
{
    GLint location = glGetUniformLocation(shaderProgram, name);
    glUniform1i(location, i);
}

inline void setUniformVec3(const char* name, HMM_Vec3 v, unsigned int shaderProgram)
{
    GLint location = glGetUniformLocation(shaderProgram, name);
    glUniform3fv(location, 1, v.Elements);
}

inline void setUniformFloat(const char* name, float f, unsigned int shaderProgram)
{
    GLint location = glGetUniformLocation(shaderProgram, name);
    glUniform1f(location, f);
}

inline void useShader(unsigned int shaderProgram)
{
    glUseProgram(shaderProgram);
}