#version 410

uniform vec4 mdif; // material diffuse (set by Material::Load)

out vec4 outcolor;

void main(void) {
  outcolor = mdif;
}