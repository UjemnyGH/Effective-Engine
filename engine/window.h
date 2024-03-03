#ifndef _EFFECTIVE_WINDOW_
#define _EFFECTIVE_WINDOW_

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <threads.h>
#include <stdint.h>

#include "core.h"

typedef void (*PFN_WindowFunction)();

typedef struct Window_s {
    GLFWwindow* mWindow;
    PFN_WindowFunction mStartFn, mUpdateFn, mLateUpdateFn, mFixedUpdateFn, mEndFn, mAwakeFn;
    bool mFixedUpdateRunning;
    uint32_t mFixedUpdateFramerate;
} Window_t;

struct Time_s {
    double deltaTime, currentTime, lastTime;
    double fixedDeltaTime, currentFixedTime, lastFixedTime;

    struct timespec mFixedTimeSpec;
} gTime;

/**
 * @brief Set Start function
 * 
 * @param pWnd window pointer
 * @param fn function
 */
void WSetStart(Window_t* pWnd, PFN_WindowFunction fn) { pWnd->mStartFn = fn; }

/**
 * @brief Set Awake function
 * 
 * @param pWnd window pointer
 * @param fn function
 */
void WSetAwake(Window_t* pWnd, PFN_WindowFunction fn) { pWnd->mAwakeFn = fn; }

/**
 * @brief Set Update function
 * 
 * @param pWnd window pointer
 * @param fn function
 */
void WSetUpdate(Window_t* pWnd, PFN_WindowFunction fn) { pWnd->mUpdateFn = fn; }

/**
 * @brief Set Late Update function
 * 
 * @param pWnd window pointer
 * @param fn function
 */
void WSetLateUpdate(Window_t* pWnd, PFN_WindowFunction fn) { pWnd->mLateUpdateFn = fn; }

/**
 * @brief Set Fixed Update function
 * 
 * @param pWnd window pointer
 * @param fn function
 */
void WSetFixedUpdate(Window_t* pWnd, PFN_WindowFunction fn) { pWnd->mFixedUpdateFn = fn; }

/**
 * @brief Set End function
 * 
 * @param pWnd window pointer
 * @param fn function
 */
void WSetEnd(Window_t* pWnd, PFN_WindowFunction fn) { pWnd->mEndFn = fn; }

/**
 * @brief DO NOT TOUCH THIS, this is main handler of fixed update thread
 * 
 * @param pWnd 
 * @return int 
 */
int __WRunFixedUpdate(void* pWnd) {
    while(((Window_t*)pWnd)->mFixedUpdateRunning) {
        timespec_get(&gTime.mFixedTimeSpec, TIME_UTC);
        gTime.currentFixedTime = (double)gTime.mFixedTimeSpec.tv_nsec * 1000000.0;

        ((Window_t*)pWnd)->mFixedUpdateFn();

        gTime.lastFixedTime = gTime.currentFixedTime;

        while(((double)gTime.mFixedTimeSpec.tv_nsec * 1000000.0) - gTime.lastFixedTime < 1.0 / (double)((Window_t*)pWnd)->mFixedUpdateFramerate) { timespec_get(&gTime.mFixedTimeSpec, TIME_UTC); }

        gTime.fixedDeltaTime = ((double)gTime.mFixedTimeSpec.tv_nsec * 1000000.0) - gTime.lastFixedTime;
    }

    return 0;
} 

/**
 * @brief Runs window
 * 
 * @param pWnd window struct pointer
 * @param width window width
 * @param height window height
 * @param title window title
 */
void WRun(Window_t* pWnd, int width, int height, const char* title) {
    pWnd->mFixedUpdateFramerate = 128;
    pWnd->mFixedUpdateRunning = true;

    if(pWnd->mAwakeFn != nullptr) pWnd->mAwakeFn(); 

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWnd->mWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if(!pWnd->mWindow) {
        //printf("[ERROR]: In %s at %d line in function %s :> Cannot create window!\n", __FILE__, __LINE__, __FUNCTION__);
        E_ERR("Cannot create window!")

        return;
    }

    glfwMakeContextCurrent(pWnd->mWindow);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        //printf("[ERROR]: In %s at %d line in function %s :> Cannot load OpenGL 4.5 Core context\n", __FILE__, __LINE__, __FUNCTION__);
        E_ERR("Cannot load OpenGL 4.5 Core context!")

        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(pWnd->mStartFn != nullptr) pWnd->mStartFn();

    thrd_t fixedUpdate;

    thrd_create(&fixedUpdate, __WRunFixedUpdate, (void*)pWnd);
    thrd_detach(fixedUpdate);

    while(!(pWnd->mFixedUpdateRunning = glfwWindowShouldClose(pWnd->mWindow))) {
        glClear(0x100 | 0x4000);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        if(pWnd->mUpdateFn != nullptr) pWnd->mUpdateFn();

        glfwSwapBuffers(pWnd->mWindow);

        if(pWnd->mLateUpdateFn != nullptr) pWnd->mLateUpdateFn();

        glfwPollEvents();
    }

    if(pWnd->mEndFn != nullptr) pWnd->mEndFn();

    glfwTerminate();
    
    return;
}

#endif