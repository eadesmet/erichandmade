

// NOTE(Eric): This is version 1 from fabian - Should not actually be used anywhere
// Over the rest of his post, this will be optimized


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
	// TODO(Eric): Not finished with reading through fabian's article!!!

	// Compute triangle bounding box
    real32 minX = Min3(V0.x, V1.x, V2.x);
    real32 minY = Min3(V0.y, V1.y, V2.y);
    real32 maxX = Max3(V0.x, V1.x, V2.x);
    real32 maxY = Max3(V0.y, V1.y, V2.y);

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
}