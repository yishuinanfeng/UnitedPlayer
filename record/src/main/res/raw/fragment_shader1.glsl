precision mediump float;
varying vec2 v_texPo;
uniform sampler2D sTexture;
void main() {
    gl_FragColor = vec4(vec3(1.0 - texture2D(sTexture, v_texPo)), 1.0);
}
