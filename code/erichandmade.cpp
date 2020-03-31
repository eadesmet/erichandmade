
#include "erichandmade.h"
#include "tile.cpp"
#include "render.cpp"

//~ NOTE(Eric): Init functions
internal player
InitPlayer(game_state *GameState)
{
    player Result = {};
    Result.CenterP = GameState->Map.Tiles[TILE_COUNT_Y/2][TILE_COUNT_X/2].BottomLeft;
    Result.FacingDirectionAngle = 180;
    Result.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(Result.FacingDirectionAngle * Pi32/180)),
                       RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(Result.FacingDirectionAngle * Pi32/180))) + Result.CenterP;
    
    return(Result);
}

internal asteroid
InitAsteroid(game_state *GameState)
{
    asteroid Result = {};
    
    // TODO(Eric): Randomize all positions!
    
    Result.CenterP = V2(600, 600);
    Result.Radius = ASTEROID_SMALL_R;
    Result.State = ASTEROIDSTATE_ACTIVE;
    Result.Speed = ASTEROIDSPEED_SLOW;
    
    Result.StartP = Result.CenterP;
    v2 CenterMap = V2(MAP_WIDTH/2, MAP_HEIGHT/2);
    Result.EndP = CenterMap;// - (Result.StartP - CenterMap);
    
    return(Result);
}

//~ NOTE(Eric): Unused Player Movement code
// Might need some of these formulas for Asteroids moving on the screen
#if 0
internal void
MovePlayer(game_state *GameState, real32 dt, v2 ddP)
{
    // NOTE(Eric): Player Movement notes:
    // The Center point will always remain the same!
    // Front point will change, then the calculation for the other two
    // will have to be from the front calculated with the center and width
    
    // The FrontP has to be around a circle!
    
    // TODO(Eric): Pass in the entity that's moving
    real32 ddPLength = LengthSq(ddP);
    if(ddPLength > 1.0f)
    {
        ddP *= (1.0f / SquareRoot(ddPLength));
    }
    
    real32 PlayerSpeed = .1f; // m/s^2
    ddP *= PlayerSpeed;
    
    ddP += -8.0f*GameState->DeltaPlayerPosition;
    
    v2 OldPlayerP = GameState->PlayerPosition;
    v2 PlayerDelta = (0.5f*ddP*Square(dt) +
                      GameState->DeltaPlayerPosition*dt);
    GameState->DeltaPlayerPosition = ddP*dt + GameState->DeltaPlayerPosition;
    v2 NewPlayerP = OldPlayerP + PlayerDelta;
    
    GameState->PlayerPosition = NewPlayerP;
    
    
    // TODO(Eric): Collition With Player here!
}
#endif

internal void
MoveAsteroid(asteroid *Asteroid, real32 dt)
{
    // The goal of this function is just to change the CenterP by a certain amount
    // that is along the line of StartP -> EndP
    
    // Direction == Slope
    real32 Direction = ((Asteroid->EndP.y - Asteroid->StartP.y) /
                        AbsoluteValue(Asteroid->EndP.x - Asteroid->StartP.x));
    Asteroid->CenterP.x += (Direction * (dt * Asteroid->Speed));
    Asteroid->CenterP.y += (Direction * (dt * Asteroid->Speed));
}


//~NOTE(Eric): Game Update and Render

// (render_buffer *Render, game_memory *Memory, game_input *Input)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        //u32 TileCountY = MAP_HEIGHT / TILE_SIZE;
        //u32 TileCountX = MAP_WIDTH / TILE_SIZE;
        for (u32 IndexY = 0; IndexY < TILE_COUNT_Y; ++IndexY)
        {
            for (u32 IndexX = 0; IndexX < TILE_COUNT_X; ++IndexX)
            {
                v2 NewPoint = V2((real32)(OFFSET_X + (IndexX * TILE_SIZE)),
                                 (real32)(OFFSET_Y + (IndexY * TILE_SIZE)));
                
                //RenderSquare(Render, NewPoint, v2{2,2}, 1, 0, 0);
                
                GameState->Map.Tiles[IndexY][IndexX].BottomLeft = NewPoint;
            }
        }
        
        // Init Player
        GameState->Player = InitPlayer(GameState);
        
        
        
        
        // Init Asteroids
#if 0
        for(int AsteroidIndex = 0;
            AsteroidIndex < ArrayCount(GameState->Asteroids);
            ++AsteroidIndex)
        {
            asteroid Asteroid = GameState->Asteroids[AsteroidIndex];
        }
#endif
        GameState->Asteroids[0] = InitAsteroid(GameState);
        
        Memory->IsInitialized = true;
    }
    
    
    // Get Input from Controllers and Adjust player's movement
    for(int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers);
        ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input, ControllerIndex);
        
        v2 ddP = {};
        
        if(Controller->IsAnalog)
        {
            // NOTE(casey): Use analog movement tuning
            ddP = v2{Controller->StickAverageX, Controller->StickAverageY};
        }
        else
        {
            // TODO(Eric): Handle movement when the button is held down
            // Maybe we can add IsDown to the game_input_state?
            // NOTE(casey): Use digital movement tuning
            if(Controller->MoveUp.EndedDown)
            {
                GameState->Player.FacingDirectionAngle -= 5;
                ddP.y = 1.0f;
            }
            if(Controller->MoveDown.EndedDown)
            {
                GameState->Player.FacingDirectionAngle += 5;
                ddP.y = -1.0f;
            }
            if(Controller->MoveLeft.EndedDown)
            {
                GameState->Player.FacingDirectionAngle += 5;
                ddP.x = -1.0f;
            }
            if(Controller->MoveRight.EndedDown)
            {
                GameState->Player.FacingDirectionAngle -= 5;
                ddP.x = 1.0f;
            }
        }
        
        //MovePlayer(GameState, Input->dtForFrame, ddP);
        GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                      RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    }
    
    // NOTE(Eric): Single for now, either loop here or change to MoveAsteroids
    MoveAsteroid(&GameState->Asteroids[0], Input->dtForFrame);
    
    //
    // NOTE(Eric): Render
    //
    ClearBackground(Render);
    RenderMap(Render, &GameState->Map);
    
    // NOTE(Eric): Input.MouseX and Input.MouseY origin is the Top Left corner!
    // This doesn't match the coords of what render_buffer uses!
    
    // NOTE(Eric): Movement is essentially:
    // Getting the old position
    // Calculating their 'speed' based on dtForFrame
    // Setting their new position: old position + speed
    
    
#if 0
    // Drawing a grid
    int GridSize = 20;
    for (real32 y = 0; y <= Render->Height; y += GridSize)
    {
        // Draw horizontal lines
        v2 P1 = v2{0, y};
        v2 P2 = v2{(real32)Render->Width, y};
        RenderLine(Render, P1, P2, 1, 0.4, 0.4, 0.4);
    }
    for (real32 x = 0; x <= Render->Width; x += GridSize)
    {
        // Draw vertical lines
        v2 P1 = v2{x, 0};
        v2 P2 = v2{x, (real32)Render->Height};
        RenderLine(Render, P1, P2, 1, 0.4, 0.4, 0.4);
    }
#endif
    
    
    
    
    // Crosshairs
    v3 CrosshairColor = V3(0.5,0.5,0.5);
    RenderLine(Render,
               V2(OFFSET_X, (MAP_HEIGHT / 2)+OFFSET_Y),
               V2(MAP_WIDTH+OFFSET_X, (MAP_HEIGHT / 2)+OFFSET_Y), 1, CrosshairColor);
    
    RenderLine(Render,
               V2((MAP_WIDTH/2)+OFFSET_X, OFFSET_Y),
               V2((MAP_WIDTH/2)+OFFSET_X, MAP_HEIGHT+OFFSET_Y), 1, CrosshairColor);
    
    
    
    RenderPlayer(Render, &GameState->Player, GameState->Map);
    
    // NOTE(Eric): Single for now, either loop here or change to RenderAsteroids
    RenderAsteroid(Render, &GameState->Asteroids[0]);
    
    
    
}







/*

Blog post 1 (3/8/2020):

Thinking about how to draw the player (going to be a triangle).

I broke it down into simpler steps, step 1 being just 'draw a line'.
Then I figured I can just draw 3 lines on 3 points to get the triangle.
I also figured early on that I will 'draw rectangle' on each point.

Drawing a line took me a long time because I just don't know simple math..

Finally discovered y = mx + b, and the calculation for the slope of a line.

and now I'm drawing a line!! woohoo!

Oh, and early on I was thinking the hot reloading isn't working,
but it was all just because I wasn't clearing the render buffer between loads
so it would always have the old drawing there too.
Then created ClearBackground call every time.

Blog post Day 1, Part 2? (3/9/2020):

I mostly fixed the line between the two points issue
(it was about 20 pixles too much in both x and y)
It was what I was using for B - The y-intercept is not just the y coord of the first point
it has to be calculated from the other variables.

however, the blue line is now too low?

My god..

Showed my co-worker the problem, and he solves the whole thing in 2 minutes.
A couple code changes, it's working.

We only need 1 loop, for all the x's, and we are calculating y.
The slope and y-intercept are constants, based off of the points.

Also, drew a little cheeky border

Next step.. transforms??
Drawing a triangle (player) and turning it


*/

/*

Blog post 2 (3/16/2020);

I drew a grid, and I have my first drawing a circle.

Drawing a circle went super smoothly, suprisingly.
this one, I just looked up the math around it, and coded it.
super simple, and it just worked.

Fixed a bug in RenderLine - Checking if length+width are negative, I'm swapping the points

Our next big problem is actually going to be mapping out our positions,
meaning we have to store our current map, and where things are located
so that we can eventually do collision detection



*/

/*

Blog Post 3 (starting 3/17/2020):

Started by cleaning up some code, basically just moving it around to where it should be
(no functions in header files, created a couple new files)

got rid of my Clamp calls and added Asserts instead

Refactored Pixels to be void *
Refactored v2 to be real32
Added color values as real32 R, G and B
Added game_state structure and initializing it with Memory

When I finally got all the above changes compiling, my broken screenshot appears
    Multiple players are drawn.. how??
Grid dots are just _everywhere_

The grid dots got fixed, that was from a refactor in calling DrawMap I believe..
I think it was an issue with rendersquare??

Then the dots just everywhere, it was a miscalculation from renderline
I noticed that when I changed my points by only small values, the dots would change drastically
it was plotting a point, then the next point was down 1 in y, and about 80 pixels to the right
and so on, it was wrapping around i think
the x positions were consistently off, I believe the y positions were always correct

the fix goes to Bresenham's line algorithm
I just implemented this line for line and it works perfectly for all lines
https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

RenderCircle has also been broken
changed this to Bresenham's circle algorithm and it's almost working




*/

/*

Blog Post 4 (starting 3/24/2020):

Cleaned up a couple of other things
Removed all the old sample drawing things
Changed draw grid to draw outlines instead (much more useful)
Drawing crosshairs over grid to show center

Changed RenderPlayer to take a player struct
Changed RenderPlayer to draw using the points in the player struct
Player initialization is setting the points for player

realized that the player movement is always dependent on the FrontP
and that FrontP is around a circle!

- lots of testing, trying to get the top and bottom points of the player.. :(

now show moreissues.png
The pink square should be inside the blue square
and our small circle is obviously off
my first guess is a rounding error, maybe inside of rendersquare.
that was correct! rendersquare_fix.png
the change was to Round X and Y loop variables

we are getting close - the player works for pretty much every angle except 140-180
this likely has to do with if OppositeAngle >= 320
I was adjusting Bottom for both cases, when the first is Bottom and second is Top
that fixed it
[firstplayermovement.gif]


Work on something first, until you get stuck, _then_ read about _that specific problem_
this gives you context
otherwise, reading something where you don't know anything, it could be good or bad..


*/

/*
Just a couple ideas:

We could add a screen shake to the game fairly easily
all we'd need to do is adjust the offset x and y randomly
would put these in gamestate


*/

/*

Notes from Early HH:

CHange our color to 3 floating point values
These are easier to calculate and blend against
(Values between 0 and 1)
Straight up casting a float to an integer TRUNCATES the number: 1.89 -> 1.0

Remember there are steps to moving things..
It all has to be calculated up front, and then rendered with the new calculated positions.

*/


//~ TODO LIST
/*

Change rendering to be mapped only to the Map
     - I think essentially what I have to do here..
- To say that OFFSET_X and OFFSET_Y is my new 0,0
- and that MAP_WIDTH + OFFSET_X is my new Max Width
- and that MAP_HEIGHT + OFFSET_Y is my new Max Height

-- The functionality we want out of this, is asteroids to be half drawn as they come in to the map

Asteroid Movement
- Going to have a random starting position and an end position(across from start, nearly crossing player)
- Random speed (set asteroid speeds, slow, med, fast?)


Asteroid Destroying
- first run can just be not rendering the asteroid anymore (changing it's state)

Collision detection
- Player and Asteroid (game over)
- Asteroid and Asteroid (bounce slightly?)



Improve player movement
- Holding down the button continuosluyouyyyy moves

Player shooting






*/