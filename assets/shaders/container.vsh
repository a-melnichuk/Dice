uniform mat4 u_MVPMatrix; 
uniform mat4 u_LightVPMatrix;
uniform mat4 u_ModelMatrix;

attribute vec3 a_Position; 
attribute vec3 a_Normal; 
attribute vec2 a_TexCoord; 

varying vec3 v_Pos; 
varying vec3 v_Normal; 
varying vec2 v_TexCoord; 
varying vec4 v_ShadowMapCoord;

void main() { 

	const mat4 biasMat = mat4(0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0,
                              0.5, 0.5, 0.0, 1.0);
	v_Pos = a_Position; 
	v_Normal = a_Normal; 
	v_TexCoord = a_TexCoord; 
	v_ShadowMapCoord = u_LightVPMatrix* u_ModelMatrix * vec4(a_Position,1);
	v_ShadowMapCoord= biasMat * v_ShadowMapCoord;
	gl_Position = u_MVPMatrix *  vec4(a_Position,1);
}