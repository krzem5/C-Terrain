#pragma pack_matrix(row_major)



// static float3 LIGHT_DIR=float3(-0.4767312946227962,0.6674238124719146,-0.5720775535473555);
static float3 LIGHT_DIR=float3(0,-1,0);



cbuffer core:register(b0){
	matrix cm;
	matrix pm;
};



struct VS_OUT{
	float4 p:SV_POSITION;
	float4 c:COLOR;
	float3 n:NORMAL;
};



VS_OUT color_3d_vs(float3 p:SV_POSITION,float3 c:COLOR,float3 n:NORMAL){
	VS_OUT o={
		mul(mul(float4(p.x,p.y,p.z,1),cm),pm),
		float4(c.x,c.y,c.z,1),
		n
	};
	return o;
}



float map(float v,float aa,float ab,float ba,float bb){
	return (v-aa)/(ab-aa)*(bb-ba)+ba;
}



float4 color_3d_ps(float4 p:SV_POSITION,float4 c:COLOR,float3 n:NORMAL):SV_Target{
	return c*(length(n)?map(dot(n,LIGHT_DIR),-1,1,0.15,1.1):0);
}
