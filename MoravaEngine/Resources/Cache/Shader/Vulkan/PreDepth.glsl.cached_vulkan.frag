#  
                   GLSL.std.450                     main    
               
    Resources/Shaders/PreDepth.glsl  <    �     #version 450 core

layout(location = 0) out vec4 o_LinearDepth;

layout(location = 0) in float LinearDepth;

void main()
{
	// TODO: Check for alpha in texture
	o_LinearDepth = vec4(LinearDepth, 0.0, 0.0, 1.0);
}   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   o_LinearDepth        LinearDepth J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G  
          G                 !                               	         ;  	   
                  ;           +            +          �?6               �          
       =           P                    >  
      �  8  