#version 330

uniform mat4 MVP;
uniform sampler2D u_Texture0;
in vec3 i_Position;
in vec2 i_TextureCoords;
out vec2 TextureCoords;

void main()
{
    TextureCoords = i_TextureCoords;
    gl_Position =  MVP * vec4(i_Position, 1.0);
}

