#version 330 core
layout (location = 0) in vec4 vertex;
layout (location = 1) in vec2 offset;
layout (location = 2) in float rotation;
layout (location = 3) in float scale;
layout (location = 4) in vec3 color;
layout (location = 5) in float alpha;

out vec2 TexCoords;
out vec3 SpriteColor;
out float Alpha;
uniform mat4 projection;

mat4 rotationZ(in float angle) {
    return mat4(cos(angle), -sin(angle), 0, 0,
    sin(angle), cos(angle), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);
}
void main()
{
    // translate to the center
    vec4 rotated = vec4(vertex.xy, 0, 1) - vec4(0.5, 0.5, 0, 0);
    // rotate
    rotated *= rotationZ(rotation);
    // translate back
    rotated += vec4(0.5, 0.5, 0, 0);

    TexCoords = vertex.xy;
    SpriteColor = color;
    Alpha = alpha;
    //gl_Position = projection * vec4(vertex*scale + aOffset, 0.0, 1.0);
    gl_Position = projection * vec4((rotated.xy * scale) + offset, 0.0, 1.0);
}