
#include "erichandmade.h"
#include "random.h"
#include "tile.cpp"
#include "render.cpp"

//~ NOTE(Eric): Init functions
internal void
InitPlayer(game_state *GameState)
{
    player Result = {};
    Result.CenterP = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    Result.FacingDirectionAngle = 180;
    Result.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(Result.FacingDirectionAngle * Pi32/180)),
                       RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(Result.FacingDirectionAngle * Pi32/180))) + Result.CenterP;
    
    GameState->Player = Result;
}

internal void
InitAsteroids(game_state *GameState)
{
    // TODO(Eric): Randomize all positions! (and size, and speed)
    random_series Series = RandomSeed(321);
    
    v2 CenterMap = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
    v2 RenderMax = V2((real32)GameState->RenderWidth, (real32)GameState->RenderHeight);
    
#define ASTEROID_COLLISION_TEST 1
#if ASTEROID_COLLISION_TEST
    
    v2 P1 = V2(100, 100);
    v2 P2 = V2(300, 200);
    v2 P3 = V2(100, 100);
    
    GameState->AsteroidCount = 0;
    
    asteroid Asteroid = {};
    Asteroid.CenterP = P1;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P2;
    Asteroid.Radius = ASTEROID_SMALL_R;
    Asteroid.State = ASTEROIDSTATE_ACTIVE;
    Asteroid.Speed = ASTEROIDSPEED_SLOW;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[0] = Asteroid;
    GameState->AsteroidCount++;
    
    Asteroid = {};
    Asteroid.CenterP = P2;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P2;
    Asteroid.Radius = ASTEROID_SMALL_R;
    Asteroid.State = ASTEROIDSTATE_ACTIVE;
    Asteroid.Speed = ASTEROIDSPEED_SLOW;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[1] = Asteroid;
    GameState->AsteroidCount++;
    /*
    Asteroid = {};
    Asteroid.CenterP = P2;
    Asteroid.StartP = Asteroid.CenterP;
    Asteroid.EndP = P1;
    Asteroid.Radius = ASTEROID_SMALL_R;
    Asteroid.State = ASTEROIDSTATE_ACTIVE;
    Asteroid.Speed = ASTEROIDSPEED_SLOW;
    Asteroid.Color = V3(1,1,1);
    GameState->Asteroids[2] = Asteroid;
    GameState->AsteroidCount++;
    */
    
    
    
#else
    u32 AsteroidCount = 2;
    
    GameState->AsteroidCount = 0;
    for(u32 AsteroidIndex = 0;
        AsteroidIndex < ArrayCount(GameState->Asteroids) && AsteroidIndex < AsteroidCount;
        ++AsteroidIndex)
    {
        v2 RandomP = {};
        
        RandomP.x = RandomBetween(&Series, (real32)0, (real32)600);
        RandomP.y = RandomBetween(&Series, (real32)0, (real32)600);
        
        asteroid Asteroid = {};
        
        Asteroid.CenterP = RandomP;
        Asteroid.StartP = Asteroid.CenterP;
        Asteroid.EndP = RenderMax - RandomP;
        
        Asteroid.Radius = ASTEROID_SMALL_R;
        Asteroid.State = ASTEROIDSTATE_ACTIVE;
        Asteroid.Speed = ASTEROIDSPEED_SLOW;
        Asteroid.Color = V3(1,1,1);
        
        //tile Tile = GetTileAtPosition(&GameState->Map, Asteroid.CenterP);
        //Asteroid.TileRelP = Asteroid.CenterP - Tile.BottomLeft;
        //Asteroid.TileIndex = GetTileIndexAtPosition(&GameState->Map, Asteroid.CenterP);
        
        GameState->Asteroids[AsteroidIndex] = Asteroid;
        GameState->AsteroidCount++;
    }
#endif
}

internal void
MoveAsteroid(asteroid *Asteroid, real32 dt)
{
    // The goal of this function is just to change the CenterP by a certain amount
    // that is along the line of StartP -> EndP
    
    // Direction == Slope
    
    v2 DeltaP = Asteroid->EndP - Asteroid->CenterP;
    real32 Distance = SquareRoot(LengthSq(DeltaP));
    
    if (Distance > 1)
    {
#if 1
        v2 ddP = {};
        
        ddP.x = (DeltaP.x/(dt*60));
        ddP.y = (DeltaP.y/(dt*60));
        
        real32 MoveX = ddP.x * (dt * Asteroid->Speed);
        real32 MoveY = ddP.y * (dt * Asteroid->Speed);
        
        Asteroid->CenterP.x += (real32)(MoveX);
        Asteroid->CenterP.y += (real32)(MoveY);
#else
        Asteroid->CenterP = Mix(Asteroid->CenterP, Asteroid->EndP, .03f);
#endif
    }
    else
    {
        Asteroid->CenterP = Asteroid->EndP;
        Asteroid->State = ASTEROIDSTATE_INACTIVE;
    }
    // 2*pos - prev_pos + dt*dt*accel
    //Asteroid->CenterP = (2 * Asteroid->CenterP) - (Asteroid->StartP);// + ((2 * dt) * Asteroid->Speed);
}

internal bool32
CheckCollisionAsteroids(asteroid Ast1, asteroid Ast2)
{
    bool32 Result = false;
    
    real32 DeltaX = Ast2.CenterP.x - Ast1.CenterP.x;
    real32 DeltaY = Ast2.CenterP.y - Ast1.CenterP.y;
    
    real32 Distance = SquareRoot(DeltaX*DeltaX + DeltaY*DeltaY);
    
    if (Distance <= ((Ast1.Radius) + (Ast2.Radius)))
        Result = true;
    
    return(Result);
}

internal void
CheckCollisions(game_state *GameState)
{
    
}



//~NOTE(Eric): Game Update and Render

// (render_buffer *Render, game_memory *Memory, game_input *Input)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth / 2.0f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight / 2.0f;
        
        
        //GameState->Map[0] =
        // Init Map
        InitMap(GameState);
        
        // Init Player
        InitPlayer(GameState);
        
        // Init Asteroids
        InitAsteroids(GameState);
        
        Memory->IsInitialized = true;
    }
    
    if (GameState->RenderWidth != Render->Width ||
        GameState->RenderHeight != Render->Height)
    {
        GameState->RenderWidth = Render->Width;
        GameState->RenderHeight = Render->Height;
        GameState->RenderHalfWidth = (real32)GameState->RenderWidth / 2.0f;
        GameState->RenderHalfHeight = (real32)GameState->RenderHeight / 2.0f;
        // Reinitialize Map
        InitMap(GameState);
        
        GameState->Player.CenterP = V2(GameState->RenderHalfWidth, GameState->RenderHalfHeight);
        GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                      RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    }
    
    
    
    game_controller_input *Controller = GetController(Input, 0);
    v2 ddP = {};
    
    // TODO(Eric): Player Movement does NOT feel good, and is based on framerate
    if(Controller->MoveUp.EndedDown)
    {
        GameState->Player.FacingDirectionAngle -= 10;
        ddP.y = 1.0f;
    }
    if(Controller->MoveDown.EndedDown)
    {
        GameState->Player.FacingDirectionAngle += 10;
        ddP.y = -1.0f;
    }
    if(Controller->MoveLeft.EndedDown)
    {
        GameState->Player.FacingDirectionAngle += 10;
        ddP.x = -1.0f;
    }
    if(Controller->MoveRight.EndedDown)
    {
        GameState->Player.FacingDirectionAngle -= 10;
        ddP.x = 1.0f;
    }
    
    // Update players position
    GameState->Player.FrontP = V2(RoundReal32(PLAYER_LENGTH_TO_CENTER * Cos(GameState->Player.FacingDirectionAngle * Pi32/180)),
                                  RoundReal32(PLAYER_LENGTH_TO_CENTER * Sin(GameState->Player.FacingDirectionAngle * Pi32/180))) + GameState->Player.CenterP;
    
    
    // NOTE(Eric): Collide Asteroids with eachother
    for(u32 AsteroidIndex1 = 0;
        AsteroidIndex1 < GameState->AsteroidCount;
        ++AsteroidIndex1)
    {
        for(u32 AsteroidIndex2 = AsteroidIndex1;
            AsteroidIndex2 < GameState->AsteroidCount;
            ++AsteroidIndex2)
        {
            if (AsteroidIndex1 == AsteroidIndex2)
                continue;
            
            asteroid *Ast1 = &GameState->Asteroids[AsteroidIndex1];
            asteroid *Ast2 = &GameState->Asteroids[AsteroidIndex2];
            
            if (CheckCollisionAsteroids(*Ast1, *Ast2))
            {
                // Calculate new EndP for both
                // First attempt - just swap the end points
                // - it didn't  actually enforce the asteroids to never go inside of eachother..
                
                // Second attempt - set the EndP of the first to the sum of both
                // Also no good, obviously
                //Ast1->EndP += Ast2->EndP;
                
                // TODO(Eric):
                // - Differentiate between a Position Vector and a Free Vector
                // -   Position Vector - bound to a specific origin
                // -   Free Vector (or just Vector) - not bound, and can be applied to any point
                // Read the Real-time collision detection book, chapter 3
                
                // So the _direction_ needs to change, and be calculated from each other.
                // The angle of both new directions has to be away from eachother, or > 0
                
                // Third attempt - calculate deltax and deltay, and subtract
                // If the Center was the Origin, this would be easier!!
                // - Then we wouldn't have to check all bounds? right?
                /*
                v2 P1d = V2((Ast1->StartP.x - Ast1->EndP.x), ((Ast1->StartP.y - Ast1->EndP.y)));
                v2 P2d = V2((Ast2->StartP.x - Ast2->EndP.x), ((Ast2->StartP.y - Ast2->EndP.y)));
                
                // Reset startP to current Pos
                Ast1->StartP = Ast1->CenterP;
                Ast2->StartP = Ast2->CenterP;
                
                Ast1->EndP = Ast1->EndP - P1d;
                Ast2->EndP = Ast2->EndP - P1d;
*/
                
                // Fourth attempt - If it's going up and to the right, swap X
                /*
                bool32 PosNegDiagonal = (Ast1->StartP < Ast1->EndP) || (Ast1->StartP > Ast1->EndP);
                if (PosNegDiagonal)
                {
                    Ast1->EndP.x = Ast1->StartP.x;
                    Ast1->StartP = Ast1->CenterP;
                }
                
                bool32 UpRight2 = (Ast2->StartP < Ast2->EndP);
                if (UpRight2)
                {
                    Ast2->EndP.x = Ast2->StartP.x;
                }
                */
                
                // Fifth attempt - calculating the Normal from the circle
                // NOTE(Eric): THIS IS ASSUMING THE ASTEROIDS ARE THE SAME SIZE
                v2 Normal = (Ast2->CenterP - Ast1->CenterP) * 0.5; // collision point from the center of Ast2
                v2 IncomingVector = (Normal) - (Ast1->CenterP);
                
                real32 DotProd = Inner(Normal, IncomingVector);
                
                v2 ReflectionVector = (IncomingVector - (2 * DotProd) * Normal);
                
                Ast1->EndP = ReflectionVector;
                
                
                RenderLine(Render, Ast2->CenterP, ReflectionVector, 1, V3(0,.5,1));
                
            }
        }
    }
    
    // NOTE(Eric): Move Asteroids
    for(u32 AsteroidIndex = 0;
        AsteroidIndex < GameState->AsteroidCount;
        ++AsteroidIndex)
    {
        asteroid *Ast = &GameState->Asteroids[AsteroidIndex];
        if (Ast)
        {
            MoveAsteroid(Ast, Input->dtForFrame);
        }
        else
            break;
    }
    
    // TODO(Eric): Collision with the player
    // My first test of getting the tiles that the player _could_ collide with probably won't be good
    // I'm thinking maybe we can just check whether it collides with any of the 3 points on the player
    // likely the asteroids won't ever be smaller than the players length
    
    //
    // NOTE(Eric): Render
    //
    ClearBackground(Render);
    RenderMap(Render, &GameState->Map);
    
    
    // Crosshairs
    v3 CrosshairColor = V3(0.5,0.5,0.5);
    RenderLine(Render,
               V2(0, GameState->RenderHalfHeight),
               V2((real32)GameState->RenderWidth, GameState->RenderHalfHeight), 1, CrosshairColor);
    
    RenderLine(Render,
               V2(GameState->RenderHalfWidth, 0),
               V2(GameState->RenderHalfWidth, (real32)GameState->RenderHeight), 1, CrosshairColor);
    
    
    
    RenderPlayer(Render, &GameState->Player, &GameState->Map);
    
    // NOTE(Eric): Render Asteroids
    for(int AsteroidIndex = 0;
        AsteroidIndex < ArrayCount(GameState->Asteroids);
        ++AsteroidIndex)
    {
        if (&GameState->Asteroids[AsteroidIndex])
        {
            RenderAsteroid(Render, &GameState->Map, &GameState->Asteroids[AsteroidIndex]);
            RenderAsteroidPositions(Render, &GameState->Asteroids[AsteroidIndex]);
        }
        else
            break;
    }
    
    /*
    // Testing random square for colliding tiles
    colliding_tiles_result CollidingTiles = GetTilesInSquare(&GameState->Map, V2(62,62), 90);
    RenderCollidingTiles(Render, CollidingTiles);
    */
    
    RenderCircle(Render, V2(500,300), 200, 1, V3(1,1,1));
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

Blog Post 5 (starting 3/31/2020):

Started an asteroid struct
my first attempt at drawing one and moving it across the screen


-- thinking --
To have different positions/coordinates
EAch 'entity' has multiple positions (in HH, AbsTileX and Y, offsetx and y, width/height, etc.
Then I think when one of the positions change, there is a function that basically syncs all location data
See function 'MoveIntoTileSpace'
-- thinking --

-- continued thinking (days later)--
This coordinate system can be whatever we want
HH had locations relative to which TileMap they are on,
as well as a postition relative to the tiles themselves (the offset from the tile's corner)

also big note.. Define why we are trying to do this coordinate system stuff
- Trying to remove the idea of 'Pixels' when we work with positions
- Working in pixels means we are constantly thinking about calculations that we don't have to
-
-- continued thinking --

 tried to just type in a ton of code (first CheckCollisions) that I didn't understand
from the '6 useful snippets' on [hashing?]
first compiling version gave a stackoverflow, so I made the arrays smaller
- still not sure how they were so big?
- also mention that the stack overflow didn't tell me anything, and I couldn't even step into the function
second compiling version didn't do anything that I wanted, really
I stepped through it, and it just didn't work
I could probably still make it work with some tweaks, but I commented it out for now

Then, I looked up raylib's collision functions, and WOW
It's actually so easy!
two circles colliding is just a calculation of the distance of their center points and radiuses added together!
my first thoughts on circle collision was to check _every_ point on the circle, and that's just completely wrong
I typed up this collision and had a test working within 5 minutes
much better time spent than the huge 'collision buckets' thingy.

What I need is a custom solution to what we are dealing with specifically here.
So armed with the collision detector with two circles, we need to check collisions for all of them
(also, realized that for asteroids 'bouncing' off eachother,
- I can simply just change the EndP! this was huge realization)
Since we will only have, like a MAX of 50 asteroids on the screen, it wouldn't be too bad to check all of them
against all the others, every time. We'll have to test this out.

I ended this post short, because I'll be doing this refactor now. I hope.

*/

/*

Blog Post 6 (starting 4/15/2020):

Ok, do the refactor! Get into it! We can do this!

Changed the 'map' to be the render width/height
- turned out to be pretty easy, didn't take much time at all

started to get into collision detection again
- got 'colliding tile' to work; maybe show 'collision_tile_result' stuff
-
-

fixed holding down input
- there was a check in the process input for WasDown != IsDown to handle it
-    added case for movement keys only

!!!!!!!!!
I think it's time to start fresh and delete a lot of stuff I've done over the past 2 weeks
All of the current asteroid stuff, I think, is just bad, and I'm kind of flailing around
I also think this is the reason that I've been demotivated.. it's just a bunch of cruft to deal with
I need to start back to simple steps. Get one 'asteroid' on the screen. move it around. collide with something.
I also really want to flesh out the map / coordinate system.
!!!!!!!!!

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

Asteroid Movement
- Going to have a random starting position and an end position(across from start, nearly crossing player)
- Random speed (set asteroid speeds, slow, med, fast?)


Asteroid Destroying
- first run can just be not rendering the asteroid anymore (changing it's state)
- Destroy them if they are off the screen
 -    We don't want to be tracking them if their trajectory is off the screen and getting further
- We could 'cycle' out inactive asteroids and create a new one in their place in the array

Collision detection
- Player and Asteroid (game over)
- Asteroid and Asteroid (bounce slightly?)



Improve player movement
- Holding down the button continuosluyouyyyy moves

Player shooting
- I would like to build a ray tracer for this.
-  Having a line draw from the frontP on the players angle would be a start



Remove all these blasted comments everywhere!



*/

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