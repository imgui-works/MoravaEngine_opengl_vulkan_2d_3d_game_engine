#  
  >                 GLSL.std.450                      main             "   1   8   9   :   =    
    Resources/Shaders/PreDepth.glsl  �    �     #version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjectionMatrix;
	mat4 u_InverseViewProjectionMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
};

layout (push_constant) uniform Transform
{
	mat4 Transform;
} u_Renderer;

layout(location = 0) out float LinearDepth;

void main()
{
	vec4 worldPosition = u_Renderer.Transform * vec4(a_Position, 1.0);

	LinearDepth = -(u_ViewMatrix * worldPosition).z;

	gl_Position = u_ViewProjectionMatrix * worldPosition;
}

     
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   worldPosition        Transform            Transform        u_Renderer       a_Position       LinearDepth       Camera   	         u_ViewProjectionMatrix           u_InverseViewProjectionMatrix            u_ProjectionMatrix           u_ViewMatrix      "         /   gl_PerVertex      /       gl_Position   /      gl_PointSize      /      gl_ClipDistance   /      gl_CullDistance   1         8   a_Normal      9   a_Tangent     :   a_Binormal    =   a_TexCoord  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    H            H         #       H               G        G            G            H             H          #       H                H            H         #   @   H               H            H         #   �   H               H            H         #   �   H               G         G  "   "       G  "   !       H  /              H  /            H  /            H  /            G  /      G  8         G  9         G  :         G  =              !                               	                                  	      ;        	               +                  	                             ;           +          �?            ;                                !          ;  !   "      +     #         $           )           +  )   *      +  )   -        .      -     /         .   .      0      /   ;  0   1         6         ;     8      ;     9      ;     :        ;            <      ;   ;  <   =      6               �     ;  	   
                  A              =           =           Q               Q              Q              P                    �              >  
                  A  $   %   "   #   =     &   %   =     '   
   �     (   &   '   Q     +   (           ,   +   >     ,               A  $   2   "      =     3   2   =     4   
   �     5   3   4   A  6   7   1      >  7   5   �  8  