varying vec4 frag_color;
varying vec2 texcoord0;
uniform sampler2D texture;

void main(void) 
{ 
	vec4 tex = texture2D(texture, texcoord0); 
	gl_FragColor = tex * frag_color;
}

