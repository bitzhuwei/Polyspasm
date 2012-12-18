// Removed version because it caused problems on nvidia
//#version 100

varying vec3	cameraRay;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vec4	wp = gl_ModelViewMatrix * gl_Vertex;
	cameraRay = wp.xyz;
}
