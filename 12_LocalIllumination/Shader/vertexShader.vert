in vec3 vPos;
in vec4 vColor;
in vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 materialColor;

void main() {
  vec4 world = modelMatrix * vec4(vPos, 1.0);
  worldPosition = world.xyz;

  worldNormal = normalize((normalMatrix * vec4(vNormal, 0.0)).xyz);
  materialColor = vColor.rgb;

  gl_Position = projectionMatrix * viewMatrix * world;
}
