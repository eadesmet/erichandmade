
#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

global_variable render_buffer RenderBuffer;
global_variable bool GlobalRunning = true;
global_variable bool GlobalPause = false;
global_variable s64 GlobalPerfCountFrequency;

struct win32_replay_buffer
{
    HANDLE FileHandle;
    HANDLE MemoryMap;
    char FileName[WIN32_STATE_FILE_NAME_COUNT];
    void *MemoryBlock;
};

struct win32_state
{
    u64 TotalSize;
    void *GameMemoryBlock;
    win32_replay_buffer ReplayBuffers[4];
    
    HANDLE RecordingHandle;
    int InputRecordingIndex;
    
    HANDLE PlaybackHandle;
    int InputPlayingIndex;
    
    char EXEFileName[WIN32_STATE_FILE_NAME_COUNT];
    char *OnePastLastEXEFileNameSlash;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

struct win32_game_code
{
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;
    
    // IMPORTANT(casey): Either of the callbacks can be 0!  You must
    // check before calling.
    game_update_and_render *UpdateAndRender;
    
    
    bool32 IsValid;
};

struct win32_debug_time_marker
{
    DWORD OutputPlayCursor;
    DWORD OutputWriteCursor;
    DWORD OutputLocation;
    DWORD OutputByteCount;
    DWORD ExpectedFlipPlayCursor;
    
    DWORD FlipPlayCursor;
    DWORD FlipWriteCursor;
};

internal int
StringLength(char *String)
{
    int Count = 0;
    while(*String++)
    {
        ++Count;
    }
    return(Count);
}

internal void
CatStrings(size_t SourceACount, char *SourceA,
           size_t SourceBCount, char *SourceB,
           size_t DestCount, char *Dest)
{
    // TODO(casey): Dest bounds checking!
    
    for(int Index = 0;
        Index < SourceACount;
        ++Index)
    {
        *Dest++ = *SourceA++;
    }
    
    for(int Index = 0;
        Index < SourceBCount;
        ++Index)
    {
        *Dest++ = *SourceB++;
    }
    
    *Dest++ = 0;
}

inline LARGE_INTEGER
Win32GetWallClock(void)
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result);
}

inline real32
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    real32 Result = ((real32)(End.QuadPart - Start.QuadPart) /
                     (real32)GlobalPerfCountFrequency);
    return(Result);
}



