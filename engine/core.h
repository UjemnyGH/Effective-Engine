#ifndef _EFFECTIVE_CORE_
#define _EFFECTIVE_CORE_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define E_ERR(msg) printf("\x1b[1;31m[ERROR]\x1b[0;31m: In %s at line %d in %s() : \x1b[0m%s\x1b[0m\n", __FILE__, __LINE__, __func__, msg); fflush(stdout);
#define E_WARN(msg) printf("\x1b[1;33m[WARN]\x1b[0;33m: In %s at line %d in %s() : \x1b[0m%s\x1b[0m\n", __FILE__, __LINE__, __func__, msg); fflush(stdout);
#define E_INFO(msg) printf("\x1b[1;34m[INFO]\x1b[0;34m: In %s at line %d in %s() : \x1b[0m%s\x1b[0m\n", __FILE__, __LINE__, __func__, msg); fflush(stdout);

#define E_ERR_ARG(msg, ...) printf("\x1b[1;31m[ERROR]\x1b[0;31m: In %s at line %d in %s() : \x1b[0m"msg"\x1b[0m\n", __FILE__, __LINE__, __func__, __VA_ARGS__); fflush(stdout);
#define E_WARN_ARG(msg, ...) printf("\x1b[1;33m[WARN]\x1b[0;33m: In %s at line %d in %s() : \x1b[0m"msg"\x1b[0m\n", __FILE__, __LINE__, __func__, __VA_ARGS__); fflush(stdout);
#define E_INFO_ARG(msg, ...) printf("\x1b[1;34m[INFO]\x1b[0;34m: In %s at line %d in %s() : \x1b[0m"msg"\x1b[0m\n", __FILE__, __LINE__, __func__, __VA_ARGS__); fflush(stdout);

const char* gSimpleVertexShaderSource = 
"#version 450 core\n"
"uniform mat4 uProjection;\n"
"uniform mat4 uView;\n"
"uniform mat4 uTransform;\n"
"layout(location = 0) in vec4 iPos;\n"
"layout(location = 1) in vec4 iCol;\n"
"layout(location = 2) in vec3 iNorm;\n"
"layout(location = 3) in vec2 iTexCoord;\n"
"layout(location = 4) in float iTexId;\n"
"out vec4 vCol;\n"
"out vec3 vNorm;\n"
"out vec2 vTexCoord;\n"
"out float vTexId;\n"
"void main() {\n"
"   gl_Position = uProjection * uView * uTransform * iPos;\n"
"   vCol = iCol;\n"
"   vNorm = iNorm;\n"
"   vTexCoord = iTexCoord;\n"
"   vTexId = iTexId;\n"
"}\0";

const char* gSimpleFragmentShaderSource = 
"#version 450 core\n"
"uniform sampler2DArray uTexture[32];\n"
"in vec4 vCol;\n"
"in vec3 vNorm;\n"
"in vec2 vTexCoord;\n"
"in float vTexId;\n"
"out vec4 oCol;\n"
"void main() {\n"
"   vec4 result;\n"
"   int texId = int(vTexId);\n"
"   if(texId > 31) {\n"
"       result = vCol;\n"
"   } else {\n"
"       result = texture(uTexture[texId], vec3(vTexCoord, 0.0)) * vCol;\n"
"   }\n"
"   if(result.w < 0.05) discard;\n"
"   oCol = result;\n"
"}\n";

const int gTextureSamplers[32] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31
};

/**
 * @brief Convert bytes to little endian (switch bit places)
 * 
 * @param bytes 
 * @return uint64_t 
 */
uint32_t CBytesToLittleEndian(uint32_t bytes) {
    return ((bytes & 0xff) << 24) | ((bytes & 0xff00) << 8) | ((bytes & 0xff0000) >> 8) | ((bytes & 0xff000000) >> 24);
}

/**
 * @brief Convert bytes to little endian (switch bit places) 64 bit version
 * 
 * @param bytes 
 * @return uint64_t 
 */
uint64_t CBytesToLittleEndian_64(uint64_t bytes) {
    return ((bytes & 0xff) << 56) | ((bytes & 0xff00) << 48) | ((bytes & 0xff0000) << 40) | ((bytes & 0xff000000) << 32) | ((bytes & 0xff00000000) >> 32) | ((bytes & 0xff0000000000) >> 40) | ((bytes & 0xff000000000000) >> 48) | ((bytes & 0xff000000000000) >> 56);
}

/**
 * @brief Convert bytes to float
 * 
 * @param bytes 
 * @return float 
 */
float CBytesToFloat(uint32_t bytes) {
    uint32_t convert_endiness = CBytesToLittleEndian(bytes);

    float result = 0.0f;

    memcpy(&result, &convert_endiness, sizeof(float));

    return result;
}

/**
 * @brief Convert bytes to double
 * 
 * @param bytes 
 * @return double 
 */
double CBytesToDouble(uint64_t bytes) {
    uint64_t convert_endiness = CBytesToLittleEndian_64(bytes);

    double result = 0.0;

    memcpy(&result, &convert_endiness, sizeof(double));

    return result;
}

/**
 * @brief Memory Error Check free
 * 
 * @param allocPtr 
 * @return void
 */
void MECFree(void* allocPtr) {
    if(allocPtr == nullptr) {
        E_WARN_ARG("Memory Error Check (MEC) Trying to free (%p @ %p) nullptr or already freed memory!", allocPtr, (void*)&allocPtr);

        return;
    }

    free(allocPtr);

    allocPtr = nullptr;
}

/**
 * @brief Memory Error Check malloc
 * 
 * @param size 
 * @return void* 
 */
void* MECMalloc(size_t size) {
    void* result = malloc(size);

    if(result == nullptr) {
        E_ERR("Memory Error Check (MEC) malloc didn`t returned valid pointer!");
        MECFree(result);
    } 

    return result;
}

/**
 * @brief Memory Error Check calloc
 * 
 * @param numberOfElements 
 * @param elementSize 
 * @return void* 
 */
void* MECCalloc(size_t numberOfElements, size_t elementSize) {
    void* result = calloc(numberOfElements, elementSize);

    if(result == nullptr) {
        E_ERR("Memory Error Check (MEC) calloc didn`t returned valid pointer!");
        MECFree(result);
    }

    return result;
}

/**
 * @brief Memory Error Check realloc
 * 
 * @param allocPtr 
 * @param size 
 * @return void* 
 */
void* MECRealloc(void* allocPtr, size_t size) {
    /*if(allocPtr == nullptr) {
        E_INFO("Memory Error Check (MEC) pointer is equal to nullptr, using MECCalloc instead!");

        allocPtr = MECCalloc(size, 1);

        return allocPtr;
    }*/

    allocPtr = realloc(allocPtr, size);

    if(allocPtr == nullptr) {
        E_ERR("Memory Error Check (MEC) realloc didn`t returned valid pointer!")
        MECFree(allocPtr);
    }

    return allocPtr;
}

/**
 * @brief Search Find string in source, slow version
 * 
 * @param searched searched string
 * @param src source
 * @return size_t 
 */
size_t SFindInString_Slow(const uint8_t* searched, const uint8_t* src) {
    for(size_t i = 0; i < strlen((char*)src); i++) {
        if(strncmp((char*)searched, (char*)src + i, strlen((char*)searched)) == 0) {
            return i;
        }
    }

    E_WARN_ARG("Cannot find \"%s\" in searched string, returuning SIZE_MAX!", searched);

    return SIZE_MAX;
}

/**
 * @brief Memory Error Check Copying some memory until value occours
 * 
 * @param dst destination
 * @param src source
 * @param size source/destination size (pick smaller value)
 * @param val value to end copying
 * @return void* 
 */
void* MECCopyMemoryUntil(void* dst, void* src, size_t size, uint8_t val) {
    int dst_counter = 0;

    if(dst == nullptr) {
        E_WARN("Memory Error Check (MEC) Specified destination doesn`t exist!");
    }

    if(src == nullptr) {
        E_WARN("Memory Error Check (MEC) Specified source doesn`t exist!");
    }

    if(size == 0) {
        E_INFO("Memory Error Check (MEC) There no size, returning dst!");

        return dst;
    }

    E_INFO_ARG("Copying less than %lu bytes", size);

    for(size_t i = 0; i < size; i++) {
        if(*(((uint8_t*)src) + i) == val) {
            break;
        }

        ((uint8_t*)dst)[dst_counter++] = *(((uint8_t*)src) + i);
    }

    return dst;
}

/**
 * @brief Copy number until find stop value
 * 
 * @param dst destination
 * @param src source
 * @param size source/destination size (pick smaller)
 * @param stopVal value that found stops function 
 * @return size_t size of copyied number + 1 to skip stop value index
 */
size_t CopyNumber(void* dst, void* src, size_t size, uint8_t stopVal) {
    size_t dst_count = 0;
    memset(dst, 0, strlen(dst));

    for(size_t i = 0; i < size; i++) {
        if(((uint8_t*)src)[i] == stopVal) {
            return i + 1;
        }

        if((*(((uint8_t*)src) + i) >= '0' && *(((uint8_t*)src) + i) <= '9') || *(((uint8_t*)src) + i) == '.' || *(((uint8_t*)src) + i) == '-') {
            ((uint8_t*)dst)[dst_count++] = *(((uint8_t*)src) + i);
        }
    }

    return size;
}

#endif