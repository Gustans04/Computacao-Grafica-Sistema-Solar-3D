#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

uniform vec4 lpos;  // light pos in eye space

out data {
  vec3 fragNormal;   // vetor normal para o fragment shader
  vec3 fragLightDir; // vetor para luz para o fragment shader
  vec3 fragViewDir;  // vetor para observador para o fragment shader
  vec2 texcoord; // coordenadas de textura
} v;

void main (void) 
{
  vec3 veye = vec3(Mv*coord); // p'

  v.fragNormal = normalize(vec3(Mn * vec4(normal, 0.0)));

  if (lpos.w == 0) 
    v.fragLightDir = normalize(vec3(lpos));
  else 
    v.fragLightDir = normalize(vec3(lpos)-veye);

  v.fragViewDir = normalize(-veye);

  v.texcoord = texcoord;
  gl_Position = Mvp*coord; 
}

