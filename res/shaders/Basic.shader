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

varying vec2 v_TexCoord;

void main()
{
    gl_FragColor = texture2D(u_Texture, v_TexCoord);
})"