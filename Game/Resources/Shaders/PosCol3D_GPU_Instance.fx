float4x4 gWorldViewProj : WorldViewProjection;

// ------------------------
// Input/Output structs
// ------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 Uv : TEXCOORD;
	float3 InstancePosition : INSTANCEPOS;
	float4 InstanceLengthVOne : INSTANCELENGTHVONE;
	float4 InstanceLengthVTwo : INSTANCELENGTHVTWO;
};
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;			// SV = system variable
	float3 Color : COLOR;
	float2 Uv : TEXCOORD;
	float3 InstancePosition : INSTANCEPOS;
	float4 InstanceLengthVOne : INSTANCELENGTHVONE;
	float4 InstanceLengthVTwo : INSTANCELENGTHVTWO;
};

// ------------------------
// Vertex shader
// ------------------------
VS_OUTPUT VS(VS_INPUT input, float3 instancePos : INSTANCEPOS, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	//float4 newPos = mul(float4(input.Position, 1.0f), gWorldViewProj);
	//newPos = newPos + float4(input.InstancePosition, 0);
	bool isEndVertexSpline = input.Position.x;
	input.Position.x = 0.f;
	float3 newPos3 = input.Position + input.InstancePosition;

	if (isEndVertexSpline && input.InstanceLengthVTwo.x == 1.f)
		newPos3.x += input.InstanceLengthVTwo.w;
	else if (isEndVertexSpline && input.InstanceLengthVTwo.y == 1.f)
		newPos3.y += input.InstanceLengthVTwo.w;
	else if (isEndVertexSpline && input.InstanceLengthVTwo.z == 1.f)
		newPos3.z += input.InstanceLengthVTwo.w;

	float4 newPos = mul(float4(newPos3, 1.0f), gWorldViewProj);
	
	output.Position = newPos;
	output.InstancePosition = newPos;
	output.Color = input.Color;
		
	return output;
}

// ------------------------
// Pixel shader
// ------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.Color, 1.f);
}

// ------------------------
// Technique
// ------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS()) );
	}
}