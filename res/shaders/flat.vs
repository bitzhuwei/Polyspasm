#version 130
in vec3 position;
in vec3 normal;

out vec3 n;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
	n = normal;
}