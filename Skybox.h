#ifndef SKYBOX_H
#define SKYBOX_H

#include "GenericHelper.h"

#define SKYBOX_OFFSET 10.f

#pragma once
class SkyBox{

public:
	SkyBox(void);
	~SkyBox(void);

    void draw(float camX = 0.f, float camY = 0.f, float camZ = 0.f);
	void setPosition(float x, float y, float z);

    float x,y,z;

    GenericHelper* helper;
};

#endif /* SKYBOX_H */
