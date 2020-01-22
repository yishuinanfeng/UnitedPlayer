//
// Created by yanyinan on 2020/1/4.
//

#ifndef MANCHESTERUNITEDPLAYER_FRAGMENTSHADER_H
#define MANCHESTERUNITEDPLAYER_FRAGMENTSHADER_H


/**
 * 专门存放各种shader的文件
 */

//顶点着色器(static的意义?)
#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
        uniform float u_time;
        varying float time;
        attribute
        vec4 aPosition;//输入的顶点坐标
        attribute
        vec2 aTextCoord;//输入的纹理坐标
        varying
        vec2 vTextCoord;//输出的纹理坐标
        //缩放矩阵
        uniform
        mat4 uScaleMatrix;
        void main() {
            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）
            vTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);
            gl_Position = uScaleMatrix * aPosition;
            time = u_time;
        }
);
/**
 * 普通yuv420p shader
 */
static const char *fragYUV420P = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);
/**
 * yuv420p反色 shader
 */
static const char *fragYUV420POppositeColor = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            //gl_FragColor = vec4(rgb, 1.0);
            gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
        }
);

/**
 * yuv420p灰度shader
 */
static const char *fragYUV420PGray = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
            gl_FragColor = vec4(gray, gray, gray, 1.0);
        }
);

/**
 * yuv420p使用反色和灰度图轮播效果滤镜 shader
 */
static const char *fragYUV420POppoColorAndGray = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            float filterType = sin(time / 400.0);
            if (filterType > 0.0) {
                if (vTextCoord.x < 0.5 && vTextCoord.y < 0.5) {
                    //反色滤镜
                    gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
                } else if (vTextCoord.x > 0.5 && vTextCoord.y > 0.5) {
                    float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
                    gl_FragColor = vec4(gray, gray, gray, 1.0);
                } else {
                    gl_FragColor = vec4(rgb, 1.0);

                }
            } else {
                if (vTextCoord.x > 0.5 && vTextCoord.y < 0.5) {
                    //反色滤镜
                    gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
                } else if (vTextCoord.x < 0.5 && vTextCoord.y > 0.5) {
                    float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
                    gl_FragColor = vec4(gray, gray, gray, 1.0);
                } else {
                    gl_FragColor = vec4(rgb, 1.0);

                }
            }


        }
);

/**
 * NV12 shader
 */
static const char *fragNV12 = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * NV12反色 shader
 */
static const char *fragNV12OppoColor = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
        }
);

/**
 * NV12灰度 shader
 */
static const char *fragNV12Gray = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
            gl_FragColor = vec4(gray, gray, gray, 1.0);
        }
);
/**
 * NV21 shader
 */
static const char *fragNV21 = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);


/**
 * NV21反色 shader
 */
static const char *fragNV21OppoColor = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
        }
);

/**
 * NV21灰度 shader
 */
static const char *fragNV21Gray = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
            gl_FragColor = vec4(gray, gray, gray, 1.0);
        }
);

/**
 * YUV420P闪白
 */
static const char *fragYUV420PSplash = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            //除以1.3是为了减小变化的幅度，使得不出现一段时间的全白色。减0.2是为了给原始颜色画面提供停留的时间
            //因为这里的time单位为毫秒，所以要除以一个比较大的数才可以保持一个肉眼可见的闪动效果
            float uAdditionalColor = abs(sin(time / 150.0))  - 0.3;
            if (uAdditionalColor < 0.0) {
                uAdditionalColor = 0.0;
            }
            gl_FragColor = vec4(rgb.r + uAdditionalColor, rgb.g + uAdditionalColor, rgb.b + uAdditionalColor, 1.0);
        }
);

/**
 * NV21闪白
 */
static const char *fragNV21Splash = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        varying float time;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            //除以1.3是为了减小变化的幅度，使得不出现一段时间的全白色。减0.2是为了给原始颜色画面提供停留的时间
            //因为这里的time单位为毫秒，所以要除以一个比较大的数才可以保持一个肉眼可见的闪动效果
            float uAdditionalColor = abs(sin(time / 150.0))  - 0.3;
            if (uAdditionalColor < 0.0) {
                uAdditionalColor = 0.0;
            }

            gl_FragColor = vec4(rgb.r + uAdditionalColor, rgb.g + uAdditionalColor, rgb.b + uAdditionalColor, 1.0);
        }
);


/**
 * NV12闪白
 */
static const char *fragNV12Splash = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        //修改这个值，可以控制曝光的程度
        // uniform float uAdditionalColor;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            //除以1.3是为了减小变化的幅度，使得不出现一段时间的全白色。减0.2是为了给原始颜色画面提供停留的时间
            //因为这里的time单位为毫秒，所以要除以一个比较大的数才可以保持一个肉眼可见的闪动效果
            float uAdditionalColor = abs(sin(time / 150.0))  - 0.3;
            if (uAdditionalColor < 0.0) {
                uAdditionalColor = 0.0;
            }

            gl_FragColor = vec4(rgb.r + uAdditionalColor, rgb.g + uAdditionalColor, rgb.b + uAdditionalColor, 1.0);
        }
);

/**
 * YUV420P缩放
 */
static const char *fragYUV420PScale = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * NV21缩放
 */
static const char *fragNV21Scale = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        varying float time;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(rgb, 1.0);
        }
);


/**
 * NV12缩放
 */
static const char *fragNV12Scale = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        //修改这个值，可以控制曝光的程度
        // uniform float uAdditionalColor;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(rgb, 1.0);
        }
);


#endif //MANCHESTERUNITEDPLAYER_FRAGMENTSHADER_H
