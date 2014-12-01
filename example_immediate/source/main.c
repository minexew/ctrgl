
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <3ds.h>
#include <gl.h>

#include "math.h"

#include "basic_vsh_shbin.h"

//will be moved into ctrulib at some point
#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080)

GLuint program;

//vertex structure
typedef struct
{
    vect3Df_s position;
    float texcoord[2];      // not used in this example
    vect3Df_s color;
}
vertex_s;

static void set2DView()
{
    static const float nearZ = -1.0f, farZ = 1.0f;

    mtx44 projection, modelView;
    loadIdentity44((float*) projection);

    // build an orthographic projection matrix accounting for the 3DS screen size and orientation
    // now in glorious 3D

    projection[0][0] = 0.0f;
    projection[0][1] = 1.0f / (240.0f * 0.5f);
    projection[0][3] = -1.0f;

    projection[1][0] = -1.0f / (400.0f * 0.5f);
    projection[1][1] = 0.0f;
    projection[1][3] = 1.0f;

    projection[2][2] = 1.0f / (farZ - nearZ);
    projection[2][3] = -0.5f - 0.5f * (farZ + nearZ) / (farZ - nearZ);

    glOrthoProjectionMatrixfCTR((float*) projection,
            20.0f,      // units of X (in our case pixels) per a unit of Z*interaxial
                        // e.g. 20.0 means a point at distance z=1.0 from screen plane
                        // will be drawn with a stereo separation of x=20 (if interaxial=1.0),
                        // a point at distance z=0.5 with a separation of x=10 etc.

            0.0f        // screen plane Z again, in eye space (can be <= 0, unlike in perspective)
            );

    // also reset modelView
    loadIdentity44((float*) modelView);
    glModelviewMatrixfCTR((float*) modelView);
}

// topscreen
void renderFrame()
{
    // draw a triangle
    set2DView();

    // returns a pointer for us to fill out
    vertex_s* vertices = (vertex_s*) glVerticesCTR(GL_TRIANGLES, 3);

    vertices[0].position = vect3Df(200.0f, 160.0f, 0.0f);
    vertices[0].texcoord[0] = 0.0f;
    vertices[0].texcoord[1] = 0.0f;
    vertices[0].color = vect3Df(1.0f, 0.0f, 0.0f);

    vertices[1].position = vect3Df(100.0f, 80.0f, 0.0f);
    vertices[1].texcoord[0] = 0.0f;
    vertices[1].texcoord[1] = 0.0f;
    vertices[1].color = vect3Df(0.0f, 1.0f, 0.0f);

    vertices[2].position = vect3Df(300.0f, 80.0f, 0.0f);
    vertices[2].texcoord[0] = 0.0f;
    vertices[2].texcoord[1] = 0.0f;
    vertices[2].color = vect3Df(0.0f, 0.0f, 1.0f);
}

int main(int argc, char** argv)
{
    // init services
    srvInit();  
    aptInit();
    gfxInit();
    hidInit(NULL);

    // init GPU
    GPU_Init(NULL);

    // let GFX know we're ok with doing stereoscopic 3D rendering
    gfxSet3D(true);

    // init CTRGL and reset the GPU
    ctrglInit();
    ctrglAllocateCommandBuffers(0x40000, 2);
    ctrglAllocateVertexCache(0x100 * sizeof(vertex_s));
    ctrglResetGPU();

    // basic OpenGL settings
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load the vertex shader
    program = glCreateProgram();
    glLoadProgramBinary2CTR(program, (void*) basic_vsh_shbin, basic_vsh_shbin_size, GL_MEMORY_STATIC_CTR);
    glUseProgram(program);

    // set up vertex format
    glVertexFormatCTR(3, sizeof(vertex_s));
    glVertexAttribCTR(0, 3, GL_FLOAT);
    glVertexAttribCTR(1, 2, GL_FLOAT);
    glVertexAttribCTR(2, 3, GL_FLOAT);

    // set background color to black
    glClearColorRgba8CTR(glMakeRgba8CTR(0x00, 0x00, 0x00, 0x00));

    while (aptMainLoop())
    {
        //controls
        hidScanInput();
        //START to exit to hbmenu
        if(keysDown()&KEY_START)break;

        //A/B to change vertex lighting angle
        /*if(keysHeld()&KEY_A)lightAngle+=0.1f;
        if(keysHeld()&KEY_B)lightAngle-=0.1f;

        //D-PAD to rotate object
        if(keysHeld()&KEY_RIGHT)angle.x+=0.05f;
        if(keysHeld()&KEY_LEFT)angle.x-=0.05f;
        if(keysHeld()&KEY_UP)angle.y+=0.05f;
        if(keysHeld()&KEY_DOWN)angle.y-=0.05f;

        //R/L to bring object closer to or move it further from the camera
        if(keysHeld()&KEY_R)position.z+=0.1f;
        if(keysHeld()&KEY_L)position.z-=0.1f;*/

        // NOTE: no actual rendering is being done yet, just writing commands into a buffer
        ctrglBeginRendering();

        renderFrame();

        float slider = CONFIG_3D_SLIDERSTATE;

        if (slider > 0.0f)
            glStereoEnableCTR(slider);
        else
            glStereoDisableCTR();

        // finally send rendering commands to the GPU
        ctrglFinishRendering();
    }

    glDeleteProgram(program);

    ctrglExit();
    hidExit();
    gfxExit();
    aptExit();
    srvExit();
    return 0;
}
