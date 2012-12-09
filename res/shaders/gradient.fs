#version 130

uniform sampler2D	w;
uniform sampler2D	p;
uniform vec2		invRes;
uniform float		halfrdx;

out vec2			uOut;

void main()
{
	ivec2	tc = ivec2(gl_FragCoord.xy);
	float	pL = texelFetchOffset(p, tc, 0, ivec2(-1, 0)).x;
	float	pR = texelFetchOffset(p, tc, 0, ivec2(1, 0)).x;
	float	pB = texelFetchOffset(p, tc, 0, ivec2(0, -1)).x;
	float	pT = texelFetchOffset(p, tc, 0, ivec2(0, 1)).x;
	
	uOut = texelFetch(w, tc, 0).xy;
	uOut -= halfrdx * vec2(pR - pL, pT - pB);
}
