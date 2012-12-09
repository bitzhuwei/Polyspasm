#version 330
#extension GL_EXT_gpu_shader4 : enable

out vec4	fragColor;

float RNG(vec2 x)
{
	uint n = floatBitsToUint(x.y * 214013.0 + x.x * 2531011.0);
	n = n * (n * n * 15731u + 789221u);
	n = (n >> 9u) | 0x3F800000u;

	return 2.0 - uintBitsToFloat(n);
}

void main()
{
	fragColor = vec4(vec3(RNG(gl_FragCoord.xy)), 1.0);
}
