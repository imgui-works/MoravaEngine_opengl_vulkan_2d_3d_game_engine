#  
  ?                 GLSL.std.450                      main                %   )   /   6        Resources/Shaders/Renderer2D_Circle.glsl     �    �     #version 430 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in float a_Thickness;
layout(location = 2) in vec2 a_LocalPosition;
layout(location = 3) in vec4 a_Color;

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
	vec2 LocalPosition;
	float Thickness;
	vec4 Color;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.LocalPosition = a_LocalPosition;
	Output.Thickness = a_Thickness;
	Output.Color = a_Color;
	gl_Position = u_ViewProjection * u_Renderer.Transform * vec4(a_WorldPosition, 1.0);
}

  
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   VertexOutput      
       LocalPosition     
      Thickness     
      Color        Output       a_LocalPosition      a_Thickness      a_Color   #   gl_PerVertex      #       gl_Position   #      gl_PointSize      #      gl_ClipDistance   %         '   Camera    '       u_ViewProjection      )         -   Transform     -       Transform     /   u_Renderer    6   a_WorldPosition J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G            G           G           G           H  #              H  #            H  #            G  #      H  '          H  '       #       H  '             G  '      G  )   "       G  )   !       H  -          H  -       #       H  -             G  -      G  6               !                              	           
         	            
   ;                       +                        ;                       +                       ;                       +                    	   ;                    	                 +      !        "      !     #   	      "      $      #   ;  $   %        &   	        '   &      (      '   ;  (   )         *      &     -   &      .   	   -   ;  .   /   	      0   	   &     4            5      4   ;  5   6      +     8     �?6               �                 =           A              >                    =           A              >                    =  	         A              >                     A  *   +   )      =  &   ,   +   A  0   1   /      =  &   2   1   �  &   3   ,   2   =  4   7   6   Q     9   7       Q     :   7      Q     ;   7      P  	   <   9   :   ;   8   �  	   =   3   <   A     >   %      >  >   =   �  8  