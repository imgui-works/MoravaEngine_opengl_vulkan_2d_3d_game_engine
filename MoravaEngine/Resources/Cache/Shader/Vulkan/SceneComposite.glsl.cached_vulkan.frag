#  
  E                 GLSL.std.450              	       main             <                Resources/Shaders/SceneComposite.glsl       Β     #version 450 core

layout(location = 0) out vec4 o_Color;

struct OutputBlock
{
	vec2 TexCoord;
};

layout (location = 0) in OutputBlock Input;

layout (binding = 0) uniform sampler2D u_Texture;

layout(push_constant) uniform Uniforms
{
	float Exposure;
} u_Uniforms;

void main()
{
	const float gamma     = 2.2;
	const float pureWhite = 1.0;

	vec3 color = texture(u_Texture, Input.TexCoord).rgb * u_Uniforms.Exposure;
	// Reinhard tonemapping operator.
	// see: "Photographic Tone Reproduction for Digital Images", eq. 4
	float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);

	// Scale color by ratio of average luminances.
	vec3 mappedColor = (mappedLuminance / luminance) * color;

	// Gamma correction.
	o_Color = vec4(pow(mappedColor, vec3(1.0 / gamma)), 1.0);

	// Show over-exposed areas
	// if (o_Color.r > 1.0 || o_Color.g > 1.0 || o_Color.b > 1.0)
	// 	o_Color.rgb *= vec3(1.0, 0.25, 0.25);
}
   
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main      
   color        u_Texture        OutputBlock          TexCoord         Input        Uniforms             Exposure         u_Uniforms    $   luminance     +   mappedLuminance   5   mappedColor   <   o_Color J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    G     "       G     !       G            H         #       G        G  <               !                               	          	                                                  ;                                           ;                       +                                                 	      ;        	         	         #         +     &   Π³Y>+     '   Y7?+     (   έ=,     )   &   '   (   +     -     ?   ;         ;  ;   <      +     >   /Ίθ>,     ?   >   >   >   6               ψ     ;  	   
      ;  #   $      ;  #   +      ;  	   5                  =           A              =           W              O                        A               =     !            "      !   >  
   "               =     %   
        *   %   )   >  $   *               =     ,   $   =     .   $        /   .   -        0   -   /        1   ,   0   =     2   $        3   -   2        4   1   3   >  +   4               =     6   +   =     7   $        8   6   7   =     9   
        :   9   8   >  5   :        "       =     =   5        @         =   ?   Q     A   @       Q     B   @      Q     C   @      P     D   A   B   C   -   >  <   D   ύ  8  