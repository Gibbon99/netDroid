#version 330

uniform sampler2D   u_Texture0;
in      vec2        TextureCoords;
out     vec4        pixelColor;

void main()
{
    vec4 texColor = texture2D(u_Texture0, TextureCoords.xy);
    pixelColor = vec4(texColor.rgb, 1.0f);
}