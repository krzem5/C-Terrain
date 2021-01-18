#include <terrain.h>
#include <editor.h>
#include <renderer.h>
#include <noise.h>
#include <color_3d_pixel.h>
#include <color_3d_vertex.h>
#include <stdio.h>



#define TILE_SIZE 2
#define SMALL_SCALE 0.1f
#define SMALL_MLT 5
#define BIG_SCALE 0.001f
#define BIG_MLT 20



ID3D11VertexShader* t_vs=NULL;
ID3D11InputLayout* t_vs_il=NULL;
ID3D11PixelShader* t_ps=NULL;



Terrain terrain_init(uint16_t w,uint16_t h){
	if (t_vs==NULL||t_vs_il==NULL||t_ps==NULL){
		assert(t_vs==NULL);
		assert(t_vs_il==NULL);
		assert(t_ps==NULL);
		D3D11_INPUT_ELEMENT_DESC vs_i[]={
			{
				"SV_POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
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
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA
			}
		};
		t_vs=load_vertex_shader(g_color_3d_vs,sizeof(g_color_3d_vs),vs_i,sizeof(vs_i)/sizeof(D3D11_INPUT_ELEMENT_DESC),&t_vs_il);
		t_ps=load_pixel_shader(g_color_3d_ps,sizeof(g_color_3d_ps));
	}
	Terrain o=malloc(sizeof(struct __TERRAIN));
	o->w=w;
	o->h=h;
	o->ill=w*(h+1)*2;
	size_t vlsz=(w+1)*(h+1)*9;
	uint16_t* il=malloc(o->ill*sizeof(uint16_t));
	o->vl=malloc(vlsz*sizeof(float));
	size_t k=0;
	size_t l=0;
	for (uint16_t i=0;i<w+1;i++){
		for (uint16_t j=0;j<h+1;j++){
			*(o->vl+k)=(float)i*TILE_SIZE;
			*(o->vl+k+1)=(float)((uint8_t)(perlin_noise((float)i*SMALL_SCALE,(float)j*SMALL_SCALE)/255*SMALL_MLT)+(uint8_t)(perlin_noise((float)i*BIG_SCALE,(float)j*BIG_SCALE)/255*BIG_MLT));
			*(o->vl+k+2)=(float)j*TILE_SIZE;
			*(o->vl+k+3)=0;
			*(o->vl+k+4)=(float)(k/9)/((w+1)*(h+1));
			*(o->vl+k+5)=1-(float)(k/9)/((w+1)*(h+1));
			if (i<w&&j<h){
				if (j==0){
					*(il+l)=(uint16_t)(k/9+h+1);
					l++;
				}
				*(il+l)=(uint16_t)(k/9+h+1);
				*(il+l+1)=(uint16_t)(k/9);
				l+=2;
			}
			else if (i!=w&&j==h){
				*(il+l)=*(il+l-1);
				l++;
			}
			k+=9;
		}
	}
	terrain_recalc_normals(o);
	D3D11_BUFFER_DESC bd={
		(uint32_t)(o->ill*sizeof(uint16_t)),
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
	HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&(o->ib));
	bd.ByteWidth=(uint32_t)(vlsz*sizeof(float));
	bd.Usage=D3D11_USAGE_DYNAMIC;
	bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	dt.pSysMem=o->vl;
	hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&(o->vb));
	free(il);
	return o;
}



void terrain_recalc_normals(Terrain t){
	struct _TEMP{
		float x0;
		float y0;
		float z0;
		float x1;
		float y1;
		float z1;
	};
	struct _TEMP* n=malloc(t->w*t->h*sizeof(struct _TEMP));
	for (uint16_t i=0;i<t->w;i++){
		for (uint16_t j=0;j<t->h;j++){
			uint32_t i1=i*(t->h+1)+j;
			uint32_t i2=i*(t->h+1)+j+1;
			uint32_t i3=(i+1)*(t->h+1)+j;
			n[i*t->h+j].x0=((t->vl[i1*9+1]-t->vl[i3*9+1])*(t->vl[i3*9+2]-t->vl[i2*9+2]))-((t->vl[i1*9+2]-t->vl[i3*9+2])*(t->vl[i3*9+1]-t->vl[i2*9+1]));
			n[i*t->h+j].y0=((t->vl[i1*9+2]-t->vl[i3*9+2])*t->vl[i3*9+0]-t->vl[i2*9+0])-((t->vl[i1*9+0]-t->vl[i3*9+0])*(t->vl[i3*9+2]-t->vl[i2*9+2]));
			n[i*t->h+j].z0=((t->vl[i1*9+0]-t->vl[i3*9+0])*(t->vl[i3*9+1]-t->vl[i2*9+1]))-((t->vl[i1*9+1]-t->vl[i3*9+1])*t->vl[i3*9+0]-t->vl[i2*9+0]);
		}
	}
	for (uint16_t i=0;i<t->w+1;i++){
		for (uint16_t j=0;j<t->h+1;j++){
			float x=0;
			float y=0;
			float z=0;
			uint8_t c=0;
			if (i&&j){
				x+=n[(i-1)*t->h+j-1].x0;
				y+=n[(i-1)*t->h+j-1].y0;
				z+=n[(i-1)*t->h+j-1].z0;
				c++;
			}
			if (i&&j<t->h){
				x+=n[(i-1)*t->h+j+1].x0;
				y+=n[(i-1)*t->h+j+1].y0;
				z+=n[(i-1)*t->h+j+1].z0;
				c++;
			}
			if (i<t->w&&j){
				x+=n[(i+1)*t->h+j-1].x0;
				y+=n[(i+1)*t->h+j-1].y0;
				z+=n[(i+1)*t->h+j-1].z0;
				c++;
			}
			if (i<t->w&&j<t->h){
				x+=n[(i+1)*t->h+j+1].x0;
				y+=n[(i+1)*t->h+j+1].y0;
				z+=n[(i+1)*t->h+j+1].z0;
				c++;
			}
			x/=c;
			y/=c;
			z/=c;
			float m=sqrtf(x*x+y*y+z*z);
			*(t->vl+(i*(t->h+1)+j)*9+6)=x/m;
			*(t->vl+(i*(t->h+1)+j)*9+7)=y/m;
			*(t->vl+(i*(t->h+1)+j)*9+8)=z/m;
		}
	}
	free(n);
}



void terrain_draw(Terrain t){
	ID3D11DeviceContext_IASetInputLayout(renderer_d3_dc,t_vs_il);
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,t_vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,t_ps,NULL,0);
	unsigned int off=0;
	unsigned int st=9*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&(t->vb),&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,t->ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,t->ill,0,0);
}
