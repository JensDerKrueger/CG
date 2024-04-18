#version 410

in vec4 color;
in vec3 pos;
in vec3 normal;

out vec4 result;

void main() {
  vec3 nnormal = normalize(normal);
  vec3 nlightDir = clamp(normalize(vec3(0.0,0.0,0.0)-pos),0.0,1.0);

  result = color*dot(nlightDir,nnormal);
}
