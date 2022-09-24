#ifndef GLSHADER_H
#define GLSHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include "../spindulysGUI.h"


GUI_NAMESPACE_OPEN_SCOPE

class GLShader
{
    public:
        GLShader();

        void Setup(std::string vertexShaderPath,
            std::string fragmentShaderPath);
        void Use();

    private:
        GLuint programID;
};

GUI_NAMESPACE_CLOSE_SCOPE

#endif // GLSHADER_H
