#version 130
#extension GL_EXT_gpu_shader4 : enable

in vec3 position;

uniform vec2		invRes;
uniform float		displacement;

float RNG(vec2 x)
{
	int n = int(x.x * 40.0 + x.y * 6400.0);
	n = (n << 13) ^ n;
	float r = 1.0 - float( (n * (n * n * 15731 + 789221) +
		1376312589) & 0x7fffffff) * (1.0 / 1073741824.0);
	return r * 0.5 + 0.5;
}

void main()
{
	vec4	pos = gl_ModelViewMatrix * vec4(position, 1.0);
	
	vec2	tc = pos.xy;
	float	x = RNG(vec2(tc.x+100, tc.y+displacement)) * displacement + tc.x*0.4;
	float	y = sin(x + displacement*3)*displacement;
	pos.y += y*0.25;
	
	gl_Position = gl_ProjectionMatrix * pos;
} 
