#include <terrain.h>
#include <editor.h>
#include <renderer.h>
#include <color_3d_pixel.h>
#include <color_3d_vertex.h>
#include <stdio.h>



#define WIDTH 20
#define HEIGHT 20



ID3D11VertexShader* vs=NULL;
ID3D11InputLayout* vs_il=NULL;
ID3D11PixelShader* ps=NULL;
int64_t g_sz[]={-10,-10,10,10};
ID3D11Buffer* g_ib=NULL;
size_t g_ib_l=0;
ID3D11Buffer* g_vb=NULL;
// ID3D11Buffer* t_ib=NULL;
// size_t t_ib_l=0;
// ID3D11Buffer* t_vb=NULL;



void terrain_init(void){
	D3D11_INPUT_ELEMENT_DESC vs_i[]={
		{
			"SV_POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA
		}
	};
	vs=load_vertex_shader(g_color_3d_vs,sizeof(g_color_3d_vs),vs_i,sizeof(vs_i)/sizeof(D3D11_INPUT_ELEMENT_DESC),&vs_il);
	ps=load_pixel_shader(g_color_3d_ps,sizeof(g_color_3d_ps));
	g_ib_l=(WIDTH+HEIGHT+2)*2;
	size_t vlsz=(WIDTH+HEIGHT+2)*14;
	uint16_t* il=malloc(g_ib_l*sizeof(uint16_t));
	float* vl=malloc(vlsz*sizeof(float));
	size_t j=0;
	uint16_t k=0;
	for (int64_t i=0;i<=WIDTH;i++){
		*(vl+j)=(float)i;
		*(vl+j+1)=0;
		*(vl+j+2)=0;
		*(vl+j+4)=0.1f;
		*(vl+j+5)=0.1f;
		*(vl+j+6)=0.1f;
		*(vl+j+7)=(float)i;
		*(vl+j+8)=0;
		*(vl+j+9)=HEIGHT;
		*(vl+j+11)=0.1f;
		*(vl+j+12)=0.1f;
		*(vl+j+13)=0.1f;
		*(il+k)=k;
		*(il+k+1)=k+1;
		j+=14;
		k+=2;
	}
	for (int64_t i=0;i<=HEIGHT;i++){
		*(vl+j)=0;
		*(vl+j+1)=0;
		*(vl+j+2)=(float)i;
		*(vl+j+4)=0.1f;
		*(vl+j+5)=0.1f;
		*(vl+j+6)=0.1f;
		*(vl+j+7)=WIDTH;
		*(vl+j+8)=0;
		*(vl+j+9)=(float)i;
		*(vl+j+11)=0.1f;
		*(vl+j+12)=0.1f;
		*(vl+j+13)=0.1f;
		*(il+k)=k;
		*(il+k+1)=k+1;
		j+=14;
		k+=2;
	}
	D3D11_BUFFER_DESC bd={
		(uint32_t)(g_ib_l*sizeof(uint16_t)),
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_INDEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		il,
		0,
		0
	};
	HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_ib);
	bd.ByteWidth=(uint32_t)(vlsz*sizeof(float));
	bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	dt.pSysMem=vl;
	hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_vb);
	free(il);
	free(vl);
}



void terrain_draw(void){
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,ps,NULL,0);
	unsigned int off=0;
	unsigned int st=7*sizeof(float);
	ID3D11DeviceContext_IASetInputLayout(renderer_d3_dc,vs_il);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&g_vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,g_ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,(uint32_t)g_ib_l,0,0);
	// ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&t_vb,&st,&off);
	// ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,t_ib,DXGI_FORMAT_R16_UINT,0);
	// ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,(uint32_t)t_ib_l,0,0);
}
