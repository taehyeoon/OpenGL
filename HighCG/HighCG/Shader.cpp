#include "MyOpenglHeader.h"

// 셰이더 파일 파싱 함수
ShaderProgramSource parseShader(const std::string & filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos) //vertex 셰이더 섹션
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) //fragment 셰이더 섹션
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n'; //코드를 stringstream에 삽입
        }
    }

    return { ss[0].str(), ss[1].str() };
}

//--------Shader 컴파일 함수----------//
unsigned int compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); //셰이더 객체 생성(마찬가지)
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); // 셰이더의 소스 코드 명시
    glCompileShader(id); // id에 해당하는 셰이더 컴파일

    // Error Handling(없으면 셰이더 프로그래밍할때 괴롭다...)
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); //셰이더 프로그램으로부터 컴파일 결과(log)를 얻어옴
    if (result == GL_FALSE) //컴파일에 실패한 경우
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log의 길이를 얻어옴
        char* message = (char*)alloca(length * sizeof(char)); //stack에 동적할당
        glGetShaderInfoLog(id, length, &length, message); //길이만큼 log를 얻어옴
        std::cout << "셰이더 컴파일 실패! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); //컴파일 실패한 경우 셰이더 삭제
        return 0;
    }

    return id;
}

//--------Shader 프로그램 생성, 컴파일, 링크----------//
unsigned int createShader(const std::string& vertexShader, const std::string& fragShader)
{
    unsigned int program = glCreateProgram(); //셰이더 프로그램 객체 생성(int에 저장되는 것은 id)
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragShader);

    //컴파일된 셰이더 코드를 program에 추가하고 링크
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //셰이더 프로그램을 생성했으므로 vs, fs 개별 프로그램은 더이상 필요 없음
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}