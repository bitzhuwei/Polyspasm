#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D	tex;
uniform float		displacement;

in vec4 color;
in vec2 tc;

out vec4 fragColor;

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
	float	x = RNG(vec2(tc.x+100, tc.y+displacement)) * displacement * 0.4 + tc.x*0.4;
	float	y = sin(x + displacement*3)*displacement;
	fragColor = 0.6 * color * texture(tex, tc + vec2(0, y));
}
