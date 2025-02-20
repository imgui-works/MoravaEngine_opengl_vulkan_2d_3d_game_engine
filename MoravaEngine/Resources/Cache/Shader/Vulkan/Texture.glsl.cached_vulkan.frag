#  
                   GLSL.std.450                     main    
                  	    assets/shaders/Texture.glsl  E    �     #version 450 core

layout(location = 0) out vec4 o_Color;

struct OutputBlock
{
	vec2 TexCoord;
};

layout (location = 0) in OutputBlock Input;

layout (binding = 0) uniform sampler2D u_Texture;

void main()
{
	o_Color = texture(u_Texture, Input.TexCoord);
}   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   o_Color      u_Texture        OutputBlock          TexCoord         Input   J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G  
          G     "       G     !       G                 !                               	         ;  	   
       	                                                  ;                                           ;                       +                        6               �                 =           A              =           W              >  
      �  8  