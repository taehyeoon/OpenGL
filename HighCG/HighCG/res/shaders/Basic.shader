#shader vertex
#version 330 core

layout(location = 0) in vec4 position; //여기 있는 location = 0가, 118, 119 line의 0을 의미함

void main()
{
	gl_Position = position; // gl_Position에 넣어야 할 정보 : clip space coordinate
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color; //출력 color

uniform vec4 u_Color;

void main()
{
	color = u_Color;
}