#ifndef _EFFECTIVE_GL_BUFFERS_
#define _EFFECTIVE_GL_BUFFERS_

#include <stdint.h>
#include <glad/gl.h>
#include <stdlib.h> 
#include "core.h"

uint32_t SHDLoadFromMemory(uint32_t type, const char* src) {
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    return shader;
}

uint32_t SHDLoadFromFile(uint32_t type, const char* filename) {
    FILE* f = fopen(filename, "rb+");

    fseek(f, 0, 2);
    uint32_t len = ftell(f);
    fseek(f, 0, 0);

    char* buffer = (char*)MECCalloc(len, 1);

    fread(buffer, sizeof(char), len, f);

    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char * const *)&buffer, nullptr);
    glCompileShader(shader);

    MECFree(buffer);
    fclose(f);

    return shader;
}

typedef struct ShaderProgram_s {
    uint32_t mId;
    bool mCreated;
} ShaderProgram_t;

void SPInitialize(ShaderProgram_t *pSp) {
    if(!pSp->mCreated) {
        pSp->mId = glCreateProgram();

        pSp->mCreated = true;
    }
}

void SPUse(ShaderProgram_t* pSp) {
    SPInitialize(pSp);

    glUseProgram(pSp->mId);
}

void SPUnuse() {
    glUseProgram(0);
}

void SPLink(ShaderProgram_t *pSp) {
    SPInitialize(pSp);
    
    glLinkProgram(pSp->mId);

    int isLinked = 0;

    glGetProgramiv(pSp->mId, GL_LINK_STATUS, &isLinked);

    if(!isLinked) {
        int maxLen = 0;

        glGetProgramiv(pSp->mId, GL_INFO_LOG_LENGTH, &maxLen);

        char *infoMsg = (char*)MECCalloc(maxLen, 1);

        glGetProgramInfoLog(pSp->mId, maxLen, &maxLen, infoMsg);

        E_ERR(infoMsg);

        MECFree(infoMsg);
    }
}

void SPAttach(ShaderProgram_t *pSp, uint32_t shader) {
    SPInitialize(pSp);

    glAttachShader(pSp->mId, shader);
}

void SPDelete(ShaderProgram_t *pSp) {
    if(pSp->mCreated) {
        glDeleteProgram(pSp->mId);

        pSp->mCreated = false;
    }
}

typedef struct VArray_s {
    uint32_t mId;
    bool mCreated;
} VArray_t;

void VAInitialize(VArray_t *pVa) {
    if(!pVa->mCreated) {
        glGenVertexArrays(1, &pVa->mId);

        pVa->mCreated = true;
    }
}

void VABind(VArray_t *pVa) {
    VAInitialize(pVa);

    glBindVertexArray(pVa->mId);
}

void VAUnbind() {
    glBindVertexArray(0);
}

void VADelete(VArray_t *pVa) {
    if(pVa->mCreated) {
        glDeleteVertexArrays(1, &pVa->mId);

        pVa->mCreated = false;
    }
}

typedef struct VBuffer_s {
    uint32_t mId;
    bool mCreated;
} VBuffer_t;

void VBInitialize(VBuffer_t *pVb) {
    if(!pVb->mCreated) {
        glGenBuffers(1, &pVb->mId);

        pVb->mCreated = true;
    }
}

void VBBind(VBuffer_t *pVb) {
    VBInitialize(pVb);

    glBindBuffer(GL_ARRAY_BUFFER, pVb->mId);
}

void VBBindPlace(VBuffer_t *pVb, uint32_t index, uint32_t dimmensions) {
    VBBind(pVb);

    glVertexAttribPointer(index, dimmensions, GL_FLOAT, 0, 0, nullptr);
    glEnableVertexAttribArray(index);
}

void VBBindData(VBuffer_t *pVb, void* data, uint32_t size) {
    VBBind(pVb);

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void VBDelete(VBuffer_t *pVb) {
    if(pVb->mCreated) {
        glDeleteBuffers(1, &pVb->mId);

        pVb->mCreated = false;
    }
}

typedef struct TextureArray_s {
    uint32_t mId;
    bool mCreated;
} TextureArray_t;

void TAInitialize(TextureArray_t *pTa) {
    if(!pTa->mCreated) {
        glGenTextures(1, &pTa->mId);

        pTa->mCreated = true;
    }
}

void TABind(TextureArray_t *pTa) {
    TAInitialize(pTa);

    glBindTexture(GL_TEXTURE_2D_ARRAY, pTa->mId);
}

void TABindUnit(TextureArray_t *pTa, uint32_t unit) {
    TAInitialize(pTa);

    glBindTextureUnit(unit, pTa->mId);
}

void TABindData(TextureArray_t *pTa, uint32_t width, uint32_t height, uint8_t *pixels, uint32_t layers) {
    TABind(pTa);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layers);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layers, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TADelete(TextureArray_t *pTa) {
    if(pTa->mCreated) {
        glDeleteTextures(1, &pTa->mId);

        pTa->mCreated = false;
    }
}

typedef struct Framebuffer_s {
    uint32_t mId, mTextureId;
    bool mCreated, mTextureCreated;
} Framebuffer_t;

void FBInitialize(Framebuffer_t *pFb) {
    if(!pFb->mCreated) {
        glGenFramebuffers(1, &pFb->mId);

        pFb->mCreated = true;
    }
}

void FBBind(Framebuffer_t *pFb) {
    FBInitialize(pFb);

    glBindFramebuffer(GL_FRAMEBUFFER, pFb->mId);
}

void FBUnbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBGetFrameColor(Framebuffer_t *pFb, uint32_t width, uint32_t height) {
    FBBind(pFb);

    if(!pFb->mTextureCreated) {
        glGenTextures(1, &pFb->mTextureId);

        pFb->mTextureCreated = true;
    }

    glBindTexture(GL_TEXTURE_2D, pFb->mTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pFb->mTextureId, 0);

    FBUnbind();
}

void FBGetFrameDepth(Framebuffer_t *pFb, uint32_t width, uint32_t height) {
    FBBind(pFb);

    if(!pFb->mTextureCreated) {
        glGenTextures(1, &pFb->mTextureId);

        pFb->mTextureCreated = true;
    }

    glBindTexture(GL_TEXTURE_2D, pFb->mTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pFb->mTextureId, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    FBUnbind();
}

void FBGetFrameDepthStencil(Framebuffer_t *pFb, uint32_t width, uint32_t height) {
    FBBind(pFb);

    if(!pFb->mTextureCreated) {
        glGenTextures(1, &pFb->mTextureId);

        pFb->mTextureCreated = true;
    }

    glBindTexture(GL_TEXTURE_2D, pFb->mTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, pFb->mTextureId, 0);

    FBUnbind();
}

void FBDelete(Framebuffer_t *pFb) {
    if(pFb->mCreated) {
        glDeleteFramebuffers(1, &pFb->mId);

        pFb->mCreated = false;
    }

    if(pFb->mTextureCreated) {
        glDeleteTextures(1, &pFb->mTextureId);

        pFb->mTextureCreated = false;
    }
}

typedef struct Renderbuffer_s {
    uint32_t mId;
    bool mCreated;
} Renderbuffer_t;

void RBInitialize(Renderbuffer_t *pRb) {
    if(!pRb->mCreated) {
        glGenRenderbuffers(1, &pRb->mId);

        pRb->mCreated = true;
    }
}

void RBBind(Renderbuffer_t *pRb) {
    RBInitialize(pRb);

    glBindRenderbuffer(GL_RENDERBUFFER, pRb->mId);
}

void RBUnbind() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RBCreateStorage(Renderbuffer_t *pRb, Framebuffer_t *pFb, uint32_t width, uint32_t height) {
    RBBind(pRb);
    FBBind(pFb);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pRb->mId);

    FBGetFrameColor(pFb, width, height);

    FBUnbind();
    RBUnbind();
}

void RBDelete(Renderbuffer_t *pRb) {
    if(pRb->mCreated) {
        glDeleteRenderbuffers(1, &pRb->mId);

        pRb->mCreated = false;
    }
}

#endif