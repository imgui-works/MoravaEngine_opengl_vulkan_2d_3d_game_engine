#  
  >                 GLSL.std.450                      main                   $   (   .   5        Resources/Shaders/Renderer2D_Text.glsl   �    �     #version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

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
};

layout (location = 0) out VertexOutput Output;
layout (location = 5) out flat float TexIndex;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * u_Renderer.Transform * vec4(a_Position, 1.0);
}

     
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   VertexOutput      
       Color     
      TexCoord         Output       a_Color      a_TexCoord       TexIndex         a_TexIndex    "   gl_PerVertex      "       gl_Position   "      gl_PointSize      "      gl_ClipDistance   "      gl_CullDistance   $         &   Camera    &       u_ViewProjection      (         ,   Transform     ,       Transform     .   u_Renderer    5   a_Position  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G            G           G           G        G           G           H  "              H  "            H  "            H  "            G  "      H  &          H  &       #       H  &             G  &      G  (   "       G  (   !       H  ,          H  ,       #       H  ,             G  ,      G  5               !                              	           
      	            
   ;                       +                        ;                       +                    	   ;                    	               ;                       ;                        +              !            "         !   !      #      "   ;  #   $        %           &   %      '      &   ;  '   (         )      %     ,   %      -   	   ,   ;  -   .   	      /   	   %     3            4      3   ;  4   5      +     7     �?6               �                 =           A              >                    =  	         A              >                    =           >                     A  )   *   (      =  %   +   *   A  /   0   .      =  %   1   0   �  %   2   +   1   =  3   6   5   Q     8   6       Q     9   6      Q     :   6      P     ;   8   9   :   7   �     <   2   ;   A     =   $      >  =   <   �  8  