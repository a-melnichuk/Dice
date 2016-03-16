precision mediump float;

uniform sampler2D u_TextureUnit;
uniform sampler2D u_ShadowMapTex;
uniform vec3 u_EyePos;
uniform vec3 u_LightColor;
 
varying vec3 v_Pos; 
varying vec3 v_Normal; 
varying vec2 v_TexCoord; 
varying vec4 v_ShadowMapCoord;

void main() {
	vec3 color = texture2D(u_TextureUnit, v_TexCoord).rgb;

	vec3 lightPos = u_EyePos;
	vec3 lightDir = normalize(lightPos - v_Pos);
	
	float ambient = 0.4;
	float diff = max(dot(lightDir, v_Normal), 0.0);

	vec3 viewDir = normalize(u_EyePos - v_Pos);
	
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(v_Normal, halfwayDir), 0.0), 2.0);
    vec3 lightColor = vec3(0.1);  

    float closestDepth = texture2DProj(u_ShadowMapTex, v_ShadowMapCoord).r;
    float currentDepth = v_ShadowMapCoord.z/v_ShadowMapCoord.w;
    float bias = max(0.01 * (1.0 - dot(v_Normal, lightDir)), 0.003);
    float shadow = 1.0;
    
    if(currentDepth < 1.0 && closestDepth < currentDepth - bias)
    {
        shadow = 0.5;
    }
    
	//blended to a*c^2 + d*c for my visual pleasure
	gl_FragColor =  vec4( ( (ambient * color + diff) * color +  spec * u_LightColor) * shadow , 1.0);
}