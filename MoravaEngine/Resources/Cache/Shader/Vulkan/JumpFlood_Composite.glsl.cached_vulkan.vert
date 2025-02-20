#  
  %                 GLSL.std.450              	        main                     Resources/Shaders/JumpFlood_Composite.glsl   Q    �     #version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

struct VertexOutput
{
	vec2 TexCoords;
};
layout (location = 0) out VertexOutput Output;

void main()
{
    Output.TexCoords = a_TexCoords;

    gl_Position = vec4(a_Position, 1.0f);
}

  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      	   VertexOutput      	       TexCoords        Output       a_TexCoords      gl_PerVertex             gl_Position         gl_PointSize            gl_ClipDistance         gl_CullDistance               a_Position  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G            G           H                H              H              H              G        G                 !                              	         
      	   ;  
                     +                        ;                                               +                                                   ;                                  ;           +          �?   #         6               �                 =           A              >                    =           Q               Q               Q     !         P     "          !      A  #   $         >  $   "   �  8  