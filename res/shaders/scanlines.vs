varying vec4 color;
varying vec2 tc;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	color = gl_Color;
	tc = gl_MultiTexCoord0.xy;
}
