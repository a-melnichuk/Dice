precision mediump float;

uniform samplerCube u_TextureUnit;
uniform vec3 u_EyePos;
uniform vec3 u_LightColor;

varying vec3 v_PosW;
varying vec3 v_PosM;
varying vec3 v_Normal;

void main() {
	vec3 color = textureCube(u_TextureUnit, v_PosM).rgb;

	vec3 lightPos = u_EyePos;
	vec3 lightDir = normalize(lightPos - v_PosW);
	
	float ambient = 0.6;
	float diff = max(dot(lightDir, v_Normal), 0.0);
	
	vec3 viewDir = normalize(u_EyePos - v_PosW);
	
	vec3 reflectDir = reflect(-lightDir, v_Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0); 

	gl_FragColor = vec4( (ambient + diff) * color  +  spec * u_LightColor, 1.0);
}
