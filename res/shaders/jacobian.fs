#version 130

uniform sampler2D	x;
uniform sampler2D	b;
uniform float		alpha;
uniform float		rBeta;

out float			xOut;

void main()
{
	ivec2	tc = ivec2(gl_FragCoord.xy);
	
	// sample x - left right top and bottom
	float	xL = texelFetchOffset(x, tc, 0, ivec2(1, 0)).x;
	float	xR = texelFetchOffset(x, tc, 0, ivec2(-1, 0)).x;
	float	xT = texelFetchOffset(x, tc, 0, ivec2(0, 1)).x;
	float	xB = texelFetchOffset(x, tc, 0, ivec2(0, -1)).x;
	
	// sample b - center
	float	bC = texelFetch(b, tc, 0).x;
	xOut = (xL + xR + xB + xT + alpha * bC) * rBeta;
}