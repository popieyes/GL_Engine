#version 330 core

out vec4 FragColor;

in vec3 vertexColor;
in vec2 texCoords;

uniform sampler2D texture_diffuse1;

uniform vec4 ourColor;

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	FragColor = texture(texture_diffuse1, texCoords);
	//FragColor = vec4(abs(vertexColor),1.0f);
}