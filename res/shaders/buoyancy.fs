#version 130

uniform sampler2D	u;
uniform sampler2D	temp;
uniform float		tDelta;
uniform float		sigma;
uniform vec2		invRes;

out vec2			uOut;

void main()
{
	ivec2	tc = ivec2(gl_FragCoord.xy);
	vec2	v = texelFetch(u, tc, 0).xy;
	float	T = texelFetch(temp, tc, 0).x;
	
	uOut = v + 0.15*T * vec2(0, tDelta);
}
