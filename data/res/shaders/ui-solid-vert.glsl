attribute vec2 vpos; 
attribute vec2 uv0;
attribute vec4 color;

varying vec4 frag_color;

uniform mat4 proj;

void main(void)
{
	frag_color = color;
	gl_Position = proj * vec4(vpos.x, vpos.y, 0.0, 1.0);
}
