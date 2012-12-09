#version 130

out vec2 tc;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	tc = gl_MultiTexCoord0.xy;
}
