#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec2 oUV;
out vec3 oNormal;

void main(){
  oUV = aUV;
  oNormal = aNormal;
  gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
}
