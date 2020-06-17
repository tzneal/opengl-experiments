#version 330 core
precision mediump float;
in vec2 TexCoords;
in vec3 SpriteColor;
in float Alpha;
out vec4 color;


uniform sampler2D image;
void main()
{
    if (Alpha < 0.1) {
        discard;
    }
    vec4 texColor = texture(image, TexCoords);
    if (texColor.a < 0.1) {
        discard;
    }
    color = vec4(SpriteColor, Alpha) * texColor;
}