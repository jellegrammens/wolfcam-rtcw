#include "cg_local.h"

void CG_DrawSides_NoScale( float x, float y, float w, float h, float size ) {
    CG_AdjustFrom640( &x, &y, &w, &h );
    trap_R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, cgs.media.whiteShader 
                           );
    trap_R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, cgs.media.whiteShader );
}

void CG_DrawTopBottom_NoScale( float x, float y, float w, float h, float size ) 
{
    CG_AdjustFrom640( &x, &y, &w, &h );
    trap_R_DrawStretchPic( x, y, w, size, 0, 0, 0, 0, cgs.media.whiteShader 
                           );
    trap_R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, cgs.media.whiteShader);
}

void CG_DrawRect_FixedBorder( float x, float y, float width, float height, int border,
                              const float *color ) {
    trap_R_SetColor( color );

    CG_DrawTopBottom_NoScale( x, y, width, height, border );
    CG_DrawSides_NoScale( x, y, width, height, border );

    trap_R_SetColor( NULL );
}

