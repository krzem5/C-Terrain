#include <editor.h>
#include <terrain.h>
#include <renderer.h>
#include <stdio.h>



#define EDITOR_BG_COLOR_R 0.91f
#define EDITOR_BG_COLOR_G 0.91f
#define EDITOR_BG_COLOR_B 0.91f



typedef struct _CAMERA* Camera;



struct _CAMERA{
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float zm;
	float ms;
	float rs;
	bool enabled;
	uint16_t _f;
	int16_t _rx;
	int16_t _ry;
	int16_t _px;
	int16_t _py;
	float _ox;
	float _oy;
	float _oz;
};




Camera create_camera(float ms,float rs,float x,float y,float z,float rx,float ry,float rz,float zm){
	Camera o=malloc(sizeof(struct _CAMERA));
	o->x=x;
	o->y=y;
	o->z=z;
	o->rx=(rx-90)*PI_DIV_180;
	o->ry=ry*PI_DIV_180;
	o->rz=rz*PI_DIV_180;
	o->zm=zm;
	o->ms=ms;
	o->rs=rs;
	o->enabled=false;
	o->_f=0;
	o->_rx=0;
	o->_ry=0;
	o->_px=0;
	o->_py=0;
	o->_ox=0;
	o->_oy=0;
	o->_oz=0;
	return o;
}



Matrix update_camera(Camera c,float dt){
	if (c->enabled==true){
		bool rc=false;
		if ((renderer_mf&M_RIGHT)!=0){
			rc=true;
			if ((c->_f&M_RIGHT)==0){
				c->_f|=M_RIGHT;
				c->_rx=renderer_mx;
				c->_ry=renderer_my;
			}
			c->rx-=(c->_ry-renderer_my)*c->rs*dt*PI_DIV_180;
			c->ry-=(c->_rx-renderer_mx)*c->rs*dt*PI_DIV_180;
			if (c->rx>PI_DIV_TWO-EPSILON){
				c->rx=PI_DIV_TWO-EPSILON;
			}
			else if (c->rx<-PI_DIV_TWO+EPSILON){
				c->rx=-PI_DIV_TWO+EPSILON;
			}
			c->_rx=renderer_mx;
			c->_ry=renderer_my;
		}
		else{
			c->_f&=(~M_RIGHT);
		}
		float dx=cosf(c->ry)*c->ms*dt;
		float dz=sinf(c->ry)*c->ms*dt;
		if (is_pressed(0x57)){
			rc=true;
			c->_ox+=dx;
			c->_oz+=dz;
		}
		if (is_pressed(0x53)){
			rc=true;
			c->_ox-=dx;
			c->_oz-=dz;
		}
		if (is_pressed(0x41)){
			rc=true;
			c->_ox+=dz;
			c->_oz-=dx;
		}
		if (is_pressed(0x44)){
			rc=true;
			c->_ox-=dz;
			c->_oz+=dx;
		}
		if (is_pressed(VK_SPACE)){
			rc=true;
			c->_oy+=c->ms*dt;
		}
		if (is_pressed(VK_LSHIFT)){
			rc=true;
			c->_oy-=c->ms*dt;
		}
		if (rc==true){
			c->x=c->_ox+c->zm*sinf(-c->rx+PI_DIV_TWO)*cosf(PI-c->ry);
			c->y=c->_oy+c->zm*cosf(-c->rx+PI_DIV_TWO);
			c->z=c->_oz+c->zm*sinf(-c->rx+PI_DIV_TWO)*sinf(PI-c->ry);
		}
	}
	return look_at_matrix(c->x,c->y,c->z,sinf(c->rx-PI_DIV_TWO)*cosf(c->ry),cosf(c->rx-PI_DIV_TWO),sinf(c->rx-PI_DIV_TWO)*sinf(c->ry),0,1,0);
}



struct CBufferLayout{
	RawVector cd;
	RawMatrix cm;
	RawMatrix pm;
};



ID3D11Buffer* cb=NULL;
RawMatrix pm;
Camera c;



void run_editor(void){
	init_renderer();
}



void init_editor(void){
	cb=create_constant_buffer(sizeof(struct CBufferLayout));
	*renderer_cc=EDITOR_BG_COLOR_R;
	*(renderer_cc+1)=EDITOR_BG_COLOR_G;
	*(renderer_cc+2)=EDITOR_BG_COLOR_B;
	*(renderer_cc+3)=1;
	c=create_camera(10,500,0,0,0,90,0,0,1);
	c->enabled=true;
	float n=EPSILON;
	float f=1000;
	pm=raw_matrix(1/renderer_aspect_ratio,0,0,0,0,1,0,0,0,0,-2/(f-n),-(f+n)/(f-n),0,0,0,1);
	terrain_init();
}



void update_editor(double dt){
	static float t=0;
	t+=(float)(dt*1e-6);
	Matrix cm=update_camera(c,(float)(dt*1e-6));
	if (cm==NULL){
		return;
	}
	struct CBufferLayout cb_dt={
		raw_vector(sinf(c->rx-PI_DIV_TWO)*cosf(c->ry),cosf(c->rx-PI_DIV_TWO),sinf(c->rx-PI_DIV_TWO)*sinf(c->ry),1),
		as_raw_matrix(cm),
		pm
	};
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	update_constant_buffer(cb,(void*)&cb_dt);
	terrain_draw();
	free(cm);
}
