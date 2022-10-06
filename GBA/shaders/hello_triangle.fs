#version 450

//uniform vec4 inputColor;

out vec4 color;
in vec4 VertexColor;
uniform vec4 inputColor;

void main()
{

color = inputColor;

}