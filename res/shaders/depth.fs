#version 130

uniform sampler2D	depthFront;
uniform sampler2D	depthBack;
uniform vec2		invRes;

out vec4			fragColor;

const float			n = 0.1;
const float			f =  5.0;

void main()
{
	float		front = (2.0 * n) / (f + n - texture(depthFront, gl_FragCoord.xy * invRes).r * (f-n));
	float		back = (2.0 * n) / (f + n - texture(depthBack, gl_FragCoord.xy * invRes).r * (f-n));
	fragColor = vec4(vec3(back - front)*60.0, 1.0);
	//fragColor = vec4(vec3(back), 1.0);
}