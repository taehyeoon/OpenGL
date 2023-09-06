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
	glEnableVertexAttribArray(0); // shader 관련
	glVertexAttribPointer(0, // 위에있는 0과 같은 의미임, 나중에 다시 설명
		2, // 하나의 vertex에 몇개의 데이터를 넘기는지, 여기서는 x,y이므로 2개
		GL_FLOAT, // 입력해주는 데이터 타입
		GL_FALSE, // 나중에 다시 설명
		sizeof(float) * 2, // 값을 하나 읽을 때마다, 몇 바이트 뒤의 데이터를 읽을 지
		0); // 0번째 데이터부터 읽기 시작한다

	

	// glfw창을 사용자가 닫기 직전까지 반복
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);

		// Modern code
		glDrawArrays(GL_TRIANGLES, 0, 3); // Draw call

		// 지금 상태의 코드라면 삼각형이 안보이는 것이 정상
		// 이유는 shader를 작성하지 않았기 때문에 점이 찍히더라도 삼각형 면이 보이지 않게됨

		// 앞 뒤 버퍼를 switch
		glfwSwapBuffers(window);

		// 키보드 마우스 입력처리할 때 사용
		glfwPollEvents();
	}

	return 0;
}