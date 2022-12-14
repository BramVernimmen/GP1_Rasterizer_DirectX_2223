//-----------------------------
//	Globals
//-----------------------------
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
SamplerState gSamplerState;
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
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	//float3 Color : COLOR;
	float2 UV : TEXCOORD;
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
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader ( CompileShader( ps_5_0, PS() ) );
	}
}