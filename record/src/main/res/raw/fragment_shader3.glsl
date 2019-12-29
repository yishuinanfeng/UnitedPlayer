precision mediump float;
varying vec2 v_texPo;
uniform sampler2D sTexture;
void main() {
    lowp vec4 textureColor = texture2D(sTexture, v_texPo);
    gl_FragColor = vec4((textureColor.rgb + vec3(-0.5f)), textureColor.w);
}
