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
    static void drawTriangle(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2);
    static void normalize(float n[]);
    static void crossProduct(float a[], float b[], float result[]);
private:
    static void drawTreeStart();
    static void initShader();

    static GLuint _shader;
    static bool _camRotating, _camZooming;
    static size_t _lightModel;
};



#endif // EXTRA_H
