uniform mat4 u_MVPMatrix;
uniform mat4 u_ModelMatrix;

attribute vec3 a_Position;
attribute vec3 a_Normal;

varying vec3 v_PosW;
varying vec3 v_PosM;
varying vec3 v_Normal;

void main() {
	v_PosW = vec3(u_ModelMatrix * vec4(a_Position,1.0) );
	
	v_PosM = a_Position;
	v_PosM.x *= -1.0;
	
	v_Normal = vec3(u_ModelMatrix * vec4(a_Normal,0.0) );
	
	gl_Position = u_MVPMatrix *  vec4(a_Position,1);
}