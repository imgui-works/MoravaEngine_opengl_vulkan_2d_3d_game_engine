#  
  '             2        GLSL.std.450                     main    4   �   �   ,  K  d  y  �  �  �  �  �                 Resources/Shaders/HazelPBR_Static.glsl   �
   �     #version 450 core

const float PI = 3.141592;
const float Epsilon = 0.00001;

const int LightCount = 1;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

struct Light {
	vec3 Direction;
	vec3 Radiance;
	float Multiplier;
};

struct VertexOutput
{
	vec3 WorldPosition;
    vec3 Normal;
	vec2 TexCoord;
	mat3 WorldNormals;
	mat3 WorldTransform;
	vec3 Binormal;
};

layout (location = 0) in VertexOutput Input;

layout(location = 0) out vec4 color;

layout (std140, binding = 1) uniform Environment
{
	Light lights;
	vec3 u_CameraPosition; // Offset = 32
};

// PBR texture inputs
layout (binding = 2) uniform sampler2D u_AlbedoTexture;
layout (binding = 3) uniform sampler2D u_NormalTexture;
layout (binding = 4) uniform sampler2D u_MetalnessTexture;
layout (binding = 5) uniform sampler2D u_RoughnessTexture;

// Environment maps
layout (set = 1, binding = 0) uniform samplerCube u_EnvRadianceTex;
layout (set = 1, binding = 1) uniform samplerCube u_EnvIrradianceTex;

// BRDF LUT
layout (set = 1, binding = 2) uniform sampler2D u_BRDFLUTTexture;

layout (push_constant) uniform Material
{
	layout (offset = 64) vec3 AlbedoColor;
	float Metalness;
	float Roughness;

	float EnvMapRotation;

	// Toggles
	float RadiancePrefilter;
	float AlbedoTexToggle;
	float NormalTexToggle;
	float MetalnessTexToggle;
	float RoughnessTexToggle;
} u_MaterialUniforms;

struct PBRParameters
{
	vec3 Albedo;
	vec3 Normal;
	float Metalness;
	float Roughness;

	vec3 View;
	float NdotV;
};

PBRParameters m_Params;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

// ---------------------------------------------------------------------------------------------------
// The following code (from Unreal Engine 4's paper) shows how to filter the environment map
// for different roughnesses. This is mean to be computed offline and stored in cube map mips,
// so turning this on online will cause poor performance
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N)
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );
	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;
	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 TangentX = normalize( cross( UpVector, N ) );
	vec3 TangentY = cross( N, TangentX );
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float TotalWeight = 0.0;

vec3 PrefilterEnvMap(float Roughness, vec3 R)
{
	vec3 N = R;
	vec3 V = R;
	vec3 PrefilteredColor = vec3(0.0);
	int NumSamples = 1024;
	for(int i = 0; i < NumSamples; i++)
	{
		vec2 Xi = Hammersley(i, NumSamples);
		vec3 H = ImportanceSampleGGX(Xi, Roughness, N);
		vec3 L = 2 * dot(V, H) * H - V;
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		if (NoL > 0)
		{
			// PrefilteredColor += texture(u_EnvRadianceTex, L).rgb * NoL;
			TotalWeight += NoL;
		}
	}
	return PrefilteredColor / TotalWeight;
}

// ---------------------------------------------------------------------------------------------------

vec3 RotateVectorAboutY(float angle, vec3 vec)
{
    angle = radians(angle);
    mat3x3 rotationMatrix ={vec3(cos(angle),0.0,sin(angle)),
                            vec3(0.0,1.0,0.0),
                            vec3(-sin(angle),0.0,cos(angle))};
    return rotationMatrix * vec;
}

vec3 Lighting(vec3 F0)
{
	vec3 result = vec3(0.0);
	for(int i = 0; i < LightCount; i++)
	{
		vec3 Li = lights.Direction;
		vec3 Lradiance = lights.Radiance * lights.Multiplier;
		vec3 Lh = normalize(Li + m_Params.View);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(m_Params.Normal, Li));
		float cosLh = max(0.0, dot(m_Params.Normal, Lh));

		vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, m_Params.View)));
		float D = ndfGGX(cosLh, m_Params.Roughness);
		float G = gaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

		vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
		vec3 diffuseBRDF = kd * m_Params.Albedo;

		// Cook-Torrance
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);

		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}
	return result;
}

vec3 LightingTemp(vec3 F0)
{
	vec3 result = vec3(0.0);
	for(int i = 0; i < LightCount; i++)
	{
		vec3 Li = lights.Direction; // vec3(-0.5, 0.5, 0.5); // 
		vec3 Lradiance = vec3(1.0, 1.0, 1.0) * 2.0; //  lights.Radiance * lights.Multiplier;
		vec3 Lh = normalize(Li + m_Params.View);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(m_Params.Normal, Li));
		float cosLh = max(0.0, dot(m_Params.Normal, Lh));

		vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, m_Params.View)));
		float D = ndfGGX(cosLh, m_Params.Roughness);
		float G = gaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

		vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
		vec3 diffuseBRDF = kd * m_Params.Albedo;

		// Cook-Torrance
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);

		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}
	return result;
}

vec3 IBL(vec3 F0, vec3 Lr)
{
	vec3 irradiance = texture(u_EnvIrradianceTex, m_Params.Normal).rgb;
	vec3 F = fresnelSchlickRoughness(F0, m_Params.NdotV, m_Params.Roughness);
	vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
	vec3 diffuseIBL = m_Params.Albedo * irradiance;

	int envRadianceTexLevels = textureQueryLevels(u_EnvRadianceTex);
	float NoV = clamp(m_Params.NdotV, 0.0, 1.0);
	vec3 R = 2.0 * dot(m_Params.View, m_Params.Normal) * m_Params.Normal - m_Params.View;
	vec3 specularIrradiance = textureLod(u_EnvRadianceTex, RotateVectorAboutY(u_MaterialUniforms.EnvMapRotation, Lr), m_Params.Roughness * envRadianceTexLevels).rgb;

	// Sample BRDF Lut, 1.0 - roughness for y-coord because texture was generated (in Sparky) for gloss model
	vec2 specularBRDF = texture(u_BRDFLUTTexture, vec2(m_Params.NdotV, 1.0 - m_Params.Roughness)).rg;
	vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

	return kd * diffuseIBL + specularIBL;
}

void main()
{
	//	Standard PBR inputs

	// Temporary (values are not updated through the uniform buffer)
	float u_MaterialUniforms_AlbedoTexToggle = 1.0;
	float u_MaterialUniforms_NormalTexToggle = 1.0;
	float u_MaterialUniforms_MetalnessTexToggle = 1.0;
	float u_MaterialUniforms_RoughnessTexToggle = 1.0;

	m_Params.Albedo = u_MaterialUniforms_AlbedoTexToggle > 0.5 ? texture(u_AlbedoTexture, Input.TexCoord).rgb : u_MaterialUniforms.AlbedoColor;
	m_Params.Metalness = u_MaterialUniforms_MetalnessTexToggle > 0.5 ? texture(u_MetalnessTexture, Input.TexCoord).r : u_MaterialUniforms.Metalness;
	m_Params.Roughness = u_MaterialUniforms_RoughnessTexToggle > 0.5 ?  texture(u_RoughnessTexture, Input.TexCoord).r : u_MaterialUniforms.Roughness;
	m_Params.Roughness = max(m_Params.Roughness, 0.05); // Minimum roughness of 0.05 to keep specular highlight

	// Normals (either from vertex or map)
	m_Params.Normal = normalize(Input.Normal);
	if (u_MaterialUniforms_NormalTexToggle > 0.5)
	{
		m_Params.Normal = normalize(2.0 * texture(u_NormalTexture, Input.TexCoord).rgb - 1.0);
		m_Params.Normal = normalize(Input.WorldNormals * m_Params.Normal);
	}

	m_Params.View = normalize(u_CameraPosition - Input.WorldPosition);
	m_Params.NdotV = max(dot(m_Params.Normal, m_Params.View), 0.0);

	// Specular reflection vector
	vec3 Lr = 2.0 * m_Params.NdotV * m_Params.Normal - m_Params.View;

	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, m_Params.Albedo, m_Params.Metalness);

	vec3 lightContribution = LightingTemp(F0);
	vec3 iblContribution = IBL(F0, Lr);

	// color = vec4(lightContribution, 1.0);
	color = vec4(lightContribution + iblContribution, 1.0);

	// color = vec4(Input.WorldPosition, 1.0);
	// color = texture(u_RoughnessTexture, Input.TexCoord);
	// color = vec4(F0, 1.0);
	// color = vec4(m_Params.View, 1.0);

	// vec3 albedo = texture(u_AlbedoTexture, Input.TexCoord).rgb;
	// color = vec4(albedo, 1);

	/**** BEGIN main() from VulkanWeekMesh ****/
	// m_Params.Albedo = texture(u_AlbedoTexture, Input.TexCoord).rgb;
	// 
	// // Normals (either from vertex or map)
	// m_Params.Normal = normalize(2.0 * texture(u_NormalTexture, Input.TexCoord).rgb - 1.0);
	// m_Params.Normal = normalize(Input.WorldNormals * m_Params.Normal);
	// 
	// float ambient = 0.2;
	// vec3 lightDir = vec3(-1.0, 1.0, 0.0);
	// float intensity = clamp(dot(lightDir, m_Params.Normal), ambient, 1.0);
	// 
	// color = vec4(m_Params.Albedo, 1.0);
	// color.rgb *= intensity;

	/**** END main() from VulkanWeekMesh ****/
}
    
 GL_GOOGLE_cpp_style_line_directive    GL_GOOGLE_include_directive      main         ndfGGX(f1;f1;     
   cosLh        roughness        gaSchlickG1(f1;f1;       cosTheta         k        gaSchlickGGX(f1;f1;f1;       cosLi        NdotV        roughness        fresnelSchlick(vf3;f1;       F0       cosTheta      #   fresnelSchlickRoughness(vf3;f1;f1;        F0    !   cosTheta      "   roughness    	 (   RotateVectorAboutY(f1;vf3;    &   angle     '   vec   ,   LightingTemp(vf3;     +   F0    1   IBL(vf3;vf3;      /   F0    0   Lr    4   TotalWeight   6   alpha     :   alphaSq   >   denom     Z   r     ]   k     c   param     e   param     h   param     j   param     �   rotationMatrix    �   result    �   i     �   Li    �   Light     �       Direction     �      Radiance      �      Multiplier    �   Environment   �       lights    �      u_CameraPosition      �         �   Lradiance     �   Lh    �   PBRParameters     �       Albedo    �      Normal    �      Metalness     �      Roughness     �      View      �      NdotV     �   m_Params      �   cosLi     �   cosLh     �   F     �   param     �   param     �   D     �   param     �   param     �   G     �   param     �   param     �   param     �   kd      diffuseBRDF   
  specularBRDF      (  irradiance    ,  u_EnvIrradianceTex    3  F     4  param     6  param     9  param     =  kd    E  diffuseIBL    J  envRadianceTexLevels      K  u_EnvRadianceTex      O  NoV   S  R     `  specularIrradiance    b  Material      b      AlbedoColor   b     Metalness     b     Roughness     b     EnvMapRotation    b     RadiancePrefilter     b     AlbedoTexToggle   b     NormalTexToggle   b     MetalnessTexToggle    b     RoughnessTexToggle    d  u_MaterialUniforms    e  param     i  param     u  specularBRDF      y  u_BRDFLUTTexture      �  specularIBL   �  u_MaterialUniforms_AlbedoTexToggle    �  u_MaterialUniforms_NormalTexToggle    �  u_MaterialUniforms_MetalnessTexToggle     �  u_MaterialUniforms_RoughnessTexToggle     �  u_AlbedoTexture   �  VertexOutput      �      WorldPosition     �     Normal    �     TexCoord      �     WorldNormals      �     WorldTransform    �     Binormal      �  Input     �  u_MetalnessTexture    �  u_RoughnessTexture    �  u_NormalTexture     Lr      F0      lightContribution       param       iblContribution     param       param       color   J entry-point main    J client vulkan100    J target-env spirv1.5 J target-env vulkan1.2    J entry-point main    H  �       #       H  �      #      H  �      #      H  �       #       H  �      #       G  �      G  �   "       G  �   !      G  ,  "      G  ,  !      G  K  "      G  K  !       H  b      #   @   H  b     #   L   H  b     #   P   H  b     #   T   H  b     #   X   H  b     #   \   H  b     #   `   H  b     #   d   H  b     #   h   G  b     G  y  "      G  y  !      G  �  "       G  �  !      G  �         G  �  "       G  �  !      G  �  "       G  �  !      G  �  "       G  �  !      G                !                             !  	            !                                        !              !                 !  %            !  *         !  .               3         ;  3   4      +     5       +     C     �?+     H   �I@+     a      A+     v     �@  �            �      �   ,     �   5   C   5   ,     �   5   5   5     �             �      �   +  �   �       +  �   �        �     �              �   �         �      �   ;  �   �         �         +     �      @,     �   �   �   �     �                        �      �   ;  �   �      +  �   �         �         +  �   �      +  �   �      +  �         +       ��'7+         �@ 	 )                             *  )     +      *  ;  +  ,        0        ;  +  K        b                                c  	   b  ;  c  d  	      f  	        s           t     s   	 v                             w  v     x      w  ;  x  y        �          +  �  �      +  �  �     +     �     ?;  x  �        �        s  �   �         �     �  ;  �  �        �     s     �  	      ;  x  �      ;  x  �      +     �  ��L=   �        ;  x  �         �     �   +       
�#=,                     0  ;         6               �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;     �     ;          ;          ;          ;          ;          ;          ;               �       >  4   5              >  �  C              >  �  C               >  �  C        !      >  �  C        #      =     �  �  �  �   �  �  �  �  �      �  �  �  �  �  �  =  w  �  �  A  �  �  �     =  s  �  �  W  0  �  �  �  O     �  �  �            >  �  �  �  �  �  �  A  �  �  d  �   =     �  �  >  �  �  �  �  �  �  =     �  �  A  �   �  �   �   >  �  �       $      =     �  �  �  �   �  �  �  �  �      �  �  �  �  �  �  =  w  �  �  A  �  �  �     =  s  �  �  W  0  �  �  �  Q     �  �      >  �  �  �  �  �  �  A  f  �  d  �   =     �  �  >  �  �  �  �  �  �  =     �  �  A  3   �  �      >  �  �       %      =     �  �  �  �   �  �  �  �  �      �  �  �  �  �  �  =  w  �  �  A  �  �  �     =  s  �  �  W  0  �  �  �  Q     �  �      >  �  �  �  �  �  �  A  f  �  d     =     �  �  >  �  �  �  �  �  �  =     �  �  A  3   �  �   �   >  �  �       &      A  3   �  �   �   =     �  �       �     (   �  �  A  3   �  �   �   >  �  �       )      A  �  �  �  �   =     �  �       �     E   �  A  �   �  �   �   >  �  �       *      =     �  �  �  �   �  �  �  �  �      �  �  �  �  �  �       ,      =  w  �  �  A  �  �  �     =  s  �  �  W  0  �  �  �  O     �  �  �            �     �  �  �   P     �  C   C   C   �     �  �  �       �     E   �  A  �   �  �   �   >  �  �       -      A  �  �  �  �   =  �   �  �  A  �   �  �   �   =     �  �  �     �  �  �       �     E   �  A  �   �  �   �   >  �  �  �  �  �  �       0      A  �   �  �   �   =     �  �  A  �  �  �  �   =     �  �  �     �  �  �       �     E   �  A  �   �  �   �   >  �  �       1      A  �   �  �   �   =     �  �  A  �   �  �   �   =     �  �  �     �  �  �       �     (   �  5   A  3      �   �   >     �       4      A  3     �   �   =         �       �     A  �     �   �   =         �           A  �     �   �   =     	    �     
    	  >    
       7      A  �     �   �   =         A  3     �      =         P                       .         >           9      =         >      9       ,     >           :      =         >      =         >      9       1       >           =      =          =     !    �     "     !  Q     #  "      Q     $  "     Q     %  "     P  0  &  #  $  %  C   >    &  �  8  6            	   7     
   7        �     ;     6      ;     :      ;     >           S       =     7      =     8      �     9   7   8   >  6   9        T       =     ;   6   =     <   6   �     =   ;   <   >  :   =        V       =     ?   
   =     @   
   �     A   ?   @   =     B   :   �     D   B   C   �     E   A   D   �     F   E   C   >  >   F        W       =     G   :   =     I   >   �     J   H   I   =     K   >   �     L   J   K   �     M   G   L   �  M   8  6            	   7        7        �          ]       =     P      =     Q      =     R      �     S   C   R   �     T   Q   S   =     U      �     V   T   U   �     W   P   V   �  W   8  6               7        7        7        �     ;     Z      ;     ]      ;     c      ;     e      ;     h      ;     j           c       =     [      �     \   [   C   >  Z   \        d       =     ^   Z   =     _   Z   �     `   ^   _   �     b   `   a   >  ]   b        e       =     d      >  c   d   =     f   ]   >  e   f   9     g      c   e   =     i      >  h   i   =     k   ]   >  j   k   9     l      h   j   �     m   g   l   �  m   8  6               7        7        �          �       =     p      =     q      P     r   C   C   C   �     s   r   q   =     t      �     u   C   t        w         u   v   �     x   s   w   �     y   p   x   �  y   8  6     #          7         7     !   7     "   �  $        �       =     |       =     }   "   �     ~   C   }   P        ~   ~   ~   =     �            �      (      �   =     �       �     �   �   �   =     �   !   �     �   C   �        �         �   v   �     �   �   �   �     �   |   �   �  �   8  6     (       %   7     &   7     '   �  )   ;  �   �           �       =     �   &        �         �   >  &   �        �       =     �   &        �         �   =     �   &        �         �   P     �   �   5   �        �       =     �   &        �         �        �   �   =     �   &        �         �   P     �   �   5   �   Q     �   �       Q     �   �      Q     �   �      Q     �   �       Q     �   �      Q     �   �      Q     �   �       Q     �   �      Q     �   �      P     �   �   �   �   P     �   �   �   �   P     �   �   �   �   P  �   �   �   �   �   >  �   �        �       =  �   �   �   =     �   '   �     �   �   �   �  �   8  6     ,       *   7     +   �  -   ;     �      ;  �   �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;     �      ;          ;     
          �       >  �   �        �       >  �   �   �  �   �  �   �  �   �       �  �   �  �   =  �   �   �   �  �   �   �   �   �  �   �   �   �  �        �       A  �   �   �   �   �   =     �   �   >  �   �        �       >  �   �        �       =     �   �   A  �   �   �   �   =     �   �   �     �   �   �        �      E   �   >  �   �        �       A  �   �   �   �   =     �   �   =     �   �   �     �   �   �        �      (   5   �   >  �   �        �       A  �   �   �   �   =     �   �   =     �   �   �     �   �   �        �      (   5   �   >  �   �        �       =     �   �   A  �   �   �   �   =     �   �   �     �   �   �        �      (   5   �   =     �   +   >  �   �   >  �   �   9     �      �   �   >  �   �        �       =     �   �   >  �   �   A  3   �   �   �   =     �   �   >  �   �   9     �      �   �   >  �   �        �       =     �   �   >  �   �   A  3   �   �   �   =     �   �   >  �   �   A  3   �   �   �   =     �   �   >  �   �   9     �      �   �   �   >  �   �        �       =     �   �   P     �   C   C   C   �     �   �   �   A  3     �      =         �       C     �       �     >  �          �       =       �   A  �     �   �   =         �     	      >    	       �       =       �   =       �   �           =       �   �           =       �   �           A  3     �   �   =         �                     (       P             �           >  
               =         =       
  �           =       �   �           =       �   �            =     !  �   �     "  !     >  �   "  �  �   �  �        �       =  �   #  �   �  �   $  #  �   >  �   $  �  �   �  �              =     %  �   �  %  8  6     1       .   7     /   7     0   �  2   ;     (     ;     3     ;     4     ;     6     ;     9     ;     =     ;     E     ;  �   J     ;     O     ;     S     ;     `     ;     e     ;     i     ;  t  u     ;     �                =  *  -  ,  A  �   .  �   �   =     /  .  W  0  1  -  /  O     2  1  1            >  (  2       	      =     5  /   >  4  5  A  3   7  �   �   =     8  7  >  6  8  A  3   :  �   �   =     ;  :  >  9  ;  9     <  #   4  6  9  >  3  <       
      =     >  3  P     ?  C   C   C   �     @  ?  >  A  3   A  �      =     B  A  �     C  C   B  �     D  @  C  >  =  D             A  �   F  �   �   =     G  F  =     H  (  �     I  G  H  >  E  I             =  *  L  K  d  )  M  L  j  �   N  M  >  J  N             A  3   P  �   �   =     Q  P       R     +   Q  5   C   >  O  R             A  �   T  �   �   =     U  T  A  �   V  �   �   =     W  V  �     X  U  W  �     Y  �   X  A  �   Z  �   �   =     [  Z  �     \  [  Y  A  �   ]  �   �   =     ^  ]  �     _  \  ^  >  S  _             =  *  a  K  A  f  g  d  �   =     h  g  >  e  h  =     j  0   >  i  j  9     k  (   e  i  A  3   l  �   �   =     m  l  =  �   n  J  o     o  n  �     p  m  o  X  0  q  a  k     p  O     r  q  q            >  `  r             =  w  z  y  A  3   {  �   �   =     |  {  A  3   }  �   �   =     ~  }  �       C   ~  P  s  �  |    W  0  �  z  �  O  s  �  �  �         >  u  �             =     �  `  =     �  /   A     �  u  �  =     �  �  �     �  �  �  A     �  u  �  =     �  �  P     �  �  �  �  �     �  �  �  �     �  �  �  >  �  �             =     �  =  =     �  E  �     �  �  �  =     �  �  �     �  �  �  �  �  8  