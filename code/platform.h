#if !defined(HANDMADE_H)


#include <stdint.h>
#include <windows.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int bool32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float real32;
typedef double real64;

#define internal static 
#define local_persist static 
#define global_variable static

#define Pi32 3.14159265359f

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

#if HANDMADE_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif


struct render_buffer
{
    int Width, Height;
    real32 *Pixels;
    BITMAPINFO Bitmap;
};

typedef struct game_memory
{
    bool32 IsInitialized;
    
    u64 PermanentStorageSize;
    void *PermanentStorage; // NOTE(casey): REQUIRED to be cleared to zero at startup
    
    u64 TransientStorageSize;
    void *TransientStorage; // NOTE(casey): REQUIRED to be cleared to zero at startup
    
    //debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
    //debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    //debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
} game_memory;

typedef struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
} game_button_state;

typedef struct game_controller_input
{
    bool32 IsConnected;
    bool32 IsAnalog;    
    real32 StickAverageX;
    real32 StickAverageY;
    
    union
    {
        game_button_state Buttons[12];
        struct
        {
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;
            
            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;
            
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
            
            game_button_state Back;
            game_button_state Start;
            
            // NOTE(casey): All buttons must be added above this line
            
            game_button_state Terminator;
        };
    };
} game_controller_input;

typedef struct game_input
{
    game_button_state MouseButtons[5];
    s32 MouseX, MouseY, MouseZ;
    
    real32 dtForFrame;
    
    game_controller_input Controllers[5];
} game_input;




// TODO(ERIC): Rename "Render", actually pass in something else probably
// TODO(ERIC): Pass in user Input, set up games Memory and Buffers
#define GAME_UPDATE_AND_RENDER(name) void name(render_buffer *Render, game_memory *Memory, game_input *Input)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);



#define HANDMADE_H
#endif