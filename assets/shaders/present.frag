#version 450
layout(location=0) out vec4 outColor;
layout(binding=0) uniform sampler2D uFinal;
void main() {
    ivec2 sz = textureSize(uFinal, 0);
    vec2 uv = gl_FragCoord.xy / vec2(sz);
    outColor = texture(uFinal, uv);
}
