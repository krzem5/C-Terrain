#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	float4 cd;
	matrix cm;
	matrix pm;
};



struct VS_OUT{
	float4 p:SV_POSITION;
	float4 c:COLOR;
};



VS_OUT color_3d_vs(float3 p:SV_POSITION,float3 c:COLOR){
	VS_OUT o={
		mul(mul(float4(p.x,p.y,p.z,1),cm),pm),
		float4(c.x,c.y,c.z,1)
	};
	return o;
}



float4 color_3d_ps(float4 p:SV_POSITION,float4 c:COLOR):SV_Target{
	return c;
}
