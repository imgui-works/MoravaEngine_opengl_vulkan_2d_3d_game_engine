// Skybox shader
// #type fragment
#version 430

layout(location = 0) out vec4 finalColor;

uniform samplerCube u_Texture;
uniform float u_TextureLod;

in vec3 v_Position;

void main()
{
	finalColor = textureLod(u_Texture, v_Position, u_TextureLod);
}