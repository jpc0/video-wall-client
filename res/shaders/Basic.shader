R"(#shader vertex
#version 120

attribute vec2 position;
attribute vec2 texCoord;

varying vec2 v_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Proj;
uniform mat4 u_View;

void main()
{
    gl_Position = u_MVP * u_Proj * u_View * vec4(position.xy, 1, 1);
    v_TexCoord = texCoord;
}

#shader fragment
#version 120

uniform sampler2D u_Texture;

uniform float u_brightness;
uniform float u_rbrightness;
uniform float u_gbrightness;
uniform float u_bbrightness;

uniform float u_contrast;
uniform float u_rcontrast;
uniform float u_gcontrast;
uniform float u_bcontrast;

uniform float u_gamma;
uniform float u_rgamma;
uniform float u_ggamma;
uniform float u_bgamma;


varying vec2 v_TexCoord;

void main()
{
    vec4 color = texture2D(u_Texture, v_TexCoord);

    color = clamp(
        ((u_contrast * (color -0.5)) + 0.5 + u_brightness),
        0.0,
        1.0);

    float contrast_r = 1;
    float brightness_r = 0;

    color.r = clamp(
        ((u_rcontrast * (color.r -0.5)) + 0.5 + u_rbrightness),
        0.0,
        1.0);

    float contrast_g = 1;
    float brightness_g = 0;

    color.g = clamp(
        ((u_gcontrast * (color.g -0.5)) + 0.5 + u_gbrightness),
        0.0,
        1.0);
        
    float contrast_b = 1;
    float brightness_b = 0;

    color.b = clamp(
        ((u_bcontrast * (color.b -0.5)) + 0.5 + u_bbrightness),
        0.0,
        1.0);


    color = pow(color, 1/vec4(
        u_gamma*u_rgamma, 
        u_gamma*u_ggamma, 
        u_gamma*u_bgamma, 1));

    gl_FragColor = color;
})"