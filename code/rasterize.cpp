

// NOTE(Eric): This is mostly just version 1 from fabian - Should not actually be used anywhere
// Over the rest of his post, this will be optimized
// https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/

/*
DrawTriangle(Render,
                 GPToSP(GameState->Player.BackLeftP, false),
                 GPToSP(GameState->Player.FrontP, false),
                 GPToSP(GameState->Player.BackRightP, false));
*/



internal real32
Orient2d(v2 A, v2 B, v2 C)
{
	real32 Result;

	Result = (B.x-A.x)*(C.y-A.y) - (B.y-A.y)*(C.x-A.x);

	return (Result);
}

void
DrawTriangle(render_buffer *Render, v2 V0, v2 V1, v2 V2)
{
    // TODO(eric): Didn't quite finish the post; it gets quite mathy

	// First version of DrawTriangle. Limits the loop to a bounding box
    // Problems:
    // - Integer Overflows (we are using reals though)
    //      Our range for p is [-16384,16383]
    // - Sub-pixel precision: this code doesn't have any
    // - Fill rules: tie-breaking rules to make sure overlapping edges, each pixel is drawn once
    // - Speed


	// Compute triangle bounding box
    real32 minX = Min3(V0.x, V1.x, V2.x);
    real32 minY = Min3(V0.y, V1.y, V2.y);
    real32 maxX = Max3(V0.x, V1.x, V2.x);
    real32 maxY = Max3(V0.y, V1.y, V2.y);
#if 0
    // Clip against screen bounds
    minX = Maximum(minX, 0);
    minY = Maximum(minY, 0);
    maxX = Minimum(maxX, Render->Width - 1);
    maxY = Minimum(maxY, Render->Height - 1);

    // Rasterize
    v2 p;
    for (p.y = minY; p.y <= maxY; p.y++) {
        for (p.x = minX; p.x <= maxX; p.x++) {
            // Determine barycentric coordinates
            real32 w0 = Orient2d(V1, V2, p);
            real32 w1 = Orient2d(V2, V0, p);
            real32 w2 = Orient2d(V0, V1, p);

            // If p is on or inside all edges, render pixel.
            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                RenderSquare(Render, p, 1, V3(1,0,1));
        }
    }



    // Applying sub-pixel precision
    // "You want at least 4 extra bits in each coordinate (i.e. coordinates are specified in 1/16ths of a pixel"
    // "Let's assume 8 bits of sub-pixel precision. The trivial way to get this is to multiply everything by 256"
    // "our (still integer) coordinates are now in 1/256ths of a pixel, but we still only perform one sample each"
    // "pixel."

    const int SubStep = 256;
    const int SubMask = SubStep - 1;

    // Round start position up to next integer multiple
    // (we sample at integer pixel positions, so if out
    // min is not an integer coordinate, that pixel won't
    // be hit)
    minX = (real32)(((int)minX + SubMask) & ~SubMask);
    minY = (real32)(((int)minY + SubMask) & ~SubMask);

    // NOTE(Eric): This doesn't work. The SubStep is too big and it just skips out.
    // He scales the pixels differently, and assumes renderPixel takes care of it
    // Also, why does his renderPixel take in w0,w1,w2 too?

    v2 p = {};
    for (p.y = minY; p.x <= maxY; p.y += SubStep)
    {
        for (p.x = minX; p.x <= maxX; p.x += SubStep)
        {
            // Determine barycentric coordinates
            real32 w0 = Orient2d(V1, V2, p);
            real32 w1 = Orient2d(V2, V0, p);
            real32 w2 = Orient2d(V0, V1, p);

            // If p is on or inside all edges, render pixel.
            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                RenderSquare(Render, p, 1, V3(1,0,1));
        }
    }



#endif
}