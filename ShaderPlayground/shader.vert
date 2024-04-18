#version 410

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec3 vNormal;

out vec4 color;
out vec3 normal;
out vec3 pos;

void main() {
  mat4 MVP = P*V*M;
  mat4 MV  = V*M;
  mat4 MVit = transpose(inverse(MV));

  gl_Position = MVP * vec4(vPos, 1.0);
  pos = (MV * vec4(vPos, 1.0)).xyz;
  color = vColor;
  normal = (MVit * vec4(vNormal, 0.0)).xyz;
}
