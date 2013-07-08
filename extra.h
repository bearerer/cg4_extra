#ifndef EXTRA_H
#define EXTRA_H

#include "Skybox.h"
#include "camera.h"

class Extra {

public:
    static void display();
    static void idle();
    static void keyboard(unsigned char key, int x, int y);
    static void specialInput(int key, int x, int y);
    static void mouseClick(int button, int state, int x, int y);
    static void mousePassive(int x, int y);
    static void mouseMove(int x, int y);

    static SkyBox *_skybox;
    static Camera *_cam;
    static float _alpha;

    static bool checkShaderCompileStatus(GLuint *shader, std::string sh);
    static bool checkShaderLinkStatus(GLuint *shader, std::string sh);
private:
    static void drawTreeStart();

    static GLuint _shader;
    static bool _camRotating, _camZooming;
};



#endif // EXTRA_H
