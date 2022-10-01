#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Color;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 View;

out vec4 VertexColor;

void main()
{

//...pode ter mais linhas de código aqui!
gl_Position = projection *View* model * vec4(position.x, position.y, position.z, 1.0);
VertexColor = vec4(Color,1.0f);

}