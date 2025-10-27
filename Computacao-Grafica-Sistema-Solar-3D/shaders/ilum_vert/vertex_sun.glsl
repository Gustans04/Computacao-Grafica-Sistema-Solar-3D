#version 410

layout(location = 0) in vec4 coord;
layout(location = 3) in vec2 texcoord;

uniform mat4 Mvp;
uniform vec4 mdif;

out data {
  vec4 color;
  vec2 texcoord;
} v;

void main(void) {
  v.color = mdif;
  v.texcoord = texcoord;
  gl_Position = Mvp * coord;
}