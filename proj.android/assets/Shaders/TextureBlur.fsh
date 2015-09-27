#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec2 pixelSize;
uniform vec2 direction;
uniform int radius;
uniform float weights[64];
																			      
void main()
{
	gl_FragColor = texture2D(CC_Texture0, v_texCoord)*weights[0];
	vec2 offsetStep = pixelSize*direction;
    for (int i = 1; i < radius; i++)
	{
		vec2 offset = float(i)*offsetStep;
		gl_FragColor += texture2D(CC_Texture0, v_texCoord + offset)*weights[i];
		gl_FragColor += texture2D(CC_Texture0, v_texCoord - offset)*weights[i];
    }
}