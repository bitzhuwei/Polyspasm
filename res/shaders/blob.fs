#version 130

uniform vec2	pos;
uniform float	radius;

out vec4		fragColor;

void main()
{
	const float magnitude = 125;
	vec2	d = gl_FragCoord.xy - pos;
	float	len = length(d);
	if (len < radius) {
		float	value = (1.0 - pow(len/radius, 6.0)) * magnitude;
		fragColor = vec4(value);
	} else {
		discard;
	}
}