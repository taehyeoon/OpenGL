#include "MyOpenglHeader.h"


 //glGetError는 에러를 하나씩만 반환하기 때문에, 한 번 확인에 모든 오류를 뽑아내는 것이 필요함
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR); // GL_NO_ERROR == 0
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ") : " << function <<
            " " << file << " in line " << line << std::endl;
        return false;
    }
    return true;
}