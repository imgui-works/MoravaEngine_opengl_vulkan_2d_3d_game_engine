#  
  B                 GLSL.std.450                      main                       (   ,   2   9        Resources/Shaders/Renderer2D.glsl    �    �     #version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

layout (std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout (push_constant) uniform Transform
{
	mat4 Transform;
} u_Renderer;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 5) out flat float TexIndex;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	TexIndex = a_TexIndex;
	Output.TilingFactor = a_TilingFactor;
	gl_Position = u_ViewProjection * u_Renderer.Transform * vec4(a_Position, 1.0);
}

    
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   VertexOutput      
       Color     
      TexCoord      
      TilingFactor         Output       a_Color      a_TexCoord       TexIndex         a_TexIndex        a_TilingFactor    &   gl_PerVertex      &       gl_Position   &      gl_PointSize      &      gl_ClipDistance   &      gl_CullDistance   (         *   Camera    *       u_ViewProjection      ,         0   Transform     0       Transform     2   u_Renderer    9   a_Position  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G            G           G           G        G           G           G            H  &              H  &            H  &            H  &            G  &      H  *          H  *       #       H  *             G  *      G  ,   "       G  ,   !       H  0          H  0       #       H  0             G  0      G  9               !                              	           
      	               
   ;                       +                        ;                       +                    	   ;                    	               ;                       ;           +           ;              #           +  #   $        %      $     &         %   %      '      &   ;  '   (        )           *   )      +      *   ;  +   ,         -      )     0   )      1   	   0   ;  1   2   	      3   	   )     7            8      7   ;  8   9      +     ;     �?6               �                 =           A              >                     =  	         A              >             !       =           >             "       =     !       A     "         >  "   !        #       A  -   .   ,      =  )   /   .   A  3   4   2      =  )   5   4   �  )   6   /   5   =  7   :   9   Q     <   :       Q     =   :      Q     >   :      P     ?   <   =   >   ;   �     @   6   ?   A     A   (      >  A   @   �  8  