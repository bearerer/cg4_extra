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

    /* Render texture with fullscreen effect */
    if (_shader == 0) {
        std::cout << "glsl version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        // setup fragment shader:
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
        glCompileShader(fragment_shader);

        // setup vertex shader:
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
        glCompileShader(vertex_shader);

        // setup geometry shader:
        GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, &geometry_shader_src, NULL);
        glCompileShader(geometry_shader);

        _shader = glCreateProgram();

        checkShaderCompileStatus(&vertex_shader, "vertex");
        glAttachShader(_shader, vertex_shader);

        checkShaderCompileStatus(&fragment_shader, "fragment");
        glAttachShader(_shader, fragment_shader);

//        checkShaderCompileStatus(&geometry_shader, "geometry");
//        glProgramParameteriEXT(_shader, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
//        glProgramParameteriEXT(_shader, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLES);
//        glAttachShader(_shader, geometry_shader);

        glLinkProgram(_shader);
        checkShaderLinkStatus(&_shader, "kekse");
    }
    glUseProgram(_shader);
    glUniform1i(glGetUniformLocation(_shader, "tex"), 0);   /* we only use texture unit 0 */
    glUniform1f(glGetUniformLocation(_shader, "ripple_offset"), ripple_offset);

    glUniform1f(glGetUniformLocation(_shader, "kd"), 0.8f);
    glUniform1f(glGetUniformLocation(_shader, "ks"), 0.8f);
    glUniform1f(glGetUniformLocation(_shader, "shininess"), 15.0f);
    glUniform3f(glGetUniformLocation(_shader, "light_position"), -10.0f+10.0f*cos(_alpha), 10.0f, 4.0f+10.0f*sin(_alpha));
    glUniform3f(glGetUniformLocation(_shader, "light_color"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(_shader, "surface_color"), 0.8f, 0.8f, 1.0f);
    glUniform1i(glGetUniformLocation(_shader, "model"), 0);
    glUniform1f(glGetUniformLocation(_shader, "r"), 0.2f);
    glUniform1f(glGetUniformLocation(_shader, "a"), 0.75f);
    glUniform1f(glGetUniformLocation(_shader, "b"), 0.25f);

//    glutSolidTorus(0.1f, 0.5f, 100, 100);
    drawTreeStart();

//    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
//            glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

//    glEnable(GL_TEXTURE_2D);
//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0f, 0.0f);
//    glVertex3f(-1.0f, -1.0f, 0.0f);
//    glTexCoord2f(1.0f, 0.0f);
//    glVertex3f(1.0f, -1.0f, 0.0f);
//    glTexCoord2f(1.0f, 1.0f);
//    glVertex3f(1.0f, 1.0f, 0.0f);
//    glTexCoord2f(0.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, 0.0f);
//    glEnd();
//    glDisable(GL_TEXTURE_2D);

    glUseProgram(0);

    glutSwapBuffers();
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

void Extra::drawTreeStart()
{
    float h = 2.f;
    float sq2half = 0.707106781f;
    float sq = 0.044194174f;
    float l = 0.03125f;
    float n = 0.f;
    float norm[3];

    glDisable(GL_LIGHTING);//TODO remove
    glColor3f(0.5f , 0.25f , 0.08f);
    glBegin(GL_TRIANGLES);

//    norm[0] =

    glNormal3f(sq2half, n, sq2half);
    glVertex3f(sq, n, -sq);
//    glVertex3f(sq, h, -sq);
//    glVertex3f(n, h, l);
    glVertex3f(n, h, n);
    glVertex3f(n, n, l);

    glNormal3f(-sq2half, n, sq2half);
    glVertex3f(n, n, l);
//    glVertex3f(n, h, l);
//    glVertex3f(-sq, h, -sq);
    glVertex3f(n, h, n);
    glVertex3f(-sq, n, -sq);

    glNormal3f(n, n, -1.f);
    glVertex3f(-sq, n, -sq);
//    glVertex3f(-sq, h, -sq);
//    glVertex3f(sq, h, -sq);
    glVertex3f(n, h, n);
    glVertex3f(sq, n, -sq);

    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_LIGHTING);//TODO remove
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

    Extra::_cam = new Camera();
    Extra::_cam->setMaxDistance(SKYBOX_OFFSET / 2.f);
    Extra::_cam->setDistance(SKYBOX_OFFSET / 4.f);
    Extra::_cam->update();
    Extra::_cam->setup();

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
