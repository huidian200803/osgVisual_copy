uniform sampler2D textureImage;
uniform sampler2D textureDistort;
uniform sampler2D textureBlend;

void main()
{
    vec4 texCoord;
    vec4 distortColor;
    vec4 blendColor;

    distortColor = texture2D(textureDistort, gl_TexCoord[0].st);
    blendColor = texture2D(textureBlend, gl_TexCoord[0].st);

// modified 
//    texCoord.s = (distortColor.b / 255.0 + mod(distortColor.r , 16.0)) / 16.0;
//    texCoord.t = 1.0 - (distortColor.g / 255.0 + (distortColor.r / 16.0)) / 16.0;

// Original
texCoord.s = (distortColor.b + mod(floor(distortColor.r * 255.5), 16.0)) / 16.0;
texCoord.t = 1.0 - (distortColor.g + floor(distortColor.r * 255.5 / 16.0)) / 16.0;

    gl_FragColor = texture2D(textureImage, texCoord.st) * blendColor;
}


