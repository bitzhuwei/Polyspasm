#version 130

uniform sampler2D	value;
uniform vec2		invRes;

out vec4 fragColor;

const float	S = 17.0/10.0;
const float	s0 = S*8.0;
const float	s1 = S*6.4;
const float	s2 = S*3.0;
const float	s3 = S*0.8;
const float	s4 = S*0.0;
const float	l0 = S*10-s0;
const float	l1 = s0-s1;
const float	l2 = s1-s2;
const float	l3 = s2-s3;
const float	l4 = s3-s4;
const vec3	c0 = vec3(0.95, 0.95, 1.0);
const vec3	c1 = vec3(1.0, 1.0, 1.0);
const vec3	c2 = vec3(1.0, 1.0, 0.0);
const vec3	c3 = vec3(0.9, 0.4, 0.0);
const vec3	c4 = vec3(0.5, 0.0, 0.0);
const vec3	c5 = vec3(0.0, 0.0, 0.0);

void main()
{
	float	v = texture(value, gl_FragCoord.xy * invRes).r;
	float	m0 = clamp((v-s0) / l0, 0.0, 1.0);
	float	m1 = clamp((v-s1) / l1, 0.0, 1.0);
	float	m2 = clamp((v-s2) / l2, 0.0, 1.0);
	float	m3 = clamp((v-s3) / l3, 0.0, 1.0);
	float	m4 = clamp((v-s4) / l4, 0.0, 1.0);
	vec3	c;
	c = mix(c5, c4, m4);
	c = mix(c, c3, m3);
	c = mix(c, c2, m2);
	c = mix(c, c1, m1);
	c = mix(c, c0, m0);
	fragColor = vec4(c, 1.0);
}
