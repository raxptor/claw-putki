attribute vec2 vpos; 
attribute vec2 uv0;
attribute vec4 color;

varying vec4 frag_color;
varying vec2 texcoord0;

uniform mat4 proj;

void main(void)
{
	frag_color = color;
	texcoord0 = uv0;
	gl_Position = proj * vec4(vpos.x, vpos.y, 0.0, 1.0);
}
