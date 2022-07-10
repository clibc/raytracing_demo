#pragma once

#include <string>
#include <fstream>

char* ReadEntireFile(const char* path, int* data_length = NULL) {
    std::ifstream file;
    file.open(path);

    if(!file.is_open()) {
        DebugLog("Failed to load file");
        exit(-1);
    }

    file.seekg(0, file.end);
    s32 length = file.tellg();
    file.seekg(0, file.beg);
    char* buffer = new char [length + 1];
    file.read(buffer, length);
    file.close();

    buffer[length] = 0;
    
    if(data_length != NULL) *data_length = length;
    
    return buffer;
}

static f32 Values[10000];
static s32 Faces[6000];

void
LoadOBJ(char* FilePath, v3* Vertices, u32* Count)
{
    s32 Size = 0;
    char* FileContent = ReadEntireFile(FilePath, &Size);

    const char LineDelim[2] = {'\n'};
    const char ValueDelim[2] = {' '};

    s32 ValuesCount = 0;
    s32 FacesCount  = 0;
    
    for(;;)
    {
        char* Line = strtok(FileContent, LineDelim);

        if(!Line)
        {
            break;
        }

        FileContent += strlen(Line) + 1;

        if(*Line == 'v')
        {
            Line += 1;
            char* V1 = strtok(Line, ValueDelim);
            char* V2 = strtok(NULL, ValueDelim);
            char* V3 = strtok(NULL, ValueDelim);
            Values[ValuesCount++] = atof(V1);
            Values[ValuesCount++] = atof(V2);
            Values[ValuesCount++] = atof(V3);
        }
        else if(*Line == 'f')
        {
            Line += 1;
            char* F1 = strtok(Line, ValueDelim);
            char* F2 = strtok(NULL, ValueDelim);
            char* F3 = strtok(NULL, ValueDelim);
            Faces[FacesCount++] = atof(F1);
            Faces[FacesCount++] = atof(F2);
            Faces[FacesCount++] = atof(F3);
        }
    }

    s32 VertexIndex = 0;
        
    for(s32 I = 0; I < FacesCount; I += 1)
    {
        s32 Index = (Faces[I] - 1) * 3;
        Vertices[VertexIndex++] = v3(Values[Index + 0],
                                     Values[Index + 1],
                                     Values[Index + 2]);
    }

    *Count = VertexIndex;
}
