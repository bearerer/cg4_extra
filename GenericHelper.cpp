#include "GenericHelper.h"
#include "stdlib.h"
#include "qglfunctions.h"

using namespace std;

static GLenum faceTarget[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

// Singleton Implementation 
GenericHelper* GenericHelper::_instance = 0;
GenericHelper::GenericHelper(){
	textures = list<SimpleTexture*>();
	texturesSpaceAllocated = false;
}

GenericHelper* GenericHelper::getInstance() {
	if (_instance == 0) {
		_instance = new GenericHelper;
	}
	return _instance;
}

//get current number of textures in memory
int GenericHelper::getTextureCount(){
	return textures.size();
}

//add Texture to Texture List
void GenericHelper::addTexture(SimpleTexture* tex){
	textures.push_back(tex);
}

//identifies a texture by its name and returns the QImage, the texture was made of
QImage GenericHelper::getTexture(QString texname){
	QImage result;
	list<SimpleTexture*>::iterator it;
	for(it = textures.begin(); it != textures.end(); it++)	{
		if((*it)->name == texname){
			result = (*it)->img;
		}
	}

	return result;
}


//checks if texture is already loaded into memory
bool GenericHelper::isTextureInMemory(QString texname){
	list<SimpleTexture*>::iterator it;
	for(it = textures.begin(); it != textures.end(); it++)	{
		if ((*it)->name== texname)
			return true;
	}
	return false;
}


//sets the texture bound to GL_TEXTURE_2D to the texture given by name in texname
void GenericHelper::setActiveTexture(QString texname, QString target){
	if (target.toStdString() == "GL_TEXTURE_2D"){
		if (texname == "bottom.png") glBindTexture(GL_TEXTURE_2D,myTextures[0]);
		if (texname == "top.png")	 glBindTexture(GL_TEXTURE_2D,myTextures[1]);
		if (texname == "right.png")  glBindTexture(GL_TEXTURE_2D,myTextures[2]);
		if (texname == "back.png")   glBindTexture(GL_TEXTURE_2D,myTextures[3]);
		if (texname == "front.png")  glBindTexture(GL_TEXTURE_2D,myTextures[4]);
		if (texname == "left.png")   glBindTexture(GL_TEXTURE_2D,myTextures[5]);
		if (texname == "sine.png")   glBindTexture(GL_TEXTURE_2D,myTextures[6]);

		if (texname == "side.jpg") glBindTexture(GL_TEXTURE_2D, myTextures[7]);
		if (texname == "front_1.jpg") glBindTexture(GL_TEXTURE_2D, myTextures[8]);
		if (texname == "front_2.jpg") glBindTexture(GL_TEXTURE_2D, myTextures[9]);
		if (texname == "front_3.jpg") glBindTexture(GL_TEXTURE_2D, myTextures[10]);
		if (texname == "topbottom.jpg") glBindTexture(GL_TEXTURE_2D, myTextures[11]);
		if (texname == "tile.jpg") glBindTexture(GL_TEXTURE_2D, myTextures[12]);
	}
}

/*
reads a texture from file if it's not in texture memory already
after loading it calls GeneralHelper::setActiveTexture(..) to bind the texture
*/
void GenericHelper::loadTexture(QString texname){ 
	QImage img;

    if (!texturesSpaceAllocated){
        glGenTextures(12, myTextures);   // Allocate space for texture
		texturesSpaceAllocated = true;
    }

	if (!isTextureInMemory(texname)){
        if (img.load("../textures/"+texname)){
            img = QGLWidget::convertToGLFormat(img);

			setActiveTexture(texname);

			glTexImage2D(GL_TEXTURE_2D,0,4,img.width(),img.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,img.bits());
			addTexture(new SimpleTexture(texname, img));

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else {
			cout << texname.toStdString() <<": " << "Texture couldnt be loaded - filename wrong or file not in build directory?" <<endl;
		}
    } else {
    }

	setActiveTexture(texname);
}

//Generates a random float number in the range of min <-> max
float GenericHelper::randomFloat(float min, float max){
	return ((max-min)*((float)rand()/RAND_MAX))+min;  
}

GenericHelper::~GenericHelper(void)	{
	delete[] &myTextures;
	_instance = NULL;
}

