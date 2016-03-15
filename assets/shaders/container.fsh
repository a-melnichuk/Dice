precision mediump float;

uniform sampler2D u_TextureUnit;
uniform vec3 u_EyePos;
uniform vec3 u_LightColor;
 
varying vec3 v_Pos; 
varying vec3 v_Normal; 
varying vec2 v_TexCoord; 

void main() {
	vec3 color = texture2D(u_TextureUnit, v_TexCoord).rgb;

	vec3 lightPos = u_EyePos;
	vec3 lightDir = normalize(lightPos - v_Pos);
	
	float ambient = 0.6;
	float diff = max(dot(lightDir, v_Normal), 0.0);

	vec3 viewDir = normalize(u_EyePos - v_Pos);
	
	vec3 reflectDir = reflect(-lightDir, v_Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
    vec3 lightColor = vec3(0.1);  

	//amplify color with quadratic equation
	gl_FragColor =  vec4( (ambient * color + diff) * color +  spec * u_LightColor , 1.0);
}