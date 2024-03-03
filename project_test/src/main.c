#include <stdio.h>
#include "../../engine/window.h"
#include "../../engine/renderer.h"

void defHandler() {

}

Mesh_t gTestMesh;
Renderer_t gRend;
MeshData_t gMeshData;
RenderData_t gRenderData;

void start() {
    MClearMesh(&gTestMesh);
    MLoadPLYMeshFromFile(&gTestMesh, "../cubeBin.ply");

    MDAddMesh(&gMeshData, gTestMesh);
    MDRejoin(&gMeshData);

    RDBindMesh(&gRenderData, &gMeshData);

    // RTestSetup(&gRend);
}

void update() {
    // RRender(&gRend, &gRenderData, GL_TRIANGLES, false);
}

int main() {
    Window_t wnd;

    WSetStart(&wnd, start);
    WSetUpdate(&wnd, update);

    WSetAwake(&wnd, defHandler);
    WSetEnd(&wnd, defHandler);
    WSetLateUpdate(&wnd, defHandler);
    WSetFixedUpdate(&wnd, defHandler);

    WRun(&wnd, 800, 600, "Window");

    return 0;
}