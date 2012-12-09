#version 130

uniform sampler2D	x;
uniform sampler2D	u;
uniform float		tDelta;
uniform float		dissipation;
uniform vec2		invRes;

out vec4			xOut;

void main()
{
	vec2	tc = gl_FragCoord.xy * invRes;
	vec2	v = texture(u, tc).xy;
	
	vec2	dx = - invRes * tDelta * v;
	xOut = dissipation * texture(x, tc + dx);
}
