R"(#shader vertex
#version 120

attribute vec2 position;
attribute vec2 texCoord;

varying vec2 v_TexCoord;
varying vec3 lightDir;
varying vec3 outNormal;

uniform mat4 u_MVP;
uniform mat4 u_Proj;
uniform mat4 u_View;

void main()
{
    gl_Position = u_MVP * u_Proj * u_View * vec4(position.xy, 1, 1);
    gl_TexCoord[0].st = texCoord;
    v_TexCoord = texCoord;
    outNormal = normalize(gl_NormalMatrix * vec3(0,0,1));
    lightDir = normalize(vec3(gl_LightSource[0].position));
}

#shader fragment
#version 120

uniform sampler2D u_Texture;

varying vec2 v_TexCoord;
varying vec3 outNormal;
varying vec3 lightDir;

void main()
{
    vec3 cf;
    float intensity,af;
    vec4 texColor = texture2D(u_Texture, v_TexCoord);
    intensity = max(dot(lightDir,normalize(outNormal)),0.0);
    cf = intensity * (texColor).rgb;
    af = texColor.a;
    gl_FragColor = vec4(cf,af);
})"