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

	//// ������ ���� opengl�� ������� �׸� Ÿ���� �����̳ĸ� ��Ÿ��
	//glfwMakeContextCurrent(window);

	//// modern opengl�� ����ϱ� ���� �ڵ�
	//// glfwMakeContextCurrent���Ŀ� ȣ���ؾ� ��
	//if (glewInit() != GLEW_OK) {
	//	std::cout << "Error\n";
	//}

	//// GL_Version �� : 4.6.0 - Build 31.0.101.4502
	//std::cout << glGetString(GL_VERSION) << std::endl;

	//// glfwâ�� ����ڰ� �ݱ� �������� �ݺ�
	//while (!glfwWindowShouldClose(window)) {

	//	glClear(GL_COLOR_BUFFER_BIT);
	//	
	//	// Legacy code
	//	glBegin(GL_TRIANGLES);
	//		glVertex2f(-0.5f, -0.5f);
	//		glVertex2f(0.0f, 0.5f);
	//		glVertex2f(0.5f, -0.5f);
	//	glEnd();

	//	// �� �� ���۸� switch
	//	glfwSwapBuffers(window);

	//	// Ű���� ���콺 �Է�ó���� �� ���
	//	glfwPollEvents();
	//}

	//return 0;
}