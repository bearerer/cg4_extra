#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "extra.h"

#include "fragment-shader.glsl.h"
#include "vertex-shader.glsl.h"
#include "geometry-shader.glsl.h"

SkyBox *Extra::_skybox;
Camera *Extra::_cam;
GLuint Extra::_shader;
bool Extra::_camRotating = false;
bool Extra::_camZooming = false;
float Extra::_alpha = 0.f;
size_t Extra::_lightModel = 0;
float Extra::_branchThickness = 0.04f;
float Extra::_branchHeight = 0.65f;
float Extra::_branchUp = 0.1f;
bool Extra::_isFirst = true;

GLuint Extra::_currentVB = 0;
GLuint Extra::_currentTFB = 0;
GLuint Extra::_branchBuffer[AMOUNT_BUFFER];
GLuint Extra::_transformFeedback[AMOUNT_BUFFER];

/* GLUT state */
GLboolean glut_stereo = 0;
int glut_window_id;
GLboolean fullscreen = GL_FALSE;
int fullscreen_window_x_bak;
int fullscreen_window_y_bak;
int fullscreen_window_w_bak;
int fullscreen_window_h_bak;

/* Scene state */
GLboolean pause = GL_FALSE;
GLfloat rotation_angle = 0.0f;
GLfloat ripple_offset = 0.0f;

/* Fragment shader for a screen-space ripple effect */
const GLchar *fragment_shader_src = fragment_shader_glsl;
const GLchar *vertex_shader_src = vertex_shader_glsl;
const GLchar *geometry_shader_src = geometry_shader_glsl;


bool Extra::checkShaderCompileStatus(GLuint *shader, std::string sh)
{
    GLint compiled;
    glGetObjectParameterivARB(*shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint blen = 0;
        GLsizei slen = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH , &blen);
        if (blen > 1){
            GLchar* compiler_log = (GLchar*)malloc(blen);
            glGetInfoLogARB(*shader, blen, &slen, compiler_log);
            std::cout << "Error in " << sh << ", ";
            std::cout << "compiler_log:\n" << compiler_log;
            free (compiler_log);
        }
        std::cout << std::flush;
        return false;
    }
    return true;
}

bool Extra::checkShaderLinkStatus(GLuint *shader, std::string sh)
{
    GLint compiled;
    glGetObjectParameterivARB(*shader, GL_LINK_STATUS, &compiled);
    if (!compiled)
    {
        std::cout << "error" << std::endl;
        GLint blen = 1024;
        GLsizei slen = 0;
        GLchar* compiler_log = (GLchar*)malloc(blen);
        glGetProgramInfoLog(*shader, blen, &slen, compiler_log);

        std::cout << "compiler_log:\n" << compiler_log;
        free (compiler_log);

        std::cout << std::flush;
        return false;
    }
    return true;
}


/* GLUT display function */
void Extra::display()
{
    _alpha += 0.02f;

    static GLuint tex = 0;
    static GLint tex_w = 0, tex_h = 0;

    GLfloat objX = 2.0f, objY = 3.0f, objZ = 1.0f;
//    GLfloat upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    GLfloat eyeX = 2.0f * cosf(rotation_angle / 180.0f * (float)M_PI) + objX;
    GLfloat eyeY = cosf(3.0f * rotation_angle / 180.0f * (float)M_PI) + objY;
    GLfloat eyeZ = 2.0f * sinf(rotation_angle / 180.0f * (float)M_PI) + objZ;
    /* View: a typical frustum */
    GLfloat fovy = 50.0f;
    GLfloat aspect = (GLfloat)glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat zNear = 0.1f;
    GLfloat zFar = SKYBOX_OFFSET * 3.f;

    /* Setup GL state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    /* Render scene */
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, aspect, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//    gluLookAt(eyeX, eyeY, eyeZ, objX, objY, objZ, upX, upY, upZ);
    Extra::_cam->setup();

//    glTranslatef(objX, objY, objZ);

    Extra::_skybox->draw(eyeX, eyeY, eyeZ);

    /* Read rendered scene from back buffer into texture */
    if (tex == 0) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, tex);
    if (tex_w != glutGet(GLUT_WINDOW_WIDTH) || tex_h != glutGet(GLUT_WINDOW_HEIGHT)) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
                0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        tex_w = glutGet(GLUT_WINDOW_WIDTH);
        tex_h = glutGet(GLUT_WINDOW_HEIGHT);
    }

    // start using shaders:
    glUseProgram(_shader);
//    glUniform1i(glGetUniformLocation(_shader, "tex"), 0);   /* we only use texture unit 0 */
//    glUniform1f(glGetUniformLocation(_shader, "ripple_offset"), ripple_offset);

    // set uniform shader variables:
    glUniform1f(glGetUniformLocation(_shader, "kd"), 0.8f);
    glUniform1f(glGetUniformLocation(_shader, "ks"), 0.8f);
    glUniform1f(glGetUniformLocation(_shader, "shininess"), 5.0f);
    glUniform3f(glGetUniformLocation(_shader, "light_position"), 10.0f*cos(_alpha), 10.0f, 10.0f*sin(_alpha));
    glUniform3f(glGetUniformLocation(_shader, "light_color"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(_shader, "surface_color"), 0.5f, 0.25f, 0.08f);
    glUniform1i(glGetUniformLocation(_shader, "model"), _lightModel);
    glUniform1f(glGetUniformLocation(_shader, "r"), 0.2f);
    glUniform1f(glGetUniformLocation(_shader, "a"), 0.75f);
    glUniform1f(glGetUniformLocation(_shader, "b"), 0.25f);

    glUniform1f(glGetUniformLocation(_shader, "branchHeight"), _branchHeight);
    glUniform1f(glGetUniformLocation(_shader, "branchThickness"), _branchThickness);
    glUniform1f(glGetUniformLocation(_shader, "branchUp"), _branchUp);



//    glBindBuffer(GL_ARRAY_BUFFER, _branchBuffer[0]);
//    glBeginTransformFeedback(GL_TRIANGLES);


//    if (_isFirst) {
//        glDrawArrays(GL_TRIANGLES, 0, 1);
//        _isFirst = false;
//    } else {
//        glDrawTransformFeedback(GL_POINTS, _transformFeedback[_currentVB]);
//    }

    // draw stuff:
//    glutSolidTorus(0.1f, 0.5f, 100, 100);
    drawTreeStart();

//    glEndTransformFeedback();


    // end using shaders:
    glUseProgram(0);
    glutSwapBuffers();


    // BUFFERS:
/*
 *see this:
http://pastebin.com/KTQyV2pZ

also maybe this:
http://www.twodee.org/weblog/?p=881     <<---------------------oh yeah this looks good
http://ogldev.atspace.co.uk/www/tutorial28/tutorial28.html <<-----------also this one


        printf("Preparing buffer\n");
        glGenBuffersARB(1, &bid);
        glBindBufferARB(GL_ARRAY_BUFFER, bid);
        const float init[] = {0.1f,0.2f,0.3f,0.4f};
        glBufferDataARB(GL_ARRAY_BUFFER, 4*sizeof(float)*1, NULL, GL_STREAM_DRAW);
        printf("Buffer prepared (current error: %d)\n",glGetError());

        glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, bid);
        printf("Feedback assigned (current error: %d)\n\n",glGetError());

        printf("Beginning drawing\n");
        glUseProgramObjectARB(shid);
        glBeginTransformFeedbackEXT(GL_POINTS);
        glEnable(GL_RASTERIZER_DISCARD_EXT);

        glBegin(GL_POINTS);
        printf("In data: ");
        for(int i=0; i<4; ++i) {
                printf("%.1f ", init[i]);
        }
        printf("\n");
        glVertex4fv(init);
        glEnd();

        glDisable(GL_RASTERIZER_DISCARD_EXT);
        glEndTransformFeedbackEXT();
        printf("Drawing completed (current error: %d)\n\n", glGetError());

        printf("Reading (hopefully transformed) data\n");
        float *const data = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        printf("Out data: ");
        for(int i=0; i<4; ++i) {
                printf("%.1f ", data[i]);
        }
        printf("\n");
        glUnmapBuffer(GL_ARRAY_BUFFER);
        printf("Readback complete (current error: %d)",glGetError());

        */


}

/* GLUT idle func */
void Extra::idle()
{
//    if (!pause)
//        rotation_angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0f * 15.0f;
    ripple_offset = glutGet(GLUT_ELAPSED_TIME) / 1000.0f * 10.0f;
    display();
}

/* GLUT keyboard function */
void Extra::keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'f':
        fullscreen = !fullscreen;
        if (fullscreen) {
            fullscreen_window_x_bak = glutGet(GLUT_WINDOW_X);
            fullscreen_window_y_bak = glutGet(GLUT_WINDOW_Y);
            fullscreen_window_w_bak = glutGet(GLUT_WINDOW_WIDTH);
            fullscreen_window_h_bak = glutGet(GLUT_WINDOW_HEIGHT);
            glutFullScreen();
        } else {
            glutReshapeWindow(fullscreen_window_w_bak, fullscreen_window_h_bak);
            glutPositionWindow(fullscreen_window_x_bak, fullscreen_window_y_bak);
        }
        break;
    case 'p':
    case ' ':
        pause = !pause;
        break;
    case 27:
    case 'q':
        glutDestroyWindow(glut_window_id);
        break;
    case 'l':
        _lightModel = (_lightModel + 1) % 3;
        break;
    case 't':
        _branchThickness -= 0.01f;
        if (_branchThickness <= 0.f) _branchThickness = 0.01f;
        break;
    case 'T':
        _branchThickness += 0.01f;
        if (_branchThickness >= 1.f) _branchThickness = 0.99f;
        break;
    case 'H':
        _branchHeight -= 0.01f;
        if (_branchHeight <= 0.1f) _branchHeight = 0.11f;
        break;
    case 'h':
        _branchHeight += 0.01f;
        if (_branchHeight >= 0.9f) _branchHeight = 0.89f;
        break;
    case 'U':
        _branchUp -= 0.01f;
        if (_branchUp <= -0.1f) _branchUp = -0.09f;
        break;
    case 'u':
        _branchUp += 0.01f;
        if (_branchUp >= 0.2f) _branchUp = 0.19f;
        break;
    }
}

void Extra::specialInput(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
        break;
        case GLUT_KEY_DOWN:
        break;
        case GLUT_KEY_LEFT:
        break;
        case GLUT_KEY_RIGHT:
        break;
    }
}

void Extra::mouseClick(int button, int state, int x, int y)
{
    _camRotating = false;
    _camZooming = false;
    if(state == GLUT_DOWN){
        _cam->setLastX(x);
        _cam->setLastY(y);
        if (button == GLUT_LEFT_BUTTON)
            _camRotating = true;
        else if (button == GLUT_RIGHT_BUTTON)
            _camZooming = true;
    }
}

void Extra::mousePassive(int x, int y)
{
}

void Extra::mouseMove(int x, int y)
{
    // get delta movement to last position
    int deltaX = x - _cam->lastX();
    int deltaY = y - _cam->lastY();

    // process action
    if (_camRotating) {
      _cam->setPolar(_cam->polar() + deltaY);
      _cam->setAzimut(_cam->azimut() - deltaX);
      _cam->update();
    }
    else if (_camZooming) {
        _cam->setDistance( std::min( _cam->maxDistance(), (float) (_cam->distance() - 0.1*deltaY)));
    }
    // update state
    _cam->setLastX(x);
    _cam->setLastY(y);

    //    std::cout << x << " " << y << std::endl;
}

/* builds cross product of <a x b> and saves it into result */
void Extra::crossProduct (float a[], float b[], float result[]){
//    float result[3];

    result[0] = a[1] * b[2] - b[1] * a[2];
    result[1] = a[2] * b[0] - b[2] * a[0];
    result[2] = a[0] * b[1] - b[0] * a[1];

//    return result;
}

/* normalizes given vector */
void Extra::normalize (float n[]){
    float lengh = sqrt(n[0]*n[0]  +  n[1]*n[1]  +  n[2]*n[2]);
    if (lengh == 0){
        std::cerr << "length of normal is 0" << std::endl;
        return;
    }
    n[0] /= lengh;
    n[1] /= lengh;
    n[2] /= lengh;
}

/* draws trinagle abc with correct normal */
void Extra::drawTriangle(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2){
    float n[3];
    float ba[3];
    float bc[3];

    ba[0] = a0 - b0;
    ba[1] = a1 - b1;
    ba[2] = a2 - b2;

    bc[0] = c0 - b0;
    bc[1] = c1 - b1;
    bc[2] = c2 - b2;

//    crossProduct(bc, ba, n);
//    normalize(n);

    n[0] = a0;
    n[1] = a1 + 1.f;
    n[2] = a2;
    normalize(n);
    glNormal3f(n[0], n[1], n[2]);
    glVertex3f(a0, a1, a2);
    n[0] = a0 + c0;
    n[1] = b1 - 1.f;
    n[2] = a2 + c2;
    normalize(n);
    glNormal3f(n[0], n[1], n[2]);
    glVertex3f(b0, b1, b2);
    n[0] = c0;
    n[1] = c1 + 1.f;
    n[2] = c2;
    normalize(n);
    glNormal3f(n[0], n[1], n[2]);
    glVertex3f(c0, c1, c2);
}

void Extra::drawTreeStart()
{
    float h = 2.f;
    float sq = 0.044194174f;
    float l = 0.03125f;
    float n = 0.f;
    float b = -1.f;

//    glDisable(GL_LIGHTING);//TODO remove
    glColor3f(0.5f , 0.25f , 0.08f);
    glBegin(GL_TRIANGLES);

    drawTriangle( sq, b,-sq   ,  n,h+b,n  ,    n, b,  l);
    drawTriangle(  n, b,  l   ,  n,h+b,n  ,  -sq, b,-sq);
    drawTriangle(-sq, b,-sq   ,  n,h+b,n  ,   sq, b, -sq);

    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    //    glEnable(GL_LIGHTING);//TODO remove
}

Extra::MyVertex* Extra::getTriangle(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2)
{
    float n[3];
    float ba[3];
    float bc[3];
    MyVertex result[3];

    ba[0] = a0 - b0;
    ba[1] = a1 - b1;
    ba[2] = a2 - b2;

    bc[0] = c0 - b0;
    bc[1] = c1 - b1;
    bc[2] = c2 - b2;

    n[0] = a0;
    n[1] = a1 + 1.f;
    n[2] = a2;
    normalize(n);
    result[0].nx = n[0];
    result[0].ny = n[1];
    result[0].nz = n[2];
    result[0].x = a0;
    result[0].y = a1;
    result[0].z = a2;

    n[0] = a0 + c0;
    n[1] = b1 - 1.f;
    n[2] = a2 + c2;
    normalize(n);
    result[1].nx = n[0];
    result[1].ny = n[1];
    result[1].nz = n[2];
    result[1].x = b0;
    result[1].y = b1;
    result[1].z = b2;

    n[0] = c0;
    n[1] = c1 + 1.f;
    n[2] = c2;
    normalize(n);
    result[2].nx = n[0];
    result[2].ny = n[1];
    result[2].nz = n[2];
    result[2].x = c0;
    result[2].y = c1;
    result[2].z = c2;

    return result;
}

void Extra::fillTreeStart(MyVertex branch[])
{
    float h = 2.f;
    float sq = 0.044194174f;
    float l = 0.03125f;
    float n = 0.f;
    float b = -1.f;
    MyVertex *tmp;

    tmp = getTriangle(sq, b,-sq   ,  n,h+b,n  ,    n, b,  l);
    branch[0] = tmp[0];
    branch[1] = tmp[1];
    branch[2] = tmp[2];

    tmp = getTriangle(  n, b,  l   ,  n,h+b,n  ,  -sq, b,-sq);
    branch[3] = tmp[0];
    branch[4] = tmp[1];
    branch[5] = tmp[2];

    tmp = getTriangle(-sq, b,-sq   ,  n,h+b,n  ,   sq, b, -sq);
    branch[6] = tmp[0];
    branch[7] = tmp[1];
    branch[8] = tmp[2];
}

void Extra::initTFB()
{
    MyVertex branch[MAX_VERTICES];
    fillTreeStart(branch);

    glGenTransformFeedbacks(AMOUNT_BUFFER, _transformFeedback);
    glGenBuffers(AMOUNT_BUFFER, _branchBuffer);

    for (unsigned int i = 0; i < AMOUNT_BUFFER ; i++) {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _transformFeedback[i]);
        glBindBuffer(GL_ARRAY_BUFFER, _branchBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * MAX_VERTICES, branch, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _branchBuffer[i]);
    }
}

void Extra::initShaders()
{
    std::cout << "glsl version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // setup vertex shader:
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);
    checkShaderCompileStatus(&vertex_shader, "vertex");

    // setup geometry shader:
    GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry_shader, 1, &geometry_shader_src, NULL);
    glCompileShader(geometry_shader);
    checkShaderCompileStatus(&geometry_shader, "geometry");

    // setup fragment shader:
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);
    checkShaderCompileStatus(&fragment_shader, "fragment");

    _shader = glCreateProgram();

    glProgramParameteriEXT(_shader, GL_GEOMETRY_VERTICES_OUT_EXT,12);
    glProgramParameteriEXT(_shader, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
    glProgramParameteriEXT(_shader, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLES);

    glAttachShader(_shader, vertex_shader);
    glAttachShader(_shader, geometry_shader);
    glAttachShader(_shader, fragment_shader);

    glLinkProgram(_shader);
    checkShaderLinkStatus(&_shader, "kekse");
}

int main(int argc, char *argv[])
{
    /* Initialize GLUT Window */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glut_window_id = glutCreateWindow("cg4_extra");

    Extra::_skybox = new SkyBox();

    /* Initialize OpenGL extensions */
    glewInit();

    Extra::initShaders();

    Extra::_cam = new Camera();
    Extra::_cam->setMaxDistance(SKYBOX_OFFSET / 2.f);
    Extra::_cam->setDistance(SKYBOX_OFFSET / 4.f);
    Extra::_cam->update();
    Extra::_cam->setup();

    Extra::initTFB();

    /* Start GLUT loop */
    glutDisplayFunc(Extra::display);
    glutIdleFunc(Extra::idle);
    glutKeyboardFunc(Extra::keyboard);
    glutMouseFunc(Extra::mouseClick);
    glutMotionFunc(Extra::mouseMove);
    glutPassiveMotionFunc(Extra::mousePassive);
    glutSpecialFunc(Extra::specialInput);
    glutMainLoop();

    return 0;
}


