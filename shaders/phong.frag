#version 330 core

out vec4 fragColor;

in vec3 vertexColor;
in vec2 texCoords;
in vec3 Normal;
in vec3 Vert_World_Pos;

uniform sampler2D texture_diffuse1;

uniform vec3 Light_Pos;
uniform vec3 Light_Color;

uniform vec3 View_Pos;

uniform vec4 ourColor;

vec4 ambientLight()
{
	float ambientStrength = 0.1f;
	return vec4(ambientStrength * Light_Color, 1.f);
}

vec4 diffuseLight()
{
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(Light_Pos - Vert_World_Pos);
	float diff = max(dot(normal,lightDir), 0.0f);
	return vec4(diff * Light_Color, 1.0f);
}

vec4 specularLight()
{
	float specularStrength = 0.5f;
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(Light_Pos - Vert_World_Pos);
	vec3 viewDir = normalize(View_Pos - Vert_World_Pos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0),32);
	return vec4(specularStrength * spec * Light_Color, 1.f);
}

void main()
{
	vec4 baseColor = texture(texture_diffuse1, texCoords);
	vec4 ambientComp = ambientLight();
	vec4 diffuseComp = diffuseLight();
	vec4 specularComp = specularLight();
	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	fragColor = baseColor * (ambientComp + diffuseComp + specularComp);
	//FragColor = vec4(abs(vertexColor),1.0f);
}

