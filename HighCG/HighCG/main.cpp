#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragSource;
};

//셰이더 파일 파싱 함수
static ShaderProgramSource parseShader(const std::string& filepath)
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

static unsigned int compileShader(unsigned int type, const std::string& source)
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
static unsigned int createShader(const std::string& vertexShader, const std::string& fragShader)
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


int main(void) {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hellowwww", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// 실제로 현재 opengl의 결과물을 그릴 타겟이 무엇이냐를 나타냄
	// GL 관련된 코드는 이 함수 호출 이후에 호출되어야한다
	glfwMakeContextCurrent(window);

	// modern opengl을 사용하기 위한 코드
	// glfwMakeContextCurrent이후에 호출해야 함
	if (glewInit() != GLEW_OK) {
		std::cout << "Error\n";
		return -1;
	}

	// GL_Version 값 : 4.6.0 - Build 31.0.101.4502
	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_CULL_FACE); // 삼각형의 뒷면은 rendering하지 않도록 설정
	{
		//왜 Scope 사용? -> VertexBuffer/IndexBuffer 소멸자에서 Context가 사라진 이후에 delete하려면 glGetError에 계속 오류가 발생..
		//따라서 glfwTerminate() 호출해서 Context가 사라지기 전에 소멸자가 호출되도록 scope를 만듬(동적 할당을 통해 매뉴얼하게 소멸시켜도 됨)


		float positions[] = {
			-0.4f, 1.0f,
			-1.0f, 0.0f,
			0.0f, -0.4f,
			-1.0f, -0.4f,
			-0.7f, -1.0f,
			-0.4f, -0.4f, // 5
			0.0f, -0.7f,
			0.4f, -1.0f,
			1.0f, 0.0f,
			0.0f, 0.4f,
			1.0f, 0.4f,
			1.0f, 0.7f // 11

		};

		unsigned int indices[] = {
			0, 1, 2, // t1
			3, 4, 5,  // t2
			6, 7, 8, // t1
			9, 10, 11  // t2
		};

		// vao를 먼저 binding한 이후에 bufferID와 ibo를 바인딩하면,
		// 자동으로 두개의 buffer가 vao에 저장됨
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// 데이터를 전달하는 과정
		VertexBuffer vb{ positions, 12 * 2 * sizeof(float) };

		// 데이터를 해석하는 방법
		glEnableVertexAttribArray(0); // shader 관련, 0번 location에 저장할거다라는 의미
		glVertexAttribPointer(0, // 0번 location에 있는 데이터들을 읽기 시작할거다라는 의미
			2, // 하나의 vertex에 몇개의 데이터를 넘기는지, 여기서는 x,y이므로 2개
			GL_FLOAT, // 입력해주는 데이터 타입
			GL_FALSE, // 나중에 다시 설명
			sizeof(float) * 2, // 값을 하나 읽을 때마다, 몇 바이트 뒤의 데이터를 읽을 지
			0); // 0번째 데이터부터 읽기 시작한다

		// Index Buffer
		IndexBuffer ib{ indices, 12 };


		//---------Shader 생성---------------//
		ShaderProgramSource source = parseShader("res/shaders/Basic.shader");

		// 실제로 구동되는 gpu side의 shader코드는 우리가 tracking할 수 없음
		// 따라서 Shader의 정보를 index로 cpu side에 저장
		unsigned int shaderID = createShader(source.VertexSource, source.FragSource);
		glUseProgram(shaderID); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
		//draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림
		//glUseProgram(0); 쉐이더를 초기화 하는 코드 -> 다시 힌색으로 출력됨

		// Shader Uniform 전역변수 값 전달
		GLCall(int location = glGetUniformLocation(shaderID, "u_Color"));
		ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.9f, 0.7f, 0.4f, 1.0f)); // u_Color에 값을 넣겠다 (r, g, b, a)값


		// 모든 gl관련 데이터를 unbind시킴
		// 이제는 그리기 직전에 vao, shader만 다시 bind시키면 됨
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);

		// glfw창을 사용자가 닫기 직전까지 반복
		while (!glfwWindowShouldClose(window)) {

			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(vao);
			glUseProgram(shaderID);

			// Modern code
			//glDrawArrays(GL_TRIANGLES, 0, 3); // Draw call
			// GLCall :gl 관련 함수를 실행할 때, 오류가 발생하는 경우, 오류메시지를 출력해준다
			GLCall(glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr));

			// 앞 뒤 버퍼를 switch
			glfwSwapBuffers(window);

			// 키보드 마우스 입력처리할 때 사용
			glfwPollEvents();
		}

		glDeleteProgram(shaderID); // 쉐이더 삭제

	}

	glfwTerminate();

	return 0;
}