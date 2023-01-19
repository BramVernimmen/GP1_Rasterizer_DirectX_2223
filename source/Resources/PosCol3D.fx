//-----------------------------
//	Globals
//-----------------------------
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
SamplerState gSamplerState;

Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
float4x4 gWorldMatrix : World;
float4x4 gViewInverseMatrix : ViewInverse;

float gPi = 3.141592653589793f;
float gLightIntensity = 7.0f;
float gShininess = 25.0f;

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = false; //default
};

BlendState gBlendState
{
	BlendEnable[0] = false;
	//SrcBlend = src_alpha;
	//DestBlend = inv_src_alpha;
	//BlendOp = add;
	//SrcBlendAlpha = zero;
	//DestBlendAlpha = zero;
	//BlendOpAlpha = add;
	//RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = 1;
	DepthFunc = less;
	StencilEnable = false;

	//others are redundant because
	//	StencilEnable is FALSE
	//(for demo purposes only)
	//StencilReadMask = 0x0F;
	//StencilWriteMask = 0x0F;
	//
	//FrontFaceStencilFunc = always;
	//BackFaceStencilFunc = always;
	//
	//FrontFaceStencilDepthFail = keep;
	//BackFaceStencilDepthFail = keep;
	//
	//FrontFaceStencilPass = keep;
	//BackFaceStencilPass = keep;
	//
	//FrontFaceStencilFail = keep;
	//BackFaceStencilFail = keep;
};
// old code:
//{
//	Filter = MIN_MAG_MIP_POINT;
//	AddressU = Wrap;
//	AddressV = Wrap;
//};

//-----------------------------
//	Input/Output Structs
//-----------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	//float3 Color : COLOR;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	//float3 Color : COLOR;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float4 WorldPosition : COLOR;
};

//-----------------------------
//	Vertex Shader
//-----------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//output.Position = float4(input.Position, 1.f);
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	//output.Color = input.Color;
	output.UV = input.UV;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.WorldPosition = mul(float4(input.Position, 1.0f), gWorldMatrix);
	return output;
}
//-----------------------------
//	Shader Functions
//-----------------------------
float4 CalculateLambertDiffuse(float kd, float4 cd)
{
	return ((cd * kd) / gPi);
}

float4 CalculateSpecularPhong(float4 color, float ks, float exp, float3 l, float3 v, float3 n)
{
	float3 reflectVector = reflect(l, n);
	float angleViewReflect = saturate(dot(reflectVector, v));

	float phongValue = ks * pow(angleViewReflect, exp);

	return color * phongValue;
}

//-----------------------------
//	Pixel Shader
//-----------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//return float4(input.Color, 1.f);
	//return gDiffuseMap.Sample(gSamplerState, input.UV);

	float kd = 1.0f;
	float ks = 1.0f;

	float3 sampledNormal = input.Normal;
	//float3 sampledNormal = float3(1.0f, 1.0f, 1.0f);

	float3 binormal = normalize(cross(sampledNormal, input.Tangent));
	float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.0f), float4(binormal, 0.0f), float4(sampledNormal, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f));
	
	sampledNormal = gNormalMap.Sample(gSamplerState, input.UV).rgb;
	sampledNormal = 2.0f * sampledNormal - float3(1.0f, 1.0f, 1.0f);
	
	sampledNormal = normalize(mul(float4(sampledNormal, 0.0f), tangentSpaceAxis)).rgb;


	float observedArea = saturate(dot(sampledNormal, -gLightDirection));
	float3 observedAreaColor = float3(observedArea, observedArea, observedArea);


	float4 diffuse = CalculateLambertDiffuse(kd, gDiffuseMap.Sample(gSamplerState, input.UV));

	float exponent = gGlossinessMap.Sample(gSamplerState, input.UV).r * gShininess;
	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

	float4 specular = CalculateSpecularPhong(gSpecularMap.Sample(gSamplerState, input.UV), ks, exponent, gLightDirection, viewDirection, sampledNormal);

	return (diffuse * gLightIntensity + specular + float4(0.025f, 0.025f, 0.025f, 1.0f)) * observedArea;

	//return float4(observedAreaColor, 1.0f);
}

//-----------------------------
//	Technique
//-----------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader ( CompileShader( ps_5_0, PS() ) );
	}
}