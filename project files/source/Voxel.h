#pragma once

#ifndef __VOXEL_H__
#define __VOXEL_H__

#include "common.h"

#define EPSILON  1e-3

using namespace Angel;

class Voxel {
public:

	//std::string name;

	typedef struct {
		vec4 color = vec4(0.0);
		float Kd = 0.0;
		float Ks = 0.0;
		float Kn = 0.0;
		float Kt = 0.0;
		float Ka = 0.0;
		float Kr = 0.0;
	} ShadingValues;

	typedef struct {
		double t_w;
		vec4 P_w;
		vec4 N_w;
		int ID_;
	} IntersectionValues;

	//Voxel(vec4 color) { shadingValues.color = color; }
	Voxel() {};
	~Voxel() {};

	ShadingValues shadingValues;

private:
	mat4 C;
	mat4 INVC;
	mat4 INVCStar;
	mat4 TRANINVC;

public:

	void setShadingValues(ShadingValues _shadingValues) { 
		shadingValues = _shadingValues;
	}

	void setModelView(mat4 modelview) {
		C = modelview;
		INVC = invert(modelview);
		mat4 CStar = modelview;
		CStar[0][3] = 0;
		CStar[1][3] = 0;
		CStar[2][3] = 0;
		INVCStar = invert(CStar);
		TRANINVC = transpose(invert(modelview));
	}

	mat4 getModelView() { return C; }

	IntersectionValues intersect(vec4 p0, vec4 V);

	double testIntersection(vec4 p0, vec4 V);

};

#endif /* defined(__OBJECT_H__) */
