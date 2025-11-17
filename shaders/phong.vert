#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out vec3 vertexColor;
out vec3 Vert_World_Pos;
out vec3 Normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

   gl_Position =  projection * view * model * vec4(inPos, 1.0);
   Vert_World_Pos = vec3(model * vec4(inPos,1.0f));
   vertexColor = inPos;
   texCoords = inTexCoords;
   Normal = mat3(transpose(inverse(model))) * inNormal;
}
