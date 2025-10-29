#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoord;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

uniform vec4 lpos;  // light pos in eye space

out data {
  vec3 fragNormal;   // vetor normal para o fragment shader
  vec3 fragTangent;  // vetor tangente
  vec3 fragBitangent;// vetor bitangente
  vec3 fragLightDir; // vetor para luz para o fragment shader
  vec3 fragViewDir;  // vetor para observador para o fragment shader
  vec2 texcoord;     // coordenadas de textura
} v;

void main (void) 
{
  vec3 veye = vec3(Mv*coord); // p'

  v.fragNormal = normalize(vec3(Mn * vec4(normal, 0.0)));
  v.fragTangent = normalize(vec3(Mn * vec4(tangent, 0.0)));
  v.fragBitangent = normalize(cross(v.fragNormal, v.fragTangent));

  // construct TBN matrix (columns are T, B, N)
  mat3 TBN = transpose(mat3(normalize(v.fragTangent), normalize(v.fragBitangent), normalize(v.fragNormal)));

  if (lpos.w == 0) 
    v.fragLightDir = normalize(vec3(lpos));
  else 
    v.fragLightDir = normalize(vec3(lpos)-veye);

  v.fragViewDir = normalize(-veye);

  v.fragLightDir = TBN * v.fragLightDir;
  v.fragViewDir = TBN * v.fragViewDir;

  v.texcoord = texcoord;
  gl_Position = Mvp*coord; 
}

