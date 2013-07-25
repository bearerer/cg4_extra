#ifndef EXTRA_H
#define EXTRA_H

#include "Skybox.h"
#include "camera.h"

#define MAX_VERTICES 10000
#define MAX_STEPS 10
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

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
    static void kill_crlf(char *str);
private:
    static void link_program(const std::string &name, const GLuint prg);
    static void drawTreeStart();
    static void fillTreeStart(MyVertex branch[]);
    static MyVertex* getTriangle(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2);

    static GLuint _shader;
    static GLuint _tfShader;
    static bool _camRotating, _camZooming;
    static size_t _lightModel;
    static float _branchThickness;
    static float _branchHeight;
    static float _branchUp;

    static GLuint _branchBuffer;
    static GLuint _transformFeedback;
//    static GLint _vertexLoc;
//    static GLint _normalLoc;

    static size_t _steps;
    static GLuint _geometry_prg;
    static GLuint _tf_query;
};



#endif // EXTRA_H
