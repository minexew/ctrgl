
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <3ds.h>

#include "math.h"

#include "test_vsh_shbin.h"
#include "texture_bin.h"
#include "texture2_bin.h"

#include "gl.h"

//will be moved into ctrulib at some point
#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080)

GLuint textures[2];
GLuint program;
GLuint vbuf;

GLint lightDirection, lightAmbient;

//angle for the vertex lighting (cf test.vsh)
float lightAngle;
//object position and rotation angle
vect3Df_s position, angle;

//vertex structure
typedef struct
{
    vect3Df_s position;
    float texcoord[2];
    vect3Df_s normal;
}vertex_s;

static const float nearZ = 0.01f, farZ = 100.0f;

//object data (cube)
//obviously this doesn't have to be defined manually, but we will here for the purposes of the example
//each line is a vertex : {position.x, position.y, position.z}, {texcoord.t, texcoord.s}, {normal.x, normal.y, normal.z}
//we're drawing triangles so three lines = one triangle
const vertex_s modelVboData[]=
{
    //first face (PZ)
        //first triangle
        {(vect3Df_s){-0.5f, -0.5f, +0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){+0.5f, -0.5f, +0.5f}, (float[]){1.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){+0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        //second triangle
        {(vect3Df_s){+0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){-0.5f, +0.5f, +0.5f}, (float[]){0.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){-0.5f, -0.5f, +0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
    //second face (MZ)
        //first triangle
        {(vect3Df_s){-0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, -1.0f}},
        {(vect3Df_s){-0.5f, +0.5f, -0.5f}, (float[]){1.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, -1.0f}},
        {(vect3Df_s){+0.5f, +0.5f, -0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, -1.0f}},
        //second triangle
        {(vect3Df_s){+0.5f, +0.5f, -0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, -1.0f}},
        {(vect3Df_s){+0.5f, -0.5f, -0.5f}, (float[]){0.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, -1.0f}},
        {(vect3Df_s){-0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, -1.0f}},
    //third face (PX)
        //first triangle
        {(vect3Df_s){+0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){+1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){+0.5f, +0.5f, -0.5f}, (float[]){1.0f, 1.0f}, (vect3Df_s){+1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){+0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){+1.0f, 0.0f, 0.0f}},
        //second triangle
        {(vect3Df_s){+0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){+1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){+0.5f, -0.5f, +0.5f}, (float[]){0.0f, 0.0f}, (vect3Df_s){+1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){+0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){+1.0f, 0.0f, 0.0f}},
    //fourth face (MX)
        //first triangle
        {(vect3Df_s){-0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){-1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){-0.5f, -0.5f, +0.5f}, (float[]){1.0f, 1.0f}, (vect3Df_s){-1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){-0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){-1.0f, 0.0f, 0.0f}},
        //second triangle
        {(vect3Df_s){-0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){-1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){-0.5f, +0.5f, -0.5f}, (float[]){0.0f, 0.0f}, (vect3Df_s){-1.0f, 0.0f, 0.0f}},
        {(vect3Df_s){-0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){-1.0f, 0.0f, 0.0f}},
    //fifth face (PY)
        //first triangle
        {(vect3Df_s){-0.5f, +0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, +1.0f, 0.0f}},
        {(vect3Df_s){-0.5f, +0.5f, +0.5f}, (float[]){1.0f, 1.0f}, (vect3Df_s){0.0f, +1.0f, 0.0f}},
        {(vect3Df_s){+0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, +1.0f, 0.0f}},
        //second triangle
        {(vect3Df_s){+0.5f, +0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, +1.0f, 0.0f}},
        {(vect3Df_s){+0.5f, +0.5f, -0.5f}, (float[]){0.0f, 0.0f}, (vect3Df_s){0.0f, +1.0f, 0.0f}},
        {(vect3Df_s){-0.5f, +0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, +1.0f, 0.0f}},
    //sixth face (MY)
        //first triangle
        {(vect3Df_s){-0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, -1.0f, 0.0f}},
        {(vect3Df_s){+0.5f, -0.5f, -0.5f}, (float[]){1.0f, 1.0f}, (vect3Df_s){0.0f, -1.0f, 0.0f}},
        {(vect3Df_s){+0.5f, -0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, -1.0f, 0.0f}},
        //second triangle
        {(vect3Df_s){+0.5f, -0.5f, +0.5f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, -1.0f, 0.0f}},
        {(vect3Df_s){-0.5f, -0.5f, +0.5f}, (float[]){0.0f, 0.0f}, (vect3Df_s){0.0f, -1.0f, 0.0f}},
        {(vect3Df_s){-0.5f, -0.5f, -0.5f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, -1.0f, 0.0f}},

    // 2D overlay
        //first triangle
        {(vect3Df_s){72.0f,  0.0f,  0.0f}, (float[]){0.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){328.0f, 0.0f,  0.0f}, (float[]){1.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){328.0f, 64.0f, 0.0f}, (float[]){1.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        //second triangle
        {(vect3Df_s){328.0f, 64.0f, 0.0f}, (float[]){1.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){72.0f,  64.0f, 0.0f}, (float[]){0.0f, 1.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
        {(vect3Df_s){72.0f,  0.0f,  0.0f}, (float[]){0.0f, 0.0f}, (vect3Df_s){0.0f, 0.0f, +1.0f}},
};

static void set3DView()
{
    mtx44 projection, modelView;

    // standard perspective projection
    initProjectionMatrix((float*) projection, 80.0f*M_PI/180.0f, 240.0f/400.0f, nearZ, farZ);
    rotateMatrixZ((float*) projection, M_PI/2, false);   //because framebuffer is sideways...
    glDirectLoadMatrixfCTR(GL_PROJECTION, (float*) projection);

    // poor man's camera control
    loadIdentity44((float*) modelView);
    translateMatrix((float*) modelView, position.x, position.y, position.z);
    rotateMatrixX((float*) modelView, angle.x, false);
    rotateMatrixY((float*) modelView, angle.y, false);
    glDirectLoadMatrixfCTR(GL_MODELVIEW, (float*) modelView);
}

static void set2DView()
{
    mtx44 projection, modelView;
    loadIdentity44((float*) projection);

    // build an orthographic projection matrix accounting for the 3DS screen size and orientation
    // currently this will always project at screen depth, even in stereo

    projection[0][0] = 0.0f;
    projection[0][1] = 1.0f / (240.0f * 0.5f);
    projection[0][3] = -1.0f;

    projection[1][0] = -1.0f / (400.0f * 0.5f);
    projection[1][1] = 0.0f;
    projection[1][3] = 1.0f;

    projection[2][2] = 0.5f;
    projection[2][3] = -0.5f;

    projection[3][3] = 1.0f;

    glDirectLoadMatrixfCTR(GL_PROJECTION, (float*) projection);

    // also reset modelView
    loadIdentity44((float*) modelView);
    glDirectLoadMatrixfCTR(GL_MODELVIEW, (float*) modelView);

    // NOTE: we didn't bother resetting the lighting etc.
}

// topscreen
void renderFrame()
{
    //setup lighting (specific to our vertex shader)
    vect3Df_s lightDir = vnormf(vect3Df(cos(lightAngle), -1.0f, sin(lightAngle)));
    glUniform4fv(lightDirection, 1, (float[]){0.0f, -lightDir.z, -lightDir.y, -lightDir.x});
    glUniform4fv(lightAmbient, 1, (float[]){0.7f, 0.4f, 0.4f, 0.4f});

    // draw the cube
    set3DView();

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);

    // draw the logo
    set2DView();

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glDrawArrays(GL_TRIANGLES, 6 * 2 * 3, 2 * 3);
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
    ctrglResetGPU();

    // basic OpenGL settings
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load the vertex shader
    program = glCreateProgram();
    glLoadProgramBinaryCTR(program, test_vsh_shbin, test_vsh_shbin_size);
    glUseProgram(program);

    lightDirection = glGetUniformLocation(program, "lightDirection");
    lightAmbient = glGetUniformLocation(program, "lightAmbient");

    // create and bind the texture
    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_BLOCK_RGBA8_CTR, 128, 128, 0,
            GL_BLOCK_RGBA_CTR, GL_UNSIGNED_BYTE, texture_bin);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_BLOCK_RGBA8_CTR, 256, 64, 0,
            GL_BLOCK_RGBA_CTR, GL_UNSIGNED_BYTE, texture2_bin);

    glEnable(GL_TEXTURE_2D);

    // build the vertex buffer
    glGenBuffers(1, &vbuf);
    glBindBuffer(GL_ARRAY_BUFFER, vbuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(modelVboData), modelVboData, GL_STATIC_DRAW);

    glVertexFormatCTR(3, 32);
    glVertexAttribCTR(0, 3, GL_FLOAT);
    glVertexAttribCTR(1, 2, GL_FLOAT);
    glVertexAttribCTR(2, 3, GL_FLOAT);

    // initialize object position and angle
    position = vect3Df(0.0f, 0.0f, -4.0f);
    angle = vect3Df(M_PI / 4, M_PI / 4, 0.0f);

    // set background color to blue
    glClearColorRgba8CTR(glMakeRgba8CTR(0x11, 0x22, 0x44, 0xFF));

    while (aptMainLoop())
    {
        //controls
        hidScanInput();
        //START to exit to hbmenu
        if(keysDown()&KEY_START)break;

        //A/B to change vertex lighting angle
        if(keysHeld()&KEY_A)lightAngle+=0.1f;
        if(keysHeld()&KEY_B)lightAngle-=0.1f;

        //D-PAD to rotate object
        if(keysHeld()&KEY_RIGHT)angle.x+=0.05f;
        if(keysHeld()&KEY_LEFT)angle.x-=0.05f;
        if(keysHeld()&KEY_UP)angle.y+=0.05f;
        if(keysHeld()&KEY_DOWN)angle.y-=0.05f;

        //R/L to bring object closer to or move it further from the camera
        if(keysHeld()&KEY_R)position.z+=0.1f;
        if(keysHeld()&KEY_L)position.z-=0.1f;

        // NOTE: no actual rendering is being done yet, just writing commands into a buffer
        ctrglBeginRendering();

        renderFrame();

        float slider = CONFIG_3D_SLIDERSTATE;

        if (slider > 0.0f)
            glStereoEnableCTR(slider * 0.12f, nearZ, -5.0f);
        else
            glStereoDisableCTR();

        // finally send rendering commands to the GPU
        ctrglFinishRendering();
    }

    glDeleteBuffers(1, &vbuf);
    glDeleteTextures(2, textures);

    ctrglExit();
    hidExit();
    gfxExit();
    aptExit();
    srvExit();
    return 0;
}
