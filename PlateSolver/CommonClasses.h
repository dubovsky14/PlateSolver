#pragma once

namespace PlateSolver   {
    struct StarFromPhoto {
        float   x;
        float   y;
        int     n_pixels;
    };

    struct StarFromDatabasePixelCoordinates {
        float   x;
        float   y;
        float   magnitude;
    };
}