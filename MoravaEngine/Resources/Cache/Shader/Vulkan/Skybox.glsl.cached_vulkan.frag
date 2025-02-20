#  
                   GLSL.std.450              	       main    
                     
    Resources/Shaders/Skybox.glsl    k    �     #version 450 core

layout(location = 0) out vec4 finalColor;
// layout(location = 1) out vec4 o_Bloom;

layout (binding = 1) uniform samplerCube u_Texture;

layout (push_constant) uniform Uniforms
{
	float TextureLod;
} u_Uniforms;

layout (location = 0) in vec3 v_Position;

void main()
{
	finalColor = textureLod(u_Texture, v_Position, u_Uniforms.TextureLod);
	// o_Bloom = vec4(0.0);
}
     
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   finalColor       u_Texture        v_Position       Uniforms             TextureLod       u_Uniforms  J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G  
          G     "       G     !      G            H         #       G             !                               	         ;  	   
       	                                                  ;                                   ;                         	      ;        	               +                  	      6               �                 =           =           A              =           X                    >  
      �  8  