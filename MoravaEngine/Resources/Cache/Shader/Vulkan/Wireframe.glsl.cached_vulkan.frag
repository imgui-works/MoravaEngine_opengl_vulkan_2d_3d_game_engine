#  
                   GLSL.std.450                     main    
                   Resources/Shaders/Wireframe.glsl     <    �     #version 450 core

layout(location = 0) out vec4 color;

layout (push_constant) uniform Material
{
	layout (offset = 64) vec4 Color;
} u_MaterialUniforms;

void main()
{
	color = u_MaterialUniforms.Color;
}
     
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   color        Material             Color        u_MaterialUniforms  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G  
          H         #   @   G             !                               	         ;  	   
                    	      ;        	               +                  	      6               �                 A              =           >  
      �  8  