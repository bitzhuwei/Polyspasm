#version 130
uniform sampler2D	source;
uniform vec2		invRes;
uniform float		alpha;

out vec4			fragColor;

void main()
{
	//fragColor.rgb = texture(source, gl_FragCoord.xy*invRes).rgb;
	fragColor = vec4(0, 0, 0, alpha);
}