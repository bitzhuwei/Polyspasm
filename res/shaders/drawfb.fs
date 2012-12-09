#version 130

uniform sampler2D	tex;
uniform vec2		invRes;
out vec4			fragColor;

void main()
{
	fragColor = texture(tex, gl_FragCoord.xy * invRes);
}
