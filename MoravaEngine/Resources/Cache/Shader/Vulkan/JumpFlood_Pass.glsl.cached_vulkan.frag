#  
  �                 GLSL.std.450                     main       T   �                Resources/Shaders/JumpFlood_Pass.glsl        �     #version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
	vec2 TexCoords;
    vec2 TexelSize;
    vec2 UV[9];
};
layout (location = 0) in VertexOutput Input;

layout(binding = 0) uniform sampler2D u_Texture;

float ScreenDistance(vec2 v)
{
    float ratio = Input.TexelSize.x / Input.TexelSize.y;
    v.x /= ratio;
    return dot(v, v);
}

void BoundsCheck(inout vec2 xy, vec2 uv)
{
    if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
        xy = vec2(1000.0f);
}

void main()
{
    vec4 pixel = texture(u_Texture, Input.UV[0]);

    for (int j = 1; j <= 8; j++)
    {
        // Sample neighbouring pixel and make sure it's
        // on the same side as us
        vec4 n = texture(u_Texture, Input.UV[j]);
        if (n.w != pixel.w)
            n.xyz = vec3(0.0f);

        n.xy += Input.UV[j] - Input.UV[0];

        // Invalidate out of bounds neighbours
        BoundsCheck(n.xy, Input.UV[j]);

        float dist = ScreenDistance(n.xy);
        if (dist < pixel.z)
            pixel.xyz = vec3(n.xy, dist);
    }

    o_Color = pixel;
}    
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         ScreenDistance(vf2;      v        BoundsCheck(vf2;vf2;         xy       uv       ratio        VertexOutput             TexCoords           TexelSize           UV       Input     P   pixel     T   u_Texture     ]   j     f   n     �   param     �   param     �   dist      �   param     �   o_Color J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G            G  T   "       G  T   !       G  �               !                               	         !  
      	   !        	   	                            +        	                                        ;                       +           +                        +     !        /   +     2       +     9     �?+     L     zD,     M   L   L     N            O      N    	 Q                              R   Q      S       R   ;  S   T       +     V      +     W          X            \         +     d      +     l        t         ,  t   u   2   2   2   +     �         �      N   ;  �   �      6               �     ;  O   P      ;  \   ]      ;  O   f      ;  	   �      ;  	   �      ;     �      ;  	   �                  =  R   U   T   A  X   Y      V   W   =     Z   Y   W  N   [   U   Z   >  P   [                >  ]      �  ^   �  ^   �  `   a       �  b   �  b   =     c   ]   �  /   e   c   d   �  e   _   `   �  _        $       =  R   g   T   =     h   ]   A  X   i      V   h   =     j   i   W  N   k   g   j   >  f   k        %       A     m   f   l   =     n   m   A     o   P   l   =     p   o   �  /   q   n   p   �  s       �  q   r   s   �  r        &       =  N   v   f   O 	 N   w   v   u               >  f   w   �  s   �  s        (       =     x   ]   A  X   y      V   x   =     z   y   A  X   {      V   W   =     |   {   �     }   z   |   =  N   ~   f   O        ~   ~          �     �      }   =  N   �   f   O 	 N   �   �   �               >  f   �        +       =     �   ]   =  N   �   f   O     �   �   �          >  �   �   A  X   �      V   �   =     �   �   >  �   �   9     �      �   �   =     �   �   =  N   �   f   O 	 N   �   �   �               >  f   �        -       =  N   �   f   O     �   �   �          >  �   �   9     �      �   >  �   �        .       =     �   �   A     �   P   �   =     �   �   �  /   �   �   �   �  �       �  �   �   �   �  �        /       =  N   �   f   O     �   �   �          =     �   �   Q     �   �       Q     �   �      P  t   �   �   �   �   =  N   �   P   O 	 N   �   �   �               >  P   �   �  �   �  �   �  a   �  a                =     �   ]   �     �   �      >  ]   �   �  ^   �  `        2       =  N   �   P   >  �   �   �  8  6            
   7  	      �     ;                       A                 =            A     "         !   =     #   "   �     $       #   >     $               =     %      A     &         =     '   &   �     (   '   %   A     )         >  )   (               =     *      =     +      �     ,   *   +   �  ,   8  6               7  	      7  	      �                 A     0         =     1   0   �  /   3   1   2   �  /   4   3   �  6       �  4   5   6   �  5   A     7         =     8   7   �  /   :   8   9   �  6   �  6   �  /   ;   3      :   5   �  /   <   ;   �  >       �  <   =   >   �  =   A     ?      !   =     @   ?   �  /   A   @   2   �  >   �  >   �  /   B   ;   6   A   =   �  /   C   B   �  E       �  C   D   E   �  D   A     F      !   =     G   F   �  /   H   G   9   �  E   �  E   �  /   I   B   >   H   D   �  K       �  I   J   K   �  J               >     M   �  K   �  K   �  8  