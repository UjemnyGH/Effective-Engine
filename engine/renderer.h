#ifndef _EFFECTIVE_RENDERER_
#define _EFFECTIVE_RENDERER_

#include "gl_buffers.h"
#include "core.h"
#include "mesh.h"

typedef struct Renderer_s {
    ShaderProgram_t mShaderProgram;
    Framebuffer_t mFramebuffer;
} Renderer_t;

void RInitialize(Renderer_t* pRend) {
    SPInitialize(&pRend->mShaderProgram);
}

void RResetShader(Renderer_t* pRend) {
    SPDelete(&pRend->mShaderProgram);
    SPInitialize(&pRend->mShaderProgram);
}

void RAddShader(Renderer_t* pRend, uint32_t shader) {
    SPAttach(&pRend->mShaderProgram, shader);
}

void RMakeShader(Renderer_t* pRend) {
    SPLink(&pRend->mShaderProgram);
}

void RSetInt(Renderer_t* pRend, const char* location, int value) {
    SPUse(&pRend->mShaderProgram);

    glUniform1i(glGetUniformLocation(pRend->mShaderProgram.mId, location), value);

    SPUnuse();
}

void RSetIntPtr(Renderer_t* pRend, const char* location, int* values, uint32_t size) {
    SPUse(&pRend->mShaderProgram);

    glUniform1iv(glGetUniformLocation(pRend->mShaderProgram.mId, location), size, values);

    SPUnuse();
}

void RSetMatrix4(Renderer_t* pRend, const char* location, mat4_t matrix) {
    SPUse(&pRend->mShaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(pRend->mShaderProgram.mId, location), 1, 0, matrix.m);

    SPUnuse();
}

void RTestSetup(Renderer_t* pRend) {
    RAddShader(pRend, SHDLoadFromMemory(GL_VERTEX_SHADER, gSimpleVertexShaderSource));
    RAddShader(pRend, SHDLoadFromMemory(GL_FRAGMENT_SHADER, gSimpleFragmentShaderSource));
    RMakeShader(pRend);
    RSetIntPtr(pRend, "uTexture", (int*)gTextureSamplers, 32);
}

void RRender(Renderer_t* pRend, RenderData_t* pRd, uint32_t mode, bool useFramebuffer) {
    if(useFramebuffer) {
        FBBind(&pRend->mFramebuffer);
    }

    SPUse(&pRend->mShaderProgram);
    VABind(&pRd->mVArray);

    for(int i = 0; i < 32; i++) {
        if(pRd->mTexturesPtr[i] != nullptr) TABindUnit(pRd->mTexturesPtr[i], i);
    }

    glDrawArrays(mode, 0, pRd->mMeshPtr->mJoinedMesh.mMeshSize);

    VAUnbind();
    SPUnuse();

    if(useFramebuffer) {
        FBUnbind();
    }
}

#endif