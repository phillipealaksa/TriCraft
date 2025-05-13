#version 440 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D FaceTexture;

void main()
{
    FragColor = texture(FaceTexture, TexCoord);
 //   vec4 col = vec4(1.0, 0.0, 0.0, 1.0);
  //.  FragColor = col;
}