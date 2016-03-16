attribute vec3 a_Position;

uniform mat4 u_VPMatrix;
uniform mat4 u_ModelMatrix;

varying vec4 v_PosLightSpace;
void main(void)
{
	gl_Position = u_VPMatrix * u_ModelMatrix * vec4(a_Position, 1);
	v_PosLightSpace = gl_Position;
}