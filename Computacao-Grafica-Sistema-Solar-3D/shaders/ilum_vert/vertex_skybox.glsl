#version 410

layout(location = 0) in vec4 pos;

uniform mat4 Mvp;

out vec3 dir;

void main(void)
{
	dir = vec3(pos);
	gl_Position = Mvp * pos;
}
