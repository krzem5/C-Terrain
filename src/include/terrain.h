#ifndef __TERRAIN_H__
#define __TERRAIN_H__
#include <editor.h>
#include <renderer.h>
#include <stdint.h>



typedef struct __TERRAIN* Terrain;



struct __TERRAIN{
	uint16_t w;
	uint16_t h;
	float* vl;
	uint32_t vll;
	uint32_t ill;
	ID3D11Buffer* ib;
	ID3D11Buffer* vb;
};



Terrain terrain_init(uint16_t w,uint16_t h);



void terrain_recalc_normals(Terrain t);



void terrain_draw(Terrain t);



#endif
