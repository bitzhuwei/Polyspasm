#version 130

uniform mat4	textTransform;

in vec3		position;
in vec3		normal;

out vec3	worldPos;
out vec3	pos;
out vec3	norm;

void main()
{
	mat4	transform = textTransform * gl_ModelViewMatrix;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	worldPos = position;
	pos = (transform * gl_Vertex).xyz;
#if 1
	norm = (transform * vec4(normal, 1.0)).xyz;
#else
	norm = (transform * vec4(gl_Normal, 1.0)).xyz;
#endif
}
