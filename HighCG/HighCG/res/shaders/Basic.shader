#shader vertex
#version 330 core

layout(location = 0) in vec4 position; //여기 있는 location = 0가, 118, 119 line의 0을 의미함

void main()
{
	gl_Position = position; //119에서 보다시피, 2개의 값만 전달했지만, 알아서 vec4로 변환해줌
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color; //출력 color

void main()
{
	color = vec4(0.0, 0.0 ,1.0, 1.0); //파란색 반환
}