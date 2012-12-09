#version 130
in vec3			n;
out vec4		fragColor;

uniform vec3	color;

void main()
{
	vec3 l = normalize(vec3(0, 1, 2));
	vec3 light = color * min(dot(n, l) + 0.3, 1.0);
	fragColor = vec4(light, 1.0);
}