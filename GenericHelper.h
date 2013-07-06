#pragma once

#include <qimage.h>
#include <QGLWidget>
#include <iostream>
#include <list>
#include "SimpleTexture.h"



using namespace std;
class GenericHelper {
public: 
	static GenericHelper* getInstance();
	QImage getTexture(QString texname);
	list<SimpleTexture*> textures;


	void addTexture(SimpleTexture *tex);
	bool isTextureInMemory(QString texname);
    void setActiveTexture(QString texname, QString target = "GL_TEXTURE_2D");

    GenericHelper();
protected:

	int  getTextureCount();

private:
	static GenericHelper* _instance;
	bool texturesSpaceAllocated;

	~GenericHelper();

public:
	GLuint myTextures[15]; //textures names will be stored in here
	//GLuint cubeMap[1];
	//QImage img[6];
	void loadTexture(QString texname);
	static float randomFloat(float min,float max);
};

