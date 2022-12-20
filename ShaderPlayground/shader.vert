#version 410

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 MVit;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec3 vNormal;

out vec4 color;
out vec3 normal;
out vec3 pos;

void main() {
  gl_Position = MVP * vec4(vPos, 1.0);
  pos = (MV * vec4(vPos, 1.0)).xyz;
  color = vColor;
  normal = (MVit * vec4(vNormal, 0.0)).xyz;
}
