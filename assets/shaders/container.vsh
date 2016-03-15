uniform mat4 u_MVPMatrix; 

attribute vec3 a_Position; 
attribute vec3 a_Normal; 
attribute vec2 a_TexCoord; 

varying vec3 v_Pos; 
varying vec3 v_Normal; 
varying vec2 v_TexCoord; 

void main() { 
	v_Pos = a_Position; 
	v_Normal = a_Normal; 
	v_TexCoord = a_TexCoord; 
	gl_Position = u_MVPMatrix *  vec4(a_Position,1);
}