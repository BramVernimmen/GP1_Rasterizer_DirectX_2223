//-----------------------------
//	Globals
//-----------------------------
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
SamplerState gSamplerState;

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = false; //default
};

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;

	//others are redundant because
	//	StencilEnable is FALSE
	//(for demo purposes only)
	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
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
	//float3 Normal : NORMAL;
	//float3 Tangent : TANGENT;
	//float4 WorldPosition : COLOR;
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
	//output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	//output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	//output.WorldPosition = mul(float4(input.Position, 1.0f), gWorldMatrix);
	return output;
}

//-----------------------------
//	Pixel Shader
//-----------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//return float4(input.Color, 1.f);
	return gDiffuseMap.Sample(gSamplerState, input.UV);
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