in vec3 worldPosition;
in vec3 worldNormal;
in vec3 materialColor;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 specularLight;
uniform float shininess;

out vec4 fragmentColor;

void main() {
  // TODO:
  // Implement direct local Phong illumination with ambient, diffuse and
  // specular terms.

  fragmentColor = vec4(materialColor, 1.0);
}
