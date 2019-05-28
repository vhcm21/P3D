#version 440 core

layout (location = 0) in vec2 textureCoord;		// Coordenada de textura do v�rtice

uniform sampler2D TexSampler;					// Sampler de textura

layout (location = 0) out vec4 fColor;			// Cor final do fragmento

void main()
{
	fColor = texture(TexSampler, textureCoord);
}