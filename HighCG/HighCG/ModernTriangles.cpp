﻿#include "MyOpenglHeader.h"

int modernTriangles() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hellowwww", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// 실제로 현재 opengl의 결과물을 그릴 타겟이 무엇이냐를 나타냄
	glfwMakeContextCurrent(window);

	// modern opengl을 사용하기 위한 코드
	// glfwMakeContextCurrent이후에 호출해야 함
	if (glewInit() != GLEW_OK) {
		std::cout << "Error\n";
		return -1;
	}

	// GL_Version 값 : 4.6.0 - Build 31.0.101.4502
	std::cout << glGetString(GL_VERSION) << std::endl;


	float position[6] = {
		-0.5f, -0.5f,
		0.0f, 0.5f,
		0.5f, -0.5f
	};

	unsigned int bufferID;
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); // <-- bind 는 activate 역할
	glBufferData(GL_ARRAY_BUFFER, // 실제 data를 CPU -> GPU 로 넘기는 과정
				6 * sizeof(float),
				position,
				GL_STATIC_DRAW);

	// 데이터를 해석하는 방법
	glEnableVertexAttribArray(0); // shader 관련, 0번 location에 저장할거다라는 의미
	glVertexAttribPointer(0, // 0번 location에 있는 데이터들을 읽기 시작할거다라는 의미
		2, // 하나의 vertex에 몇개의 데이터를 넘기는지, 여기서는 x,y이므로 2개
		GL_FLOAT, // 입력해주는 데이터 타입
		GL_FALSE, // 나중에 다시 설명
		sizeof(float) * 2, // 값을 하나 읽을 때마다, 몇 바이트 뒤의 데이터를 읽을 지
		0); // 0번째 데이터부터 읽기 시작한다

	//---------Shader 생성---------------//
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;" //여기 있는 location = 0가, 118, 119 line의 0을 의미함
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n" //119에서 보다시피, 2개의 값만 전달했지만, 알아서 vec4로 변환해줌
		"}\n";

	std::string fragShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;" //출력 color
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 1.0 ,0.0, 1.0);\n" //빨간색 반환
		"}\n";
	
	// 실제로 구동되는 gpu side의 shader코드는 우리가 tracking할 수 없음
	// 따라서 Shader의 정보를 index로 cpu side에 저장
	unsigned int shaderID = createShader(vertexShader, fragShader);
	glUseProgram(shaderID); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
	//draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림
	

	// glfw창을 사용자가 닫기 직전까지 반복
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);

		// Modern code
		glDrawArrays(GL_TRIANGLES, 0, 3); // Draw call

		// 앞 뒤 버퍼를 switch
		glfwSwapBuffers(window);

		// 키보드 마우스 입력처리할 때 사용
		glfwPollEvents();
	}

	glDeleteProgram(shaderID);

	return 0;
}