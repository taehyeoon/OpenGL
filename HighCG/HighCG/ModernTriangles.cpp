#include "MyOpenglHeader.h"

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

	float position[] = {
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f // 3
	};

	unsigned int indices[] = {
		0, 1, 2, // t1
		2, 3, 0  // t2
	};


	unsigned int bufferID;
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); // <-- bind 는 activate 역할
	glBufferData(GL_ARRAY_BUFFER, // 실제 data를 CPU -> GPU 로 넘기는 과정
				8 * sizeof(float),
				position,
				GL_STATIC_DRAW); // 그려야될 데이터들이 런타임에 거의 변하지 않을 것이라는 정보

	unsigned int ibo; // index buffer object
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				6 * sizeof(unsigned int),
				indices,
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
	ShaderProgramSource source = parseShader("res/shaders/Basic.shader");
	
	// 실제로 구동되는 gpu side의 shader코드는 우리가 tracking할 수 없음
	// 따라서 Shader의 정보를 index로 cpu side에 저장
	unsigned int shaderID = createShader(source.VertexSource, source.FragSource);
	glUseProgram(shaderID); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
	//draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림
	//glUseProgram(0); 쉐이더를 초기화 하는 코드 -> 다시 힌색으로 출력됨

	// glfw창을 사용자가 닫기 직전까지 반복
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);

		// Modern code
		//glDrawArrays(GL_TRIANGLES, 0, 3); // Draw call
		// GLCall :gl 관련 함수를 실행할 때, 오류가 발생하는 경우, 오류메시지를 출력해준다
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		// 앞 뒤 버퍼를 switch
		glfwSwapBuffers(window);

		// 키보드 마우스 입력처리할 때 사용
		glfwPollEvents();
	}

	glDeleteProgram(shaderID);

	return 0;
}