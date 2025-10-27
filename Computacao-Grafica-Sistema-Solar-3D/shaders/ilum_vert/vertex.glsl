#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

uniform vec4 lpos;  // light pos in eye space

out vec3 fragNormal;   // vetor normal para o fragment shader
out vec3 fragLightDir; // vetor para luz para o fragment shader
out vec3 fragViewDir;  // vetor para observador para o fragment shader

void main (void) 
{
  vec3 veye = vec3(Mv*coord); // p'

  fragNormal = normalize(vec3(Mn * vec4(normal, 0.0)));

  if (lpos.w == 0) 
    fragLightDir = normalize(vec3(lpos));
  else 
    fragLightDir = normalize(vec3(lpos)-veye);

  fragViewDir = normalize(-veye);

  gl_Position = Mvp*coord; 
}

