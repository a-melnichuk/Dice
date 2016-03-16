precision mediump float;

varying vec2 v_TexCoord; 

uniform sampler2D u_DepthMap;

float near_plane = 0.4;
float far_plane = 25.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{             
    float depthValue = texture2D(u_DepthMap, v_TexCoord).r;
    // gl_FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    gl_FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}