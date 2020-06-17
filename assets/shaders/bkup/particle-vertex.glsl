#version 300 es
in vec4 vertex;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;
uniform float scale;
uniform float rotation;
mat4 rotationZ(in float angle) {
    return mat4(cos(angle), -sin(angle), 0, 0,
    sin(angle), cos(angle), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);
}
void main()
{
    TexCoords = vertex.zw;
    ParticleColor = color;
    // translate
    vec4 rotated = vertex - vec4(0.5, 0.5, 0, 0);
    // rotate
    rotated *= rotationZ(rotation);
    // translate back
    rotated += vec4(0.5, 0.5, 0, 0);
    gl_Position = projection * vec4((rotated.xy * scale) + offset, 0.0, 1.0);
}