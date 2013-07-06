#include "Skybox.h"

SkyBox::SkyBox(void){
    helper = new GenericHelper();
}


void SkyBox::setPosition(float x, float y, float z){
	glTranslatef(x,y,z);
}

void SkyBox::draw(float camX, float camY, float camZ){
	//disable depth-testing to make the skybox always appear in the background - reenabled after rendering
	//glDisable(GL_TEXTURE_CUBE_MAP);

    float xMin = camX - SKYBOX_OFFSET;
    float xMax = camX + SKYBOX_OFFSET;
    float yMin = camY - SKYBOX_OFFSET;
    float yMax = camY + SKYBOX_OFFSET;
    float zMin = camZ - SKYBOX_OFFSET;
    float zMax = camZ + SKYBOX_OFFSET;

	GLint polygon_state[2];
	glGetIntegerv( GL_POLYGON_MODE, polygon_state );
	if(polygon_state[0] != 6913)
		glEnable(GL_TEXTURE_2D);

	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

//  glColor3f(1.0f,1.0f,1.0f);
//	glTranslatef(0.0f,35.0f,0.0f);
//  glScalef(100.0f,35.0f,75.0f);

    glCullFace(GL_FRONT);
	glDisable(GL_LIGHTING); /* make sure the skybox isnt influenced by the scene's lighting */
	
    // Front Face
//    helper->loadTexture("front.png");
    helper->loadTexture("front.png");
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMin, zMax);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMin, zMax);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMax, zMax);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMax, zMax);
	glEnd();

	
	// Back Face
//    helper->loadTexture("back.png");
    helper->loadTexture("back.png");
    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMin, yMin, zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMin, yMax, zMin);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMax, yMax, zMin);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMax, yMin, zMin);
	glEnd();

	// Top Face
//    helper->loadTexture("bottom.png");
    helper->loadTexture("bottom.png");
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMax, zMin);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMax, zMax);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMax, zMax);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMax, zMin);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	// Bottom Face
	if(polygon_state[0] != 6913)
		glColor4f(0.0f,0.4f,0.1f,1.0f);
//    helper->loadTexture("top.png");
    helper->loadTexture("top.png");
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMin, zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMin, zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMin, zMax);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMin, zMax);
	glEnd();
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	if(polygon_state[0] != 6913)
		glEnable(GL_TEXTURE_2D);

	// Right face
//    helper->loadTexture("right.png");
    helper->loadTexture("right.png");
    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMin, zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMax, zMin);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMax, yMax, zMax);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMax, yMin, zMax);
	glEnd();

	// Left Face
//    helper->loadTexture("left.png");
    helper->loadTexture("left.png");
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMin, zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMin, yMin, zMax);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMin, yMax, zMax);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMax, zMin);
	glEnd();

    glPopMatrix();
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

SkyBox::~SkyBox(void){
}
