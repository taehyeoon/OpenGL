#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "MyOpenglHeader.h"

int main(void) {

	legacyTriangles();

	//GLFWwindow* window;

	//if (!glfwInit())
	//	return -1;

	//window = glfwCreateWindow(640, 480, "Hellowwww", NULL, NULL);
	//if (!window) {
	//	glfwTerminate();
	//	return -1;
	//}

	//// 실제로 현재 opengl의 결과물을 그릴 타겟이 무엇이냐를 나타냄
	//glfwMakeContextCurrent(window);

	//// modern opengl을 사용하기 위한 코드
	//// glfwMakeContextCurrent이후에 호출해야 함
	//if (glewInit() != GLEW_OK) {
	//	std::cout << "Error\n";
	//}

	//// GL_Version 값 : 4.6.0 - Build 31.0.101.4502
	//std::cout << glGetString(GL_VERSION) << std::endl;

	//// glfw창을 사용자가 닫기 직전까지 반복
	//while (!glfwWindowShouldClose(window)) {

	//	glClear(GL_COLOR_BUFFER_BIT);
	//	
	//	// Legacy code
	//	glBegin(GL_TRIANGLES);
	//		glVertex2f(-0.5f, -0.5f);
	//		glVertex2f(0.0f, 0.5f);
	//		glVertex2f(0.5f, -0.5f);
	//	glEnd();

	//	// 앞 뒤 버퍼를 switch
	//	glfwSwapBuffers(window);

	//	// 키보드 마우스 입력처리할 때 사용
	//	glfwPollEvents();
	//}

	//return 0;
}