#version 410

uniform samplerCube sky;

in vec3 dir;
out vec4 color;

void main(void)
{
	color = texture(sky, dir*vec3(1,1,-1));
}