#include<iostream>
#include<string>
#include<filesystem>
namespace fs = std::filesystem;

#include "raylib.h"

#include<taglib/tag.h>
#include<taglib/fileref.h>
#include<taglib/tstring.h>


void DrawAlignedText(int alignment /*0 is centered and 1 is left*/, const char* text, int posX, int posY, int fontSize, Color color) {
    int textCenter = MeasureText(text, fontSize) / 2;
    switch (alignment)
    {
    case 0:
    {
        DrawText(text, posX - textCenter, posY, fontSize, color);
        break;
    }
    case 1:
    {
        DrawText(text, posX - textCenter * 2, posY, fontSize, color); //the left side thingy still needs to be fixed
        break;
    }
    default:
        break;
    }
}

struct songMD {
    std::string title;
    std::string artist;
    std::string album;
    bool isValid{};
};

int ClampInt(int n,int min,int max) {
    if (n > max) (n = max);
    else if (n < min) (n = min);
    else {return n;}
}

songMD GetSongMetaData(const std::string& songFile, const std::string& directoryPath) {
    songMD metadata;

    TagLib::FileRef song(songFile.c_str());
    
    if (song.isNull() or !song.tag()) {
        std::cerr << "invalid song file" << '\n';
        std::cout << songFile;
        return metadata;
    }
    

    metadata.title = song.tag()->title().toCString();
    metadata.artist = song.tag()->artist().toCString();
    metadata.album = song.tag()->album().toCString();
    metadata.isValid = true;
    
    return metadata;
}



int main()
{
    const int screenWidth = 800;
    const int screenHeight = 640;
    const int screenMiddleH = screenHeight / 2;
    const int screenMiddleW = screenWidth / 2;

    TagLib::String songName;
    TagLib::String artistName;
    TagLib::String albumName;

    bool playing{};

    InitWindow(screenWidth, screenHeight, "gplayer");
    InitAudioDevice();

    SetTargetFPS(60);

    

    //the directory where the song files are stored

    std::string directoryPath = "C:/Users/gus/Music";

    //-------------------

    std::vector<std::string> songNames;
    try {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            //check whether the file that it's looking for isn't a directory or things like the filesystem library itself
            if (fs::is_regular_file(entry.status())) {
                fs::path currentPath = entry.path(); //get the directory path
                std::string fileExtention = currentPath.extension().string(); //get the file extention
                if (fileExtention == ".ogg") {
                    songNames.push_back(currentPath.filename().string());
                }
            }
            else {
                std::cerr << "not a regular file" << std::endl;
            }
        } 
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "filesystem error of type " << ex.what();
    }
    
    /*for (std::string file : songNames) {
        std::cout << file;
    }*/

    int songIndex = 1;

    Sound song = LoadSound((directoryPath + "/" + songNames[songIndex]).c_str());
    songMD songMetaData = GetSongMetaData((directoryPath + "/" + songNames[songIndex]), directoryPath);

    //game loop
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) (playing = !playing);
        if (playing == true) {
            PlaySound(song);
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            
            StopSound(song);
            UnloadSound(song);
            Sound song = LoadSound((directoryPath + "/" + songNames[songIndex]).c_str());
            songMD songMetaData = GetSongMetaData((directoryPath + "/" + songNames[songIndex]), directoryPath);
            ClampInt(songIndex++, 1, songNames.size());

        } else if (IsKeyPressed(KEY_LEFT)) {
            StopSound(song);
            UnloadSound(song);
            Sound song = LoadSound((directoryPath + "/" + songNames[songIndex]).c_str());
            songMD songMetaData = GetSongMetaData((directoryPath + "/" + songNames[songIndex]),directoryPath);
            ClampInt(songIndex--, 1, songNames.size());
        }

        TagLib::String songName = songMetaData.title;
        TagLib::String artistName = songMetaData.artist;
        TagLib::String albumName = songMetaData.album;

        //draw text
        BeginDrawing();
            ClearBackground(WHITE);
            DrawCircle(screenMiddleW, screenMiddleH, 3, BLACK);
            DrawAlignedText(0, songName.toCString(), screenMiddleW, 20, 24, BLACK);
            DrawAlignedText(1, albumName.toCString(), screenMiddleW - 50, 50, 18, BLACK);
            DrawText(artistName.toCString(), screenMiddleW + 50, 50, 18, BLACK);
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();

    return 0;

}
