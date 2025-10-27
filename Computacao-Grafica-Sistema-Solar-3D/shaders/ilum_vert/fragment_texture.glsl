#version 410

out vec4 fcolor;

in data {
  vec3 fragNormal;   // vetor normal (interpolado)
  vec3 fragLightDir; // vetor para luz (interpolado)
  vec3 fragViewDir;  // vetor para observador (interpolado)
  vec2 texcoord;
} f;

uniform vec4 lamb;    // componente ambiente da luz
uniform vec4 ldif;    // componente difusa da luz
uniform vec4 lspe;    // componente especular da luz

uniform vec4 mamb;    // componente ambiente do material
uniform vec4 mdif;    // componente difusa do material
uniform vec4 mspe;    // componente especular do material
uniform float mshi;   // brilho do material

uniform sampler2D decal;

void main (void)
{
  vec3 normal = normalize(f.fragNormal);
  vec3 luz = normalize(f.fragLightDir);
  vec3 view = normalize(f.fragViewDir);

  // ambiente e difusa
  vec4 ambiente = mamb * lamb;
  vec4 difusa = max(dot(normal,luz), 0.0) * mdif * ldif;

  fcolor = (ambiente + difusa) * texture(decal,f.texcoord);

  // especular
  if (dot(normal,luz) > 0) {
    vec3 refl = normalize(reflect(-luz, normal));
    fcolor += pow(max(dot(refl, view), 0.0), mshi) * mspe * lspe;
  }
}

