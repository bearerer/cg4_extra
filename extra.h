#ifndef EXTRA_H
#define EXTRA_H

#include "Skybox.h"
#include "camera.h"

#define MAX_VERTICES 10000
#define AMOUNT_BUFFER 2

class Extra {

public:
    struct MyVertex{
        float x;
        float y;
        float z;
        float nx;
        float ny;
        float nz;
    };

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
    static void initShaders();
    static void initTFB();
private:
    static void drawTreeStart();
    static void fillTreeStart(MyVertex branch[]);
    static MyVertex* getTriangle(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2);

    static GLuint _shader;
    static bool _camRotating, _camZooming;
    static size_t _lightModel;
    static float _branchThickness;
    static float _branchHeight;
    static float _branchUp;
    static bool _isFirst;

    static GLuint _currentVB, _currentTFB;
    static GLuint _branchBuffer[AMOUNT_BUFFER];
    static GLuint _transformFeedback[AMOUNT_BUFFER];
};



#endif // EXTRA_H
