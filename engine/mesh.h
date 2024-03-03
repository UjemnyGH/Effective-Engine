#ifndef _EFFECTIVE_MESH_
#define _EFFECTIVE_MESH_

#include "gl_buffers.h"
#include "math3d.h"
#include "core.h"

typedef struct Mesh_s {
    float* mVertices;
    float* mNormals;
    float* mTextureCoordinates;
    float* mColors;
    size_t mMeshSize;
} Mesh_t;

void MAllocMesh(Mesh_t* pMesh, size_t size) {
    pMesh->mMeshSize = size;

    pMesh->mVertices = (float*)MECRealloc(pMesh->mVertices, pMesh->mMeshSize * sizeof(float) * 3);
    pMesh->mNormals = (float*)MECRealloc(pMesh->mNormals, pMesh->mMeshSize * sizeof(float) * 3);
    pMesh->mTextureCoordinates = (float*)MECRealloc(pMesh->mTextureCoordinates, pMesh->mMeshSize * sizeof(float) * 2);
    pMesh->mColors = (float*)MECRealloc(pMesh->mColors, pMesh->mMeshSize * sizeof(float) * 4);
}

void MFreeMesh(Mesh_t* pMesh) {
    pMesh->mMeshSize = 0;

    MECFree(pMesh->mVertices);
    MECFree(pMesh->mNormals);
    MECFree(pMesh->mTextureCoordinates);
    MECFree(pMesh->mColors);
}

void MClearMesh(Mesh_t* pMesh) {
    pMesh->mMeshSize = 0;

    pMesh->mVertices = nullptr;
    pMesh->mNormals = nullptr;
    pMesh->mTextureCoordinates = nullptr;
    pMesh->mColors = nullptr;
}

void MLoadPLYMeshFromFile(Mesh_t* pMesh, const char* path) {
    FILE* mesh_file = fopen(path, "rb+");

    fseek(mesh_file, 0, SEEK_END);
    size_t mesh_length = ftell(mesh_file);
    fseek(mesh_file, 0, SEEK_SET);


    uint8_t *source_buffer = nullptr;
    source_buffer = (uint8_t*)MECCalloc(mesh_length, sizeof(uint8_t));

    fread(source_buffer, sizeof(uint8_t), mesh_length, mesh_file);

    if(source_buffer[0] != 'p' && source_buffer[1] != 'l' && source_buffer[2] != 'y') {
        fclose(mesh_file);

        char warn_string[1024] = "Cannot load model \"";
        strcat(warn_string, path);
        strcat(warn_string, "\" becouse it`s not ply model!");

        E_WARN(warn_string);

        MECFree(source_buffer);

        return;
    }

    size_t header_end = SFindInString_Slow((uint8_t*)"end_header", source_buffer) + 11;
    size_t vertex_element = SFindInString_Slow((uint8_t*)"element vertex ", source_buffer) + 15;
    size_t face_element = SFindInString_Slow((uint8_t*)"element face ", source_buffer) + 13;
    size_t format_ascii = SFindInString_Slow((uint8_t*)"format ascii 1.0", source_buffer);
    if(format_ascii == SIZE_MAX) { E_INFO("Ignore upper warning, it pops when binary format of .ply is used!") }
    size_t prop_vertices = SFindInString_Slow((uint8_t*)"property float x\n", source_buffer);
    size_t prop_normals = SFindInString_Slow((uint8_t*)"property float nx\n", source_buffer);
    if(prop_normals == SIZE_MAX) { E_INFO("Ignore upper warning, it pops when there is no normals data generated in .ply file!") }
    size_t prop_texturepos = SFindInString_Slow((uint8_t*)"property float s\n", source_buffer);
    if(prop_texturepos == SIZE_MAX) { E_INFO("Ignore upper warning, it pops when there is no texure data generated in .ply file!") }

    char numbers_buffer[1024];

    MECCopyMemoryUntil(numbers_buffer, source_buffer + vertex_element, 256, '\n');
    uint32_t vertex_amount = atoi(numbers_buffer);

    memset(numbers_buffer, 0, 1024);
    MECCopyMemoryUntil(numbers_buffer, source_buffer + face_element, 256, '\n');
    uint32_t face_amount = atoi(numbers_buffer);

    E_INFO_ARG("Model data: %u vertex, %u faces", vertex_amount, face_amount);

    fseek(mesh_file, 0, SEEK_SET);

    char *line = nullptr; 
    line = (char*)MECCalloc(mesh_length, sizeof(char));
    uint32_t vertex_counter = 0;
    uint32_t face_counter = 0;

    Mesh_t temp;
    size_t mv_counter = 0;
    size_t mn_counter = 0;
    size_t mt_counter = 0;
    MClearMesh(&temp);

    MAllocMesh(&temp, vertex_amount);

    while(!feof(mesh_file)) {
        uint8_t ch = 0;

        if(format_ascii != SIZE_MAX){
            fgets(line, (size_t)mesh_length, mesh_file);
        }
        else {
            ch = fgetc(mesh_file);
        }

        if(ftell(mesh_file) > (long)header_end) {
            if(format_ascii != SIZE_MAX) {
                if(strcmp(line, "end_header\n") == 0) { continue; }

                if(vertex_counter < vertex_amount) {
                    char number[256];
                    size_t current_pos = 0;
                    
                    if(prop_vertices != SIZE_MAX) {
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mVertices[mv_counter++] = atof(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mVertices[mv_counter++] = atof(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mVertices[mv_counter++] = atof(number);
                    }

                    if(prop_normals != SIZE_MAX) {
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mNormals[mn_counter++] = atof(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mNormals[mn_counter++] = atof(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mNormals[mn_counter++] = atof(number);
                    }

                    if(prop_texturepos != SIZE_MAX) {
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        temp.mTextureCoordinates[mt_counter++] = atof(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, '\n');
                        temp.mTextureCoordinates[mt_counter++] = atof(number);
                    }

                    vertex_counter++;

                    continue;
                }
                
                if(face_counter < face_amount) {
                    char number[256];
                    size_t current_pos = 0;

                    current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                    int faces_value_amount = atoi(number);

                    if(faces_value_amount == 4) {
                        uint32_t face[4] = {0, 0, 0, 0};

                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        face[0] = atoi(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        face[1] = atoi(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        face[2] = atoi(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, '\n');
                        face[3] = atoi(number);

                        MAllocMesh(pMesh, pMesh->mMeshSize + 6);

                        pMesh->mVertices[(pMesh->mMeshSize - 6) * 3 + 0] = temp.mVertices[face[0] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 6) * 3 + 1] = temp.mVertices[face[0] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 6) * 3 + 2] = temp.mVertices[face[0] * 3 + 2];
                    
                        pMesh->mVertices[(pMesh->mMeshSize - 5) * 3 + 0] = temp.mVertices[face[1] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 5) * 3 + 1] = temp.mVertices[face[1] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 5) * 3 + 2] = temp.mVertices[face[1] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 4) * 3 + 0] = temp.mVertices[face[2] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 4) * 3 + 1] = temp.mVertices[face[2] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 4) * 3 + 2] = temp.mVertices[face[2] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mVertices[face[0] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mVertices[face[0] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mVertices[face[0] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mVertices[face[1] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mVertices[face[1] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mVertices[face[1] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mVertices[face[3] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mVertices[face[3] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mVertices[face[3] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 6) * 3 + 0] = temp.mNormals[face[0] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 6) * 3 + 1] = temp.mNormals[face[0] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 6) * 3 + 2] = temp.mNormals[face[0] * 3 + 2];
                    
                        pMesh->mNormals[(pMesh->mMeshSize - 5) * 3 + 0] = temp.mNormals[face[1] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 5) * 3 + 1] = temp.mNormals[face[1] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 5) * 3 + 2] = temp.mNormals[face[1] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 4) * 3 + 0] = temp.mNormals[face[2] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 4) * 3 + 1] = temp.mNormals[face[2] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 4) * 3 + 2] = temp.mNormals[face[2] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mNormals[face[0] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mNormals[face[0] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mNormals[face[0] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mNormals[face[1] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mNormals[face[1] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mNormals[face[1] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mNormals[face[3] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mNormals[face[3] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mNormals[face[3] * 3 + 2];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 6) * 2 + 0] = temp.mTextureCoordinates[face[0] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 6) * 2 + 1] = temp.mTextureCoordinates[face[0] * 2 + 1];
                    
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 5) * 2 + 0] = temp.mTextureCoordinates[face[1] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 5) * 2 + 1] = temp.mTextureCoordinates[face[1] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 4) * 2 + 0] = temp.mTextureCoordinates[face[2] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 4) * 2 + 1] = temp.mTextureCoordinates[face[2] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 0] = temp.mTextureCoordinates[face[0] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 1] = temp.mTextureCoordinates[face[0] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 0] = temp.mTextureCoordinates[face[1] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 1] = temp.mTextureCoordinates[face[1] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 0] = temp.mTextureCoordinates[face[3] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 1] = temp.mTextureCoordinates[face[3] * 2 + 1];
                    }
                    else {
                        uint32_t face[3] = {};

                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        face[0] = atoi(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, ' ');
                        face[1] = atoi(number);
                        current_pos += CopyNumber(number, line + current_pos, 256, '\n');
                        face[2] = atoi(number);

                        MAllocMesh(pMesh, pMesh->mMeshSize + 3);

                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mVertices[face[0] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mVertices[face[0] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mVertices[face[0] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mVertices[face[1] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mVertices[face[1] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mVertices[face[1] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mVertices[face[2] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mVertices[face[2] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mVertices[face[2] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mNormals[face[0] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mNormals[face[0] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mNormals[face[0] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mNormals[face[1] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mNormals[face[1] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mNormals[face[1] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mNormals[face[2] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mNormals[face[2] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mNormals[face[2] * 3 + 2];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 0] = temp.mTextureCoordinates[face[0] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 1] = temp.mTextureCoordinates[face[0] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 0] = temp.mTextureCoordinates[face[1] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 1] = temp.mTextureCoordinates[face[1] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 0] = temp.mTextureCoordinates[face[2] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 1] = temp.mTextureCoordinates[face[2] * 2 + 1];
                    }

                    face_counter++;

                    continue;
                }
            }
            else {
                if(vertex_counter < vertex_amount) {                   
                    if(prop_vertices != SIZE_MAX) {
                        temp.mVertices[mv_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/(((uint32_t)ch << 24) | ((uint32_t)fgetc(mesh_file) << 16) | ((uint32_t)fgetc(mesh_file) << 8) | (uint32_t)fgetc(mesh_file)));
                        temp.mVertices[mv_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/(((uint32_t)fgetc(mesh_file) << 24) | ((uint32_t)fgetc(mesh_file) << 16) | ((uint32_t)fgetc(mesh_file) << 8) | (uint32_t)fgetc(mesh_file)));
                        temp.mVertices[mv_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/(((uint32_t)fgetc(mesh_file) << 24) | ((uint32_t)fgetc(mesh_file) << 16) | ((uint32_t)fgetc(mesh_file) << 8) | (uint32_t)fgetc(mesh_file)));
                    }

                    if(prop_normals != SIZE_MAX) {
                        temp.mNormals[mn_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file)));
                        temp.mNormals[mn_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file)));
                        temp.mNormals[mn_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file)));
                    }

                    if(prop_texturepos != SIZE_MAX) {
                        temp.mTextureCoordinates[mt_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file)));
                        temp.mTextureCoordinates[mt_counter++] = CBytesToFloat(/*CBytesToLittleEndian*/((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file)));
                    }

                    vertex_counter++;

                    continue;
                }
                
                if(face_counter < face_amount) {
                    char faces_value_amount = (char)ch;

                    if(faces_value_amount == 4) {
                        uint32_t face[4] = {};

                        face[0] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));
                        face[1] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));
                        face[2] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));
                        face[3] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));

                        MAllocMesh(pMesh, pMesh->mMeshSize + 6);

                        pMesh->mVertices[(pMesh->mMeshSize - 6) * 3 + 0] = temp.mVertices[face[0] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 6) * 3 + 1] = temp.mVertices[face[0] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 6) * 3 + 2] = temp.mVertices[face[0] * 3 + 2];
                    
                        pMesh->mVertices[(pMesh->mMeshSize - 5) * 3 + 0] = temp.mVertices[face[1] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 5) * 3 + 1] = temp.mVertices[face[1] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 5) * 3 + 2] = temp.mVertices[face[1] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 4) * 3 + 0] = temp.mVertices[face[2] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 4) * 3 + 1] = temp.mVertices[face[2] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 4) * 3 + 2] = temp.mVertices[face[2] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mVertices[face[0] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mVertices[face[0] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mVertices[face[0] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mVertices[face[1] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mVertices[face[1] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mVertices[face[1] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mVertices[face[3] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mVertices[face[3] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mVertices[face[3] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 6) * 3 + 0] = temp.mNormals[face[0] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 6) * 3 + 1] = temp.mNormals[face[0] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 6) * 3 + 2] = temp.mNormals[face[0] * 3 + 2];
                    
                        pMesh->mNormals[(pMesh->mMeshSize - 5) * 3 + 0] = temp.mNormals[face[1] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 5) * 3 + 1] = temp.mNormals[face[1] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 5) * 3 + 2] = temp.mNormals[face[1] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 4) * 3 + 0] = temp.mNormals[face[2] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 4) * 3 + 1] = temp.mNormals[face[2] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 4) * 3 + 2] = temp.mNormals[face[2] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mNormals[face[0] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mNormals[face[0] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mNormals[face[0] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mNormals[face[1] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mNormals[face[1] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mNormals[face[1] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mNormals[face[3] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mNormals[face[3] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mNormals[face[3] * 3 + 2];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 6) * 2 + 0] = temp.mTextureCoordinates[face[0] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 6) * 2 + 1] = temp.mTextureCoordinates[face[0] * 2 + 1];
                    
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 5) * 2 + 0] = temp.mTextureCoordinates[face[1] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 5) * 2 + 1] = temp.mTextureCoordinates[face[1] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 4) * 2 + 0] = temp.mTextureCoordinates[face[2] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 4) * 2 + 1] = temp.mTextureCoordinates[face[2] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 0] = temp.mTextureCoordinates[face[0] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 1] = temp.mTextureCoordinates[face[0] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 0] = temp.mTextureCoordinates[face[1] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 1] = temp.mTextureCoordinates[face[1] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 0] = temp.mTextureCoordinates[face[3] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 1] = temp.mTextureCoordinates[face[3] * 2 + 1];
                    }
                    else {
                        uint32_t face[3] = {};

                        face[0] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));
                        face[1] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));
                        face[2] = CBytesToLittleEndian((uint32_t)fgetc(mesh_file) << 24 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file));

                        MAllocMesh(pMesh, pMesh->mMeshSize + 3);

                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mVertices[face[0] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mVertices[face[0] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mVertices[face[0] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mVertices[face[1] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mVertices[face[1] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mVertices[face[1] * 3 + 2];

                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mVertices[face[2] * 3 + 0];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mVertices[face[2] * 3 + 1];
                        pMesh->mVertices[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mVertices[face[2] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 0] = temp.mNormals[face[0] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 1] = temp.mNormals[face[0] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 3) * 3 + 2] = temp.mNormals[face[0] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 0] = temp.mNormals[face[1] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 1] = temp.mNormals[face[1] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 2) * 3 + 2] = temp.mNormals[face[1] * 3 + 2];

                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 0] = temp.mNormals[face[2] * 3 + 0];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 1] = temp.mNormals[face[2] * 3 + 1];
                        pMesh->mNormals[(pMesh->mMeshSize - 1) * 3 + 2] = temp.mNormals[face[2] * 3 + 2];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 0] = temp.mTextureCoordinates[face[0] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 3) * 2 + 1] = temp.mTextureCoordinates[face[0] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 0] = temp.mTextureCoordinates[face[1] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 2) * 2 + 1] = temp.mTextureCoordinates[face[1] * 2 + 1];

                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 0] = temp.mTextureCoordinates[face[2] * 2 + 0];
                        pMesh->mTextureCoordinates[(pMesh->mMeshSize - 1) * 2 + 1] = temp.mTextureCoordinates[face[2] * 2 + 1];
                    }

                    face_counter++;

                    continue;
                }
            }
        }
    }

    MFreeMesh(&temp);
    MECFree(line);
    MECFree(source_buffer);
    fclose(mesh_file);

    for(size_t i = 0; i < pMesh->mMeshSize; i++) {
        pMesh->mColors[i * 4 + 0] = 1.0f;
        pMesh->mColors[i * 4 + 1] = 1.0f;
        pMesh->mColors[i * 4 + 2] = 1.0f;
        pMesh->mColors[i * 4 + 3] = 1.0f;
    }

    return;
}

typedef struct MeshData_s {
    Mesh_t* mMeshes;
    Transform_t* mMeshTransform;
    Mesh_t mJoinedMesh;
    Transform_t mTransform;
    float* mTextureID;
    uint32_t* mMeshStart;

    uint32_t mMeshCount;
} MeshData_t;

void __MDCalculateMeshEnds(MeshData_t* pData) {
    pData->mMeshStart = MECRealloc(pData->mMeshStart, sizeof(uint32_t) * pData->mMeshCount);

    for(uint32_t i = 0; i < pData->mMeshCount; i++) {
        pData->mMeshStart[i] = (i == 0 ? 0 : pData->mMeshStart[i - 1]) + (i == 0 ? 0 : pData->mMeshes[i - 1].mMeshSize);
    }
}

void MDRejoin(MeshData_t *pData) {
    MECFree(pData->mMeshStart);
    MFreeMesh(&pData->mJoinedMesh);
    __MDCalculateMeshEnds(pData);

    for(uint32_t i = 0; i < pData->mMeshCount; i++) {
        MAllocMesh(&pData->mJoinedMesh, pData->mJoinedMesh.mMeshSize + pData->mMeshes[i].mMeshSize);

        for(uint32_t j = 0; j < pData->mMeshes[i].mMeshSize; j++) {
            vec4_t mat_pos = MX4MulV(pData->mMeshTransform[i].mTransformMat, (vec4_t){pData->mMeshes[i].mVertices[j * 3 + 0], pData->mMeshes[i].mVertices[j * 3 + 1], pData->mMeshes[i].mVertices[j * 3 + 2], 1.0});

            pData->mJoinedMesh.mVertices[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 3 + 0] = mat_pos.x;
            pData->mJoinedMesh.mVertices[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 3 + 1] = mat_pos.y;
            pData->mJoinedMesh.mVertices[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 3 + 2] = mat_pos.z;

            pData->mJoinedMesh.mNormals[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 3 + 0] = pData->mMeshes[i].mNormals[j * 3 + 0];
            pData->mJoinedMesh.mNormals[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 3 + 1] = pData->mMeshes[i].mNormals[j * 3 + 1];
            pData->mJoinedMesh.mNormals[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 3 + 2] = pData->mMeshes[i].mNormals[j * 3 + 2];

            pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 4 + 0] = pData->mMeshes[i].mColors[j * 4 + 0];
            pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 4 + 1] = pData->mMeshes[i].mColors[j * 4 + 1];
            pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 4 + 2] = pData->mMeshes[i].mColors[j * 4 + 2];
            pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 4 + 3] = pData->mMeshes[i].mColors[j * 4 + 3];

            pData->mJoinedMesh.mTextureCoordinates[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 2 + 0] = pData->mMeshes[i].mTextureCoordinates[j * 2 + 0];
            pData->mJoinedMesh.mTextureCoordinates[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[i].mMeshSize) + j * 2 + 1] = pData->mMeshes[i].mTextureCoordinates[j * 2 + 1];
        }
    }
}

void MDAddMesh(MeshData_t* pData, Mesh_t mesh) {
    pData->mMeshes = MECRealloc(pData->mMeshes, sizeof(Mesh_t) * (++pData->mMeshCount));
    pData->mMeshes[pData->mMeshCount - 1] = mesh;
    pData->mMeshTransform = MECRealloc(pData->mMeshTransform, sizeof(Transform_t) * pData->mMeshCount);
    pData->mTextureID = MECRealloc(pData->mTextureID, sizeof(float) * ((pData->mMeshCount <= 1 ? 0 : (pData->mMeshStart[pData->mMeshCount - 2] + pData->mMeshes[pData->mMeshCount - 2].mMeshSize)) + pData->mMeshes[pData->mMeshCount - 1].mMeshSize));

    TFSetScale(&pData->mMeshTransform[pData->mMeshCount - 1], (vec4_t){1.0, 1.0, 1.0, 1.0});

    MECFree(pData->mMeshStart);
    __MDCalculateMeshEnds(pData);

    for(uint32_t i = 0; i < pData->mMeshes[pData->mMeshCount - 1].mMeshSize; i++) {
        pData->mTextureID[(pData->mMeshCount <= 1 ? 0 : (pData->mMeshStart[pData->mMeshCount - 2] + pData->mMeshes[pData->mMeshCount - 2].mMeshSize)) + i] = 33.0f;
    }

    MAllocMesh(&pData->mJoinedMesh, pData->mJoinedMesh.mMeshSize + pData->mMeshes[pData->mMeshCount - 1].mMeshSize);

    for(uint32_t j = 0; j < pData->mMeshes[pData->mMeshCount - 1].mMeshSize; j++) {
        vec4_t mat_pos = MX4MulV(pData->mMeshTransform[pData->mMeshCount - 1].mTransformMat, (vec4_t){pData->mMeshes[pData->mMeshCount - 1].mVertices[j * 3 + 0], pData->mMeshes[pData->mMeshCount - 1].mVertices[j * 3 + 1], pData->mMeshes[pData->mMeshCount - 1].mVertices[j * 3 + 2], 1.0});

        pData->mJoinedMesh.mVertices[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 3 + 0] = mat_pos.x;
        pData->mJoinedMesh.mVertices[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 3 + 1] = mat_pos.y;
        pData->mJoinedMesh.mVertices[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 3 + 2] = mat_pos.z;

        pData->mJoinedMesh.mNormals[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 3 + 0] = pData->mMeshes[pData->mMeshCount - 1].mNormals[j * 3 + 0];
        pData->mJoinedMesh.mNormals[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 3 + 1] = pData->mMeshes[pData->mMeshCount - 1].mNormals[j * 3 + 1];
        pData->mJoinedMesh.mNormals[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 3 + 2] = pData->mMeshes[pData->mMeshCount - 1].mNormals[j * 3 + 2];

        pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 4 + 0] = pData->mMeshes[pData->mMeshCount - 1].mColors[j * 4 + 0];
        pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 4 + 1] = pData->mMeshes[pData->mMeshCount - 1].mColors[j * 4 + 1];
        pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 4 + 2] = pData->mMeshes[pData->mMeshCount - 1].mColors[j * 4 + 2];
        pData->mJoinedMesh.mColors[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 4 + 3] = pData->mMeshes[pData->mMeshCount - 1].mColors[j * 4 + 3];

        pData->mJoinedMesh.mTextureCoordinates[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 2 + 0] = pData->mMeshes[pData->mMeshCount - 1].mTextureCoordinates[j * 2 + 0];
        pData->mJoinedMesh.mTextureCoordinates[(pData->mJoinedMesh.mMeshSize - pData->mMeshes[pData->mMeshCount - 1].mMeshSize) + j * 2 + 1] = pData->mMeshes[pData->mMeshCount - 1].mTextureCoordinates[j * 2 + 1];
    }
}

typedef struct RenderData_s {
    MeshData_t* mMeshPtr;

    VArray_t mVArray;
    VBuffer_t mVerticesBuffer, mColorBuffer, mNormalBuffer, mTextureCoordinatesBuffer, mTextureIDBuffer;
    TextureArray_t *mTexturesPtr[32];
} RenderData_t;

void RDUpdateMesh(RenderData_t* pRd) {
    VABind(&pRd->mVArray);

    VBBindData(&pRd->mVerticesBuffer, pRd->mMeshPtr->mJoinedMesh.mVertices, /*sizeof(float) */ (pRd->mMeshPtr->mJoinedMesh.mMeshSize - 1) * 3);
    printf("Debug\n"); fflush(stdout);
    VBBindData(&pRd->mColorBuffer, pRd->mMeshPtr->mJoinedMesh.mVertices, sizeof(float) * pRd->mMeshPtr->mJoinedMesh.mMeshSize * 4);
    VBBindData(&pRd->mNormalBuffer, pRd->mMeshPtr->mJoinedMesh.mVertices, sizeof(float) * pRd->mMeshPtr->mJoinedMesh.mMeshSize * 3);
    VBBindData(&pRd->mTextureCoordinatesBuffer, pRd->mMeshPtr->mJoinedMesh.mVertices, sizeof(float) * pRd->mMeshPtr->mJoinedMesh.mMeshSize * 2);
    VBBindData(&pRd->mTextureIDBuffer, pRd->mMeshPtr->mTextureID, sizeof(float) * pRd->mMeshPtr->mJoinedMesh.mMeshSize);

    VBBindPlace(&pRd->mVerticesBuffer, 0, 3);
    VBBindPlace(&pRd->mColorBuffer, 1, 4);
    VBBindPlace(&pRd->mNormalBuffer, 2, 3);
    VBBindPlace(&pRd->mTextureCoordinatesBuffer, 3, 2);
    VBBindPlace(&pRd->mTextureIDBuffer, 4, 1);
    
    VAUnbind();
}

void RDBindMesh(RenderData_t* pRd, MeshData_t* pMesh) {
    pRd->mMeshPtr = pMesh;
    
    RDUpdateMesh(pRd);
}

#endif