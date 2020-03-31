#include "erichandmade.h"

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <xinput.h>

#include "win32_erichandmade.h"


// NOTE(casey): XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// NOTE(casey): XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_


internal void
Win32LoadXInput(void)
{
    // TODO(casey): Test this on Windows 8
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary)
    {
        // TODO(casey): Diagnostic
        XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
    }
    
    if(!XInputLibrary)
    {
        // TODO(casey): Diagnostic
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }
    
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if(!XInputGetState) {XInputGetState = XInputGetStateStub;}
        
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if(!XInputSetState) {XInputSetState = XInputSetStateStub;}
        
        // TODO(casey): Diagnostic
        
    }
    else
    {
        // TODO(casey): Diagnostic
    }
}

inline FILETIME
Win32GetLastWriteTime(char *Filename)
{
    FILETIME LastWriteTime = {};
    
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if(GetFileAttributesEx(Filename, GetFileExInfoStandard, &Data))
    {
        LastWriteTime = Data.ftLastWriteTime;
    }
    
    return(LastWriteTime);
}

internal void
Win32BuildEXEPathFileName(win32_state *State, char *FileName,
                          int DestCount, char *Dest)
{
    CatStrings(State->OnePastLastEXEFileNameSlash - State->EXEFileName, State->EXEFileName,
               StringLength(FileName), FileName,
               DestCount, Dest);
}

internal void
Win32GetEXEFileName(win32_state *State)
{
    // NOTE(casey): Never use MAX_PATH in code that is user-facing, because it
    // can be dangerous and lead to bad results.
    // NOTE(ERIC): Beacuse MAX_PATH is actually incorrect under certain circumstances.
    DWORD SizeOfFilename = GetModuleFileNameA(0, State->EXEFileName, sizeof(State->EXEFileName));
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    for(char *Scan = State->EXEFileName;
        *Scan;
        ++Scan)
    {
        if(*Scan == '\\')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
}

internal void
Win32GetInputFileLocation(win32_state *State, bool32 InputStream,
                          int SlotIndex, int DestCount, char *Dest)
{
    char Temp[64];
    wsprintf(Temp, "loop_edit_%d_%s.hmi", SlotIndex, InputStream ? "input" : "state");
    Win32BuildEXEPathFileName(State, Temp, DestCount, Dest);
}

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return(Result);
}

//~ Input, Recording
internal void
Win32ProcessKeyboardMessage(game_button_state *NewState, bool32 IsDown)
{
    if(NewState->EndedDown != IsDown)
    {
        NewState->EndedDown = IsDown;
        ++NewState->HalfTransitionCount;
    }
}

internal void
Win32ProcessXInputDigitalButton(DWORD XInputButtonState,
                                game_button_state *OldState, DWORD ButtonBit,
                                game_button_state *NewState)
{
    NewState->EndedDown = ((XInputButtonState & ButtonBit) == ButtonBit);
    NewState->HalfTransitionCount = (OldState->EndedDown != NewState->EndedDown) ? 1 : 0;
}

internal real32
Win32ProcessXInputStickValue(SHORT Value, SHORT DeadZoneThreshold)
{
    real32 Result = 0;
    
    if(Value < -DeadZoneThreshold)
    {
        Result = (real32)((Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold));
    }
    else if(Value > DeadZoneThreshold)
    {
        Result = (real32)((Value - DeadZoneThreshold) / (32767.0f - DeadZoneThreshold));
    }
    
    return(Result);
}

internal win32_replay_buffer *
Win32GetReplayBuffer(win32_state *State, int unsigned Index)
{
    Assert(Index < ArrayCount(State->ReplayBuffers));
    win32_replay_buffer *Result = &State->ReplayBuffers[Index];
    return(Result);
}

internal void
Win32BeginRecordingInput(win32_state *State, int InputRecordingIndex)
{
    win32_replay_buffer *ReplayBuffer = Win32GetReplayBuffer(State, InputRecordingIndex);
    if(ReplayBuffer->MemoryBlock)
    {
        State->InputRecordingIndex = InputRecordingIndex;
        
        char FileName[WIN32_STATE_FILE_NAME_COUNT];
        Win32GetInputFileLocation(State, true, InputRecordingIndex, sizeof(FileName), FileName);
        State->RecordingHandle = CreateFileA(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        
#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = State->TotalSize;
        SetFilePointerEx(State->RecordingHandle, FilePosition, 0, FILE_BEGIN);
#endif
        
        CopyMemory(ReplayBuffer->MemoryBlock, State->GameMemoryBlock, State->TotalSize);
    }
}

internal void
Win32EndRecordingInput(win32_state *State)
{
    CloseHandle(State->RecordingHandle);
    State->InputRecordingIndex = 0;
}

internal void
Win32BeginInputPlayBack(win32_state *State, int InputPlayingIndex)
{
    win32_replay_buffer *ReplayBuffer = Win32GetReplayBuffer(State, InputPlayingIndex);
    if(ReplayBuffer->MemoryBlock)
    {
        State->InputPlayingIndex = InputPlayingIndex;
        
        char FileName[WIN32_STATE_FILE_NAME_COUNT];
        Win32GetInputFileLocation(State, true, InputPlayingIndex, sizeof(FileName), FileName);
        State->PlaybackHandle = CreateFileA(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
        
#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = State->TotalSize;
        SetFilePointerEx(State->PlaybackHandle, FilePosition, 0, FILE_BEGIN);
#endif
        
        CopyMemory(State->GameMemoryBlock, ReplayBuffer->MemoryBlock, State->TotalSize);
    }
}

internal void
Win32EndInputPlayBack(win32_state *State)
{
    CloseHandle(State->PlaybackHandle);
    State->InputPlayingIndex = 0;
}

internal void
Win32RecordInput(win32_state *State, game_input *NewInput)
{
    DWORD BytesWritten;
    WriteFile(State->RecordingHandle, NewInput, sizeof(*NewInput), &BytesWritten, 0);
}

internal void
Win32PlayBackInput(win32_state *State, game_input *NewInput)
{
    DWORD BytesRead = 0;
    if(ReadFile(State->PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0))
    {
        if(BytesRead == 0)
        {
            // NOTE(casey): We've hit the end of the stream, go back to the beginning
            int PlayingIndex = State->InputPlayingIndex;
            Win32EndInputPlayBack(State);
            Win32BeginInputPlayBack(State, PlayingIndex);
            ReadFile(State->PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0);
        }
    }
}




internal win32_game_code
Win32LoadGameCode(char *SourceDLLName, char *TempDLLName)
{
    win32_game_code Result = {};
    
    // TODO(casey): Need to get the proper path here!
    // TODO(casey): Automatic determination of when updates are necessary.
    
    Result.DLLLastWriteTime = Win32GetLastWriteTime(SourceDLLName);
    
    Sleep(100);
    CopyFile(SourceDLLName, TempDLLName, FALSE);
    
    Result.GameCodeDLL = LoadLibraryA(TempDLLName);
    if(Result.GameCodeDLL)
    {
        Result.UpdateAndRender = (game_update_and_render *)
            GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
        
        // Result.IsValid = (Result.UpdateAndRender);
    }
    
#if 0
    if(!Result.IsValid)
    {
        Result.UpdateAndRender = 0;
    }
#endif
    
    return(Result);
}

internal void
Win32UnloadGameCode(win32_game_code *GameCode)
{
    if(GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->UpdateAndRender = 0;
}

internal void
Win32DisplayBufferInWindow(render_buffer *Buffer,
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    int OffsetX = 0; // These were 10
    int OffsetY = 0;
    
    PatBlt(DeviceContext, 0, 0, WindowWidth, OffsetY, BLACKNESS);
    PatBlt(DeviceContext, 0, OffsetY + Buffer->Height, WindowWidth, WindowHeight, BLACKNESS);
    PatBlt(DeviceContext, 0, 0, OffsetX, WindowHeight, BLACKNESS);
    PatBlt(DeviceContext, OffsetX + Buffer->Width, 0, WindowWidth, WindowHeight, BLACKNESS);
    
    // NOTE(casey): For prototyping purposes, we're going to always blit
    // 1-to-1 pixels to make sure we don't introduce artifacts with
    // stretching while we are learning to code the renderer!
    StretchDIBits(DeviceContext,
                  OffsetX, OffsetY, Buffer->Width, Buffer->Height,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Pixels,
                  &Buffer->Bitmap,
                  DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT
Win32WindowCallback(HWND Window, UINT Message, WPARAM w_param, LPARAM l_param)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            GlobalRunning = 0;
        } break;
        case WM_SIZE:
        {
            RECT WindowRect;
            GetClientRect(Window, &WindowRect);
            RenderBuffer.Width = WindowRect.right - WindowRect.left;
            RenderBuffer.Height = WindowRect.bottom - WindowRect.top;
            
            if (RenderBuffer.Pixels)
            {
                // free
                VirtualFree(RenderBuffer.Pixels, 0, MEM_RELEASE);
            }
            
            int BytesPerPixel = 4;
            int BufferSize = RenderBuffer.Width * RenderBuffer.Height * BytesPerPixel;
            
            // NOTE(Eric): This was replaced by BytesPerPixel
            // sizeof(unsigned int);
            
            // allocate the buffer
            RenderBuffer.Pixels = VirtualAlloc(0, BufferSize,
                                               MEM_COMMIT|MEM_RESERVE,
                                               PAGE_READWRITE);
            
            // fill the bimapinfo
            RenderBuffer.Bitmap.bmiHeader.biSize = sizeof(RenderBuffer.Bitmap.bmiHeader);
            RenderBuffer.Bitmap.bmiHeader.biWidth = RenderBuffer.Width;
            RenderBuffer.Bitmap.bmiHeader.biHeight = RenderBuffer.Height;
            RenderBuffer.Bitmap.bmiHeader.biPlanes = 1;
            RenderBuffer.Bitmap.bmiHeader.biBitCount = 32;
            RenderBuffer.Bitmap.bmiHeader.biCompression = BI_RGB;
        }
        default:
        {
            Result = DefWindowProcA(Window, Message, w_param, l_param);
        }
    }
    
    return (Result);
}

internal void
Win32ProcessPendingMessages(win32_state *State, game_controller_input *KeyboardController)
{
    MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            } break;
            
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 VKCode = (u32)Message.wParam;
                
                // NOTE(casey): Since we are comparing WasDown to IsDown,
                // we MUST use == and != to convert these bit tests to actual
                // 0 or 1 values.
                bool32 WasDown = ((Message.lParam & (1 << 30)) != 0);
                bool32 IsDown = ((Message.lParam & (1 << 31)) == 0);
                if(WasDown != IsDown)
                {
                    if(VKCode == 'W')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveUp, IsDown);
                    }
                    else if(VKCode == 'A')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveLeft, IsDown);
                    }
                    else if(VKCode == 'S')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveDown, IsDown);
                    }
                    else if(VKCode == 'D')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveRight, IsDown);
                    }
                    else if(VKCode == 'Q')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->LeftShoulder, IsDown);
                    }
                    else if(VKCode == 'E')
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->RightShoulder, IsDown);
                    }
                    else if(VKCode == VK_UP)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionUp, IsDown);
                    }
                    else if(VKCode == VK_LEFT)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionLeft, IsDown);
                    }
                    else if(VKCode == VK_DOWN)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionDown, IsDown);
                    }
                    else if(VKCode == VK_RIGHT)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionRight, IsDown);
                    }
                    else if(VKCode == VK_ESCAPE)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Start, IsDown);
                        
                        GlobalRunning = false;
                    }
                    else if(VKCode == VK_SPACE)
                    {
                        Win32ProcessKeyboardMessage(&KeyboardController->Back, IsDown);
                    }
#if HANDMADE_INTERNAL
                    else if(VKCode == 'P')
                    {
                        if(IsDown)
                        {
                            GlobalPause = !GlobalPause;
                        }
                    }
                    else if(VKCode == 'L')
                    {
                        if(IsDown)
                        {
                            if(State->InputPlayingIndex == 0)
                            {
                                if(State->InputRecordingIndex == 0)
                                {
                                    Win32BeginRecordingInput(State, 1);
                                }
                                else
                                {
                                    Win32EndRecordingInput(State);
                                    Win32BeginInputPlayBack(State, 1);
                                }
                            }
                            else
                            {
                                Win32EndInputPlayBack(State);
                            }
                        }
                    }
#endif
                }
                
                bool32 AltKeyWasDown = (Message.lParam & (1 << 29));
                if((VKCode == VK_F4) && AltKeyWasDown)
                {
                    GlobalRunning = false;
                }
            } break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
}

int
WinMain(HINSTANCE h_instance, HINSTANCE prev_instance,LPSTR Command, int ShowCommand)
{
    //char MessageBuffer[500];
    //sprintf_s(MessageBuffer, "Size of uint: %zd", sizeof(unsigned int));
    //OutputDebugStringA(MessageBuffer);
    
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    
    win32_state Win32State = {};
    Win32GetEXEFileName(&Win32State);
    
    char SourceGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&Win32State, "erichandmade.dll",
                              sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);
    
    char TempGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&Win32State, "erichandmade_temp.dll",
                              sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);
    
    Win32LoadXInput();
    
    WNDCLASSA WindowClass =  {0};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32WindowCallback;
    WindowClass.lpszClassName = "Game_Window_Class";
    
    if (RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "Eric Handmade",
                                      WS_VISIBLE|WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                      1280, 720, 0, 0, 0, 0);
        if (Window)
        {
            // Timings
            int MonitorRefreshHz = 60;
            HDC RefreshDC = GetDC(Window);
            int Win32RefreshRate = GetDeviceCaps(RefreshDC, VREFRESH);
            ReleaseDC(Window, RefreshDC);
            if (Win32RefreshRate > 1)
            {
                MonitorRefreshHz = Win32RefreshRate;
            }
            real32 GameUpdateHz = (MonitorRefreshHz / 2.0f);
            real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;
            
            // NOTE(casey): Set the Windows scheduler granularity to 1ms
            // so that our Sleep() can be mre ganular.
            UINT DesiredSchedulerMS = 1;
            bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
            
            // Init GameMemory and pass it through to the game
#if HANDMADE_INTERNAL
            LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
            LPVOID BaseAddress = 0;
#endif
            game_memory GameMemory = {};
            GameMemory.PermanentStorageSize = Megabytes(64);
            GameMemory.TransientStorageSize = Megabytes(256);
            //GameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
            //GameMemory.DEBUGPlatformReadEntireFile = DEBUGPlatformReadEntireFile;
            //GameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;
            
            Win32State.TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
            Win32State.GameMemoryBlock = VirtualAlloc(BaseAddress, (size_t)Win32State.TotalSize,
                                                      MEM_RESERVE|MEM_COMMIT,
                                                      PAGE_READWRITE);
            GameMemory.PermanentStorage = Win32State.GameMemoryBlock;
            GameMemory.TransientStorage = ((u8 *)GameMemory.PermanentStorage +
                                           GameMemory.PermanentStorageSize);
            
            // Gameplay Recording/replay would go here
            for(int ReplayIndex = 0;
                ReplayIndex < ArrayCount(Win32State.ReplayBuffers);
                ++ReplayIndex)
            {
                win32_replay_buffer *ReplayBuffer = &Win32State.ReplayBuffers[ReplayIndex];
                
                // TODO(casey): Recording system still seems to take too long
                // on record start - find out what Windows is doing and if
                // we can speed up / defer some of that processing.
                
                Win32GetInputFileLocation(&Win32State, false, ReplayIndex,
                                          sizeof(ReplayBuffer->FileName), ReplayBuffer->FileName);
                
                ReplayBuffer->FileHandle =
                    CreateFileA(ReplayBuffer->FileName,
                                GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS, 0, 0);
                
                LARGE_INTEGER MaxSize;
                MaxSize.QuadPart = Win32State.TotalSize;
                ReplayBuffer->MemoryMap = CreateFileMapping(ReplayBuffer->FileHandle, 0, PAGE_READWRITE,
                                                            MaxSize.HighPart, MaxSize.LowPart, 0);
                
                ReplayBuffer->MemoryBlock = MapViewOfFile(ReplayBuffer->MemoryMap, FILE_MAP_ALL_ACCESS,
                                                          0, 0, Win32State.TotalSize);
                if(ReplayBuffer->MemoryBlock)
                {
                }
                else
                {
                    // TODO(casey): Diagnostic
                }
            }
            
            game_input Input[2] = {};
            game_input *NewInput = &Input[0];
            game_input *OldInput = &Input[1];
            
            LARGE_INTEGER LastCounter = Win32GetWallClock();
            LARGE_INTEGER FlipWallClock = Win32GetWallClock();
            
            int DebugTimeMarkerIndex = 0;
            win32_debug_time_marker DebugTimeMarkers[30] = {0};
            
            win32_game_code GameCode = Win32LoadGameCode(SourceGameCodeDLLFullPath, TempGameCodeDLLFullPath);
            
            u64 LastCycleCount = __rdtsc();
            while(GlobalRunning)
            {
                NewInput->dtForFrame = TargetSecondsPerFrame;
                
                FILETIME NewDLLWriteTime = Win32GetLastWriteTime(SourceGameCodeDLLFullPath);
                int FileTimeResult = CompareFileTime(&NewDLLWriteTime, &GameCode.DLLLastWriteTime);
                if(FileTimeResult != 0)
                {
                    Win32UnloadGameCode(&GameCode);
                    GameCode = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                                                 TempGameCodeDLLFullPath);
                    //LoadCounter = 0;
                }
                
                // Input
                // TODO(casey): Zeroing macro
                // TODO(casey): We can't zero everything because the up/down state will
                // be wrong!!!
                game_controller_input *OldKeyboardController = GetController(OldInput, 0);
                game_controller_input *NewKeyboardController = GetController(NewInput, 0);
                *NewKeyboardController = {};
                NewKeyboardController->IsConnected = true;
                for(int ButtonIndex = 0;
                    ButtonIndex < ArrayCount(NewKeyboardController->Buttons);
                    ++ButtonIndex)
                {
                    NewKeyboardController->Buttons[ButtonIndex].EndedDown =
                        OldKeyboardController->Buttons[ButtonIndex].EndedDown;
                }
                
                Win32ProcessPendingMessages(&Win32State, NewKeyboardController);
                
                if (!GlobalPause)
                {
                    POINT MouseP;
                    GetCursorPos(&MouseP);
                    ScreenToClient(Window, &MouseP);
                    NewInput->MouseX = MouseP.x;
                    NewInput->MouseY = MouseP.y;
                    NewInput->MouseZ = 0; // TODO(casey): Support mousewheel?
                    Win32ProcessKeyboardMessage(&NewInput->MouseButtons[0],
                                                GetKeyState(VK_LBUTTON) & (1 << 15));
                    Win32ProcessKeyboardMessage(&NewInput->MouseButtons[1],
                                                GetKeyState(VK_MBUTTON) & (1 << 15));
                    Win32ProcessKeyboardMessage(&NewInput->MouseButtons[2],
                                                GetKeyState(VK_RBUTTON) & (1 << 15));
                    Win32ProcessKeyboardMessage(&NewInput->MouseButtons[3],
                                                GetKeyState(VK_XBUTTON1) & (1 << 15));
                    Win32ProcessKeyboardMessage(&NewInput->MouseButtons[4],
                                                GetKeyState(VK_XBUTTON2) & (1 << 15));
                    
                    // TODO(casey): Need to not poll disconnected controllers to avoid
                    // xinput frame rate hit on older libraries...
                    // TODO(casey): Should we poll this more frequently
                    DWORD MaxControllerCount = XUSER_MAX_COUNT;
                    if(MaxControllerCount > (ArrayCount(NewInput->Controllers) - 1))
                    {
                        MaxControllerCount = (ArrayCount(NewInput->Controllers) - 1);
                    }
                    
                    for (DWORD ControllerIndex = 0;
                         ControllerIndex < MaxControllerCount;
                         ++ControllerIndex)
                    {
                        DWORD OurControllerIndex = ControllerIndex + 1;
                        game_controller_input *OldController = GetController(OldInput, OurControllerIndex);
                        game_controller_input *NewController = GetController(NewInput, OurControllerIndex);
                        
                        XINPUT_STATE ControllerState;
                        if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                        {
                            NewController->IsConnected = true;
                            NewController->IsAnalog = OldController->IsAnalog;
                            
                            // NOTE(casey): This controller is plugged in
                            // TODO(casey): See if ControllerState.dwPacketNumber increments too rapidly
                            XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                            
                            // TODO(casey): This is a square deadzone, check XInput to
                            // verify that the deadzone is "round" and show how to do
                            // round deadzone processing.
                            NewController->StickAverageX = Win32ProcessXInputStickValue(
                                                                                        Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            NewController->StickAverageY = Win32ProcessXInputStickValue(
                                                                                        Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            if((NewController->StickAverageX != 0.0f) ||
                               (NewController->StickAverageY != 0.0f))
                            {
                                NewController->IsAnalog = true;
                            }
                            
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP)
                            {
                                NewController->StickAverageY = 1.0f;
                                NewController->IsAnalog = false;
                            }
                            
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
                            {
                                NewController->StickAverageY = -1.0f;
                                NewController->IsAnalog = false;
                            }
                            
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                            {
                                NewController->StickAverageX = -1.0f;
                                NewController->IsAnalog = false;
                            }
                            
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                            {
                                NewController->StickAverageX = 1.0f;
                                NewController->IsAnalog = false;
                            }
                            
                            real32 Threshold = 0.5f;
                            Win32ProcessXInputDigitalButton(
                                                            (NewController->StickAverageX < -Threshold) ? 1 : 0,
                                                            &OldController->MoveLeft, 1,
                                                            &NewController->MoveLeft);
                            Win32ProcessXInputDigitalButton(
                                                            (NewController->StickAverageX > Threshold) ? 1 : 0,
                                                            &OldController->MoveRight, 1,
                                                            &NewController->MoveRight);
                            Win32ProcessXInputDigitalButton(
                                                            (NewController->StickAverageY < -Threshold) ? 1 : 0,
                                                            &OldController->MoveDown, 1,
                                                            &NewController->MoveDown);
                            Win32ProcessXInputDigitalButton(
                                                            (NewController->StickAverageY > Threshold) ? 1 : 0,
                                                            &OldController->MoveUp, 1,
                                                            &NewController->MoveUp);
                            
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->ActionDown, XINPUT_GAMEPAD_A,
                                                            &NewController->ActionDown);
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->ActionRight, XINPUT_GAMEPAD_B,
                                                            &NewController->ActionRight);
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->ActionLeft, XINPUT_GAMEPAD_X,
                                                            &NewController->ActionLeft);
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->ActionUp, XINPUT_GAMEPAD_Y,
                                                            &NewController->ActionUp);
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER,
                                                            &NewController->LeftShoulder);
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER,
                                                            &NewController->RightShoulder);
                            
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->Start, XINPUT_GAMEPAD_START,
                                                            &NewController->Start);
                            Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                            &OldController->Back, XINPUT_GAMEPAD_BACK,
                                                            &NewController->Back);
                        }
                        else
                        {
                            // NOTE(casey): The controller is not available
                            NewController->IsConnected = false;
                        }
                    }
                    
                    
                    // Simulation
                    //OutputDebugStringA("Calling GameUpdateAndRender\n");
                    GameCode.UpdateAndRender(&RenderBuffer, &GameMemory, NewInput);
                    
                    // NOTE(ERIC): Enforce 30 fps
                    LARGE_INTEGER WorkCounter = Win32GetWallClock();
                    real32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);
                    
                    // TODO(casey): NOT TESTED YET!  PROBABLY BUGGY!!!!!
                    real32 SecondsElapsedForFrame = WorkSecondsElapsed;
                    if(SecondsElapsedForFrame < TargetSecondsPerFrame)
                    {
                        if(SleepIsGranular)
                        {
                            DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame -
                                                               SecondsElapsedForFrame));
                            if(SleepMS > 0)
                            {
                                Sleep(SleepMS);
                            }
                        }
                        
                        real32 TestSecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter,
                                                                                   Win32GetWallClock());
                        if(TestSecondsElapsedForFrame < TargetSecondsPerFrame)
                        {
                            // TODO(casey): LOG MISSED SLEEP HERE
                        }
                        
                        while(SecondsElapsedForFrame < TargetSecondsPerFrame)
                        {
                            SecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter,
                                                                            Win32GetWallClock());
                        }
                    }
                    else
                    {
                        // TODO(casey): MISSED FRAME RATE!
                        // TODO(casey): Logging
                    }
                    
                    LARGE_INTEGER EndCounter = Win32GetWallClock();
                    real32 MSPerFrame = 1000.0f*Win32GetSecondsElapsed(LastCounter, EndCounter);
                    LastCounter = EndCounter;
                    
                    
                    // Render
                    HDC DeviceContext = GetDC(Window);
                    win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                    Win32DisplayBufferInWindow(&RenderBuffer, DeviceContext, Dimension.Width, Dimension.Height);
                    //StretchDIBits(DeviceContext, 0, 0, RenderBuffer.Width, RenderBuffer.Height,
                    //              0, 0, RenderBuffer.Width, RenderBuffer.Height, RenderBuffer.Pixels,
                    //              &RenderBuffer.Bitmap, DIB_RGB_COLORS, SRCCOPY);
                    ReleaseDC(Window, DeviceContext);
                    
#if 1
                    u64 EndCycleCount = __rdtsc();
                    u64 CyclesElapsed = EndCycleCount - LastCycleCount;
                    LastCycleCount = EndCycleCount;
                    
                    real64 FPS = 0.0f;
                    real64 MCPF = ((real64)CyclesElapsed / (1000.0f * 1000.0f));
                    
                    char FPSBuffer[256];
                    _snprintf_s(FPSBuffer, sizeof(FPSBuffer),
                                "%.02fms/f,  %.02ff/s,  %.02fmc/f\n", MSPerFrame, FPS, MCPF);
                    OutputDebugStringA(FPSBuffer);
#endif
                }
            }
        }
    }
    
    return (0);
}