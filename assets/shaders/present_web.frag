#version 300 es
precision highp float;
out vec4 outColor;
uniform sampler2D uFinal;
void main() {
    ivec2 sz = textureSize(uFinal, 0);
    vec2 uv = gl_FragCoord.xy / vec2(sz);
    outColor = texture(uFinal, uv);
}
