precision mediump float;
varying vec4 v_PosLightSpace;

void main(void)
{
    gl_FragColor = vec4(v_PosLightSpace.z/v_PosLightSpace.w, 0.0, 0.0, 1.0);
}