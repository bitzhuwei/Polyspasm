#version 130
in vec3			n;
out vec4		fragColor;

void main()
{
	fragColor = vec4(n, 1.0);
}