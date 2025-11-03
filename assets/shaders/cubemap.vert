#version 330 core
out vec3 vDir;
uniform mat4 u_View;
uniform mat4 u_Proj;
layout(location=0) in vec3 aPos;
void main() {
    vDir = aPos;
    mat4 view = mat4(mat3(u_View));
    vec4 pos = u_Proj * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
