// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

#include "cg_local.h"
#include "../ui/ui_shared.h"
#include "wolfcam_local.h"

//----(SA) added to make it easier to raise/lower our statsubar by only changing one thing
#define STATUSBARHEIGHT 452
//----(SA) end

extern displayContextDef_t cgDC;
extern menuDef_t *menuScoreboard;

// NERVE - SMF
extern void Controls_GetKeyAssignment (char *command, int *twokeys);
extern char* BindingFromName(const char *cvar);
extern void Controls_GetConfig( void );

extern void CG_FinishWeaponChange(int lastweap, int newweap); // JPW NERVE
extern void CG_DrawBinocReticle(void);
extern void CG_ScreenFade( void );
extern void CG_DrawIntermission( void );
extern void CG_DrawSpectator(void);
extern void CG_DrawTeamInfo( void );
extern void CG_DrawPickupItem( void );
extern void CG_DrawTimedMenus(void);
extern void CG_DrawVote(void);
extern void CG_DrawUpperRight( void );
extern void CG_DrawCenterString( void );
extern void CG_DrawNotify( void );
extern void CG_DrawObjectiveInfo( void );
extern void CG_ActivateLimboMenu( void );
extern void CG_ShakeCamera(void);

static void Wolfcam_DrawDebugString (void)
{
	CG_DrawSmallString (20, 200, globalDebugString, 1.0F);
}

// -NERVE - SMF

////////////////////////
////////////////////////
////// new hud stuff
///////////////////////
///////////////////////

static void Wolfcam_DrawFollowing (void)
{
    const char *s;
    const char *scolor;
    int w;
    qboolean visible;

    //FIXME wolfcam could be in limbo or not visible
    //if (cg_entities[cgs.clientinfo[wcg.clientNum].clientNum].currentValid  ||
    //    cg.snap->ps.clientNum == wcg.clientNum)
    if (wclients[wcg.clientNum].currentValid)
        visible = qtrue;
    else
        visible = qfalse;

    if (visible)
        scolor = S_COLOR_WHITE;
    else
        scolor = S_COLOR_RED;

    if (cg_entities[cgs.clientinfo[wcg.clientNum].clientNum].currentState.number != cg_entities[cgs.clientinfo[wcg.clientNum].clientNum].currentState.clientNum) {
        scolor = S_COLOR_YELLOW;
    }

    s = va ("%sFollowing^7 %s", scolor, cgs.clientinfo[wcg.clientNum].name);

#if 1
    w = CG_DrawStrlen(s) * SMALLCHAR_WIDTH;
    CG_DrawSmallString (320 - w/2 + 14, 45, s, 1.0F);
    //CG_DrawFlagModel (320 - w/2 - 20, 80, 30, 30, TEAM_RED);
    switch (cgs.clientinfo[wcg.clientNum].team) {
    case TEAM_RED:
        CG_DrawPic (320 - w/2 - 14, 45 + 2, 24, 14, trap_R_RegisterShaderNoMip( "ui_mp/assets/ger_flag.tga"));
        break;
    case TEAM_BLUE:
        CG_DrawPic (320 - w/2 - 14, 45 + 2, 24, 14, trap_R_RegisterShaderNoMip( "ui_mp/assets/usa_flag.tga"));
        break;
    default:
        break;
    }
    //    CG_DrawPic (320 - w/2 - 25, 45, 40, 40, cgs.clientinfo[wcg.clientNum].team == TEAM_RED ? trap_R_RegisterShaderNoMip( "ui_mp/assets/ger_flag.tga" ) : trap_R_RegisterShaderNoMip( "ui_mp/assets/usa_flag.tga" ));
#endif

#if 0
    { 
        vec4_t color = { 1, 1,1,1 };

        //trap_R_SetColor (color);
        w = cg.centerPrintCharWidth * CG_DrawStrlen(s);
        CG_DrawStringExt (( SCREEN_WIDTH - w ) / 2, 50, s, color, qfalse, qtrue, cg.centerPrintCharWidth, (int)(cg.centerPrintCharWidth * 1.5), 0);
    }
#endif
}


/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

/*
=================
CG_DrawTeamOverlay
=================
*/

extern int maxCharsBeforeOverlay;

#define TEAM_OVERLAY_MAXNAME_WIDTH	16
#define TEAM_OVERLAY_MAXLOCATION_WIDTH	20

#if 0  //FIXME wolfcam
//hhh
static float Wolfcam_DrawTeamOverlay( float y ) {
	int x, w, h, xx;
	int i, len;
	const char *p;
	vec4_t		hcolor;
	int pwidth, lwidth;
	int plyrs;
	char st[16];
	clientInfo_t *ci;
	// NERVE - SMF
	char		classType[2] = { 0, 0 };
	int			val;
	vec4_t		deathcolor, damagecolor; // JPW NERVE
	float		*pcolor;
    team_t team;
	// -NERVE - SMF

    if (!wolfcam_following)
        team = cg.snap->ps.persistant[PERS_TEAM];
    else
        team = cgs.clientinfo[wcg.clientNum].team;

	deathcolor[0] = 1;
	deathcolor[1] = 0;
	deathcolor[2] = 0;
	deathcolor[3] = cg_hudAlpha.value;
	damagecolor[0] = 1;
	damagecolor[1] = 1;
	damagecolor[2] = 0;
	damagecolor[3] = cg_hudAlpha.value;
	maxCharsBeforeOverlay = 80;

	if ( !cg_drawTeamOverlay.integer ) {
		return y;
	}

    if (team != TEAM_RED  &&  team != TEAM_BLUE)
        return y; // Not on any team

	plyrs = 0;

	// max player name width
	pwidth = 0;
	for (i = 0; i < numSortedTeamPlayers; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == team) {
			plyrs++;
			len = CG_DrawStrlen( ci->name );
			if (len > pwidth)
				pwidth = len;
		}
	}

	if (!plyrs)
		return y;

	if (pwidth > TEAM_OVERLAY_MAXNAME_WIDTH)
		pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;

#if 1
	// max location name width
	lwidth = 0;
	if ( cg_drawTeamOverlay.integer > 1 ) {
		for (i = 0; i < numSortedTeamPlayers; i++) {
			ci = cgs.clientinfo + sortedTeamPlayers[i];
			if ( ci->infoValid && 
				ci->team == team &&
				CG_ConfigString(CS_LOCATIONS + ci->location)) {
				len = CG_DrawStrlen( CG_TranslateString( CG_ConfigString( CS_LOCATIONS + ci->location ) ) );
				if (len > lwidth)
					lwidth = len;
			}
		}
	}
#else
	// max location name width
	lwidth = 0;
	for (i = 1; i < MAX_LOCATIONS; i++) {
		p = CG_TranslateString( CG_ConfigString( CS_LOCATIONS + i ) );
		if (p && *p) {
			len = CG_DrawStrlen(p);
			if (len > lwidth)
				lwidth = len;
		}
	}
#endif

	if (lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH)
		lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;

	if ( cg_drawTeamOverlay.integer > 1 )
		w = (pwidth + lwidth + 3 + 7) * TINYCHAR_WIDTH; // JPW NERVE was +4+7
	else
		w = (pwidth + lwidth + 8) * TINYCHAR_WIDTH; // JPW NERVE was +4+7

	x = 640 - w - 4; // JPW was -32
	h = plyrs * TINYCHAR_HEIGHT;

	// DHM - Nerve :: Set the max characters that can be printed before the left edge
	maxCharsBeforeOverlay = (x / TINYCHAR_WIDTH) - 1;

	if ( team == TEAM_RED ) {
		hcolor[0] = 0.5f; // was 0.38 instead of 0.5 JPW NERVE
		hcolor[1] = 0.25f;
		hcolor[2] = 0.25f;
		hcolor[3] = 0.25f*cg_hudAlpha.value;
	} else { // if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
		hcolor[0] = 0.25f;
		hcolor[1] = 0.25f;
		hcolor[2] = 0.5f;
		hcolor[3] = 0.25f*cg_hudAlpha.value;
	}

	CG_FillRect(x,y,w,h,hcolor);
	VectorSet( hcolor, 0.4f, 0.4f, 0.4f );
	hcolor[3] = cg_hudAlpha.value;
	CG_DrawRect( x-1, y, w+2, h+2, 1, hcolor );


	for (i = 0; i < numSortedTeamPlayers; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == team) {

			// NERVE - SMF
			// determine class type
			val = cg_entities[ ci->clientNum ].currentState.teamNum;
			if ( val == 0 )
				classType[0] = 'S';
			else if ( val == 1 )
				classType[0] = 'M';
			else if ( val == 2 )
				classType[0] = 'E';
			else if ( val == 3 )
				classType[0] = 'L';
			else
				classType[0] = 'S';

			Com_sprintf (st, sizeof(st), "%s", CG_TranslateString( classType ) );

			xx = x + TINYCHAR_WIDTH;

			hcolor[0] = hcolor[1] = 1.0;
			hcolor[2] = 0.0;
			hcolor[3] = cg_hudAlpha.value;

			CG_DrawStringExt( xx, y,
				st, hcolor, qtrue, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 1 );

			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
			hcolor[3] = cg_hudAlpha.value;

			xx = x + 3*TINYCHAR_WIDTH;

			// JPW NERVE
			if (ci->health > 80)
				pcolor = hcolor;
			else if ( ci->health > 0 )
				pcolor = damagecolor;
			else
				pcolor = deathcolor;
			// jpw

			CG_DrawStringExt( xx, y,
				ci->name, pcolor, qtrue, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH);

			if (lwidth) {
				p = CG_ConfigString( CS_LOCATIONS + ci->location );
				if (!p || !*p)
					p = "unknown";
				p = CG_TranslateString( p );
				len = CG_DrawStrlen(p);
				if (len > lwidth)
					len = lwidth;

				xx = x + TINYCHAR_WIDTH * 5 + TINYCHAR_WIDTH * pwidth + 
					((lwidth/2 - len/2) * TINYCHAR_WIDTH);
				CG_DrawStringExt( xx, y,
					p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
					TEAM_OVERLAY_MAXLOCATION_WIDTH);
			}


			Com_sprintf (st, sizeof(st), "%3i", ci->health); // JPW NERVE pulled class stuff since it's at top now

			if ( cg_drawTeamOverlay.integer > 1 )
				xx = x + TINYCHAR_WIDTH * 6 + TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;
			else
				xx = x + TINYCHAR_WIDTH * 4 + TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;

			CG_DrawStringExt( xx, y,
				st, pcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 3 );

			y += TINYCHAR_HEIGHT;
		}
	}

	return y;
}
#endif

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/

/*
=================
CG_DrawTeamInfo
=================
*/
#if 0  //FIXME wolfcam-rtcw allow colors for team chats to distinguish?
void CG_DrawTeamInfo( void ) {
	int w, h;
	int i, len;
	vec4_t		hcolor;
	int		chatHeight;
	float	alphapercent;

#define CHATLOC_Y 385 // bottom end
#define CHATLOC_X 0

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT)
		chatHeight = cg_teamChatHeight.integer;
	else
		chatHeight = TEAMCHAT_HEIGHT;
	if (chatHeight <= 0)
		return; // disabled

	if (cgs.teamLastChatPos != cgs.teamChatPos) {
		if (cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer) {
			cgs.teamLastChatPos++;
		}

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * TINYCHAR_HEIGHT;

		w = 0;

		for (i = cgs.teamLastChatPos; i < cgs.teamChatPos; i++) {
			len = CG_DrawStrlen(cgs.teamChatMsgs[i % chatHeight]);
			if (len > w)
				w = len;
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

// JPW NERVE rewritten to support first pass at fading chat messages
		for (i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i--) {
			alphapercent = 1.0f - (cg.time - cgs.teamChatMsgTimes[i % chatHeight]) / (float)(cg_teamChatTime.integer);
			if (alphapercent > 1.0f)
				alphapercent = 1.0f;
			else if (alphapercent < 0.f)
				alphapercent = 0.f;

			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
				hcolor[0] = 1;
				hcolor[1] = 0;
				hcolor[2] = 0;
//			hcolor[3] = 0.33;
			} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
				hcolor[0] = 0;
				hcolor[1] = 0;
				hcolor[2] = 1;
//			hcolor[3] = 0.33;
			} else {
				hcolor[0] = 0;
				hcolor[1] = 1;
				hcolor[2] = 0;
//			hcolor[3] = 0.33;
			}

			hcolor[3] = 0.33f * alphapercent;

			trap_R_SetColor( hcolor );
			CG_DrawPic( CHATLOC_X, CHATLOC_Y - (cgs.teamChatPos - i)*TINYCHAR_HEIGHT, 640, TINYCHAR_HEIGHT, cgs.media.teamStatusBar );

			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
			hcolor[3] = alphapercent;
			trap_R_SetColor( hcolor );

			CG_DrawStringExt( CHATLOC_X + TINYCHAR_WIDTH, 
				CHATLOC_Y - (cgs.teamChatPos - i)*TINYCHAR_HEIGHT, 
				cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
//			CG_DrawSmallString( CHATLOC_X + SMALLCHAR_WIDTH, 
//				CHATLOC_Y - (cgs.teamChatPos - i)*SMALLCHAR_HEIGHT, 
//				cgs.teamChatMsgs[i % TEAMCHAT_HEIGHT], 1.0F );
		}
// jpw
	}
}
#endif

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
#if 0  //FIXME wolfcam-rtcw show disconnect for demo taker??
static void CG_DrawDisconnect( void ) {
	float		x, y;
	int			cmdNum;
	usercmd_t	cmd;
	const char		*s;
	int			w;  // bk010215 - FIXME char message[1024];

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &cmd );
	if ( cmd.serverTime <= cg.snap->ps.commandTime
		|| cmd.serverTime > cg.time ) {	// special check for map_restart // bk 0102165 - FIXME
		return;
	}

	// also add text in center of screen
	s = CG_TranslateString( "Connection Interrupted" ); // bk 010215 - FIXME
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w/2, 100, s, 1.0F);

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

	x = 640 - 72;
	y = 480 - 52;

	CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader("gfx/2d/net.tga" ) );
}
#endif



/*
================================================================================

CROSSHAIRS

================================================================================
*/

/*
==============
CG_DrawWeapReticle
==============
*/
void Wolfcam_DrawWeapReticle(void) {
	qboolean snooper, sniper, fg;
	vec4_t	color = {0, 0, 0, 1};

	// DHM - Nerve :: So that we will draw reticle
	if ( cgs.gametype >= GT_WOLF && ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback ) ) {
		sniper = (qboolean)(cg.snap->ps.weapon == WP_SNIPERRIFLE);
		snooper = (qboolean)(cg.snap->ps.weapon == WP_SNOOPERSCOPE);
		fg = (qboolean)(cg.snap->ps.weapon == WP_FG42SCOPE);
	} else {
		sniper = (qboolean)(cg.weaponSelect == WP_SNIPERRIFLE);
		snooper = (qboolean)(cg.weaponSelect == WP_SNOOPERSCOPE);
		fg = (qboolean)(cg.weaponSelect == WP_FG42SCOPE);
	}

    if (wolfcam_following) {
        int weapon = cg_entities[cgs.clientinfo[wcg.clientNum].clientNum].currentState.weapon;
        sniper = (qboolean)(weapon == WP_SNIPERRIFLE);
        snooper = (qboolean)(weapon == WP_SNOOPERSCOPE);
        fg = (qboolean)(weapon == WP_FG42SCOPE);
    }

	if(sniper) {
		if(cg_reticles.integer) {

			// sides
			CG_FillRect (0, 0, 80, 480, color);
			CG_FillRect (560, 0, 80, 480, color);

			// center
			if(cgs.media.reticleShaderSimple)
				CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

			// hairs
			CG_FillRect (84, 239, 177, 2, color);	// left
			CG_FillRect (320, 242, 1, 58, color);	// center top
			CG_FillRect (319, 300, 2, 178, color);	// center bot
			CG_FillRect (380, 239, 177, 2, color);	// right
		}
	}
	else if (snooper) {
		if(cg_reticles.integer) {

			// sides
			CG_FillRect (0, 0, 80, 480, color);
			CG_FillRect (560, 0, 80, 480, color);
			
			// center

//----(SA)	added
		// DM didn't like how bright it gets
		{
		vec4_t		hcolor = {0.7, .8, 0.7, 0};	// greenish
		float		brt;

		brt = cg_reticleBrightness.value;
		Com_Clamp(0.0f, 1.0f, brt);
		hcolor[0]*=brt;
		hcolor[1]*=brt;
		hcolor[2]*=brt;
		trap_R_SetColor( hcolor );
		}
//----(SA)	end

			if(cgs.media.snooperShaderSimple)
				CG_DrawPic( 80, 0, 480, 480, cgs.media.snooperShaderSimple );

			// hairs

			CG_FillRect (310, 120, 20, 1, color);	//					-----
			CG_FillRect (300, 160, 40, 1, color);	//				-------------
			CG_FillRect (310, 200, 20, 1, color);	//					-----

			CG_FillRect (140, 239, 360, 2, color);	// horiz ---------------------------

			CG_FillRect (310, 280, 20, 1, color);	//					-----		
			CG_FillRect (300, 320, 40, 1, color);	//				-------------		
			CG_FillRect (310, 360, 20, 1, color);	//					-----		

			
			
			CG_FillRect (400, 220, 1, 40, color);	// l

			CG_FillRect (319, 60, 2, 360, color);	// vert

			CG_FillRect (240, 220, 1, 40, color);	// r
		}
	}
}




/*
=================
CG_DrawCrosshair
=================
*/
static void Wolfcam_DrawCrosshair(void) {
	float		w, h;
	qhandle_t	hShader;
	//float		f;
	float		x, y;
	int			weapnum;		// DHM - Nerve
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

    weapnum = es->weapon;

	switch(weapnum) {

		// weapons that get no reticle
		case WP_NONE:	// no weapon, no crosshair
            if (es->eFlags & EF_ZOOMING)
				CG_DrawBinocReticle();
			if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
				return;
			break;

		// special reticle for weapon
		case WP_SNIPERRIFLE:
			if(!(es->eFlags & EF_MG42_ACTIVE)) {

				// JPW NERVE -- don't let players run with rifles -- speed 80 == crouch, 128 == walk, 256 == run
#if 0  //FIXME wolfcam-rtcw
				if (cg_gameType.integer != GT_SINGLE_PLAYER)
					if (VectorLength(cg.snap->ps.velocity) > 127.0f) {
						if (cg.snap->ps.weapon == WP_SNIPERRIFLE) {
							CG_FinishWeaponChange(WP_SNIPERRIFLE, WP_MAUSER);
						}
						if (cg.snap->ps.weapon == WP_SNOOPERSCOPE) {
							CG_FinishWeaponChange(WP_SNOOPERSCOPE, WP_GARAND);
						}
					}
				// jpw
#endif				
				Wolfcam_DrawWeapReticle();
				return;
			}
			break;
		default:
			break;
	}

	// using binoculars
	//if(cg.zoomedBinoc) {
    if (es->eFlags & EF_ZOOMING) {
		CG_DrawBinocReticle();
		return;
	}

	if ( cg_drawCrosshair.integer < 0 )	//----(SA)	moved down so it doesn't keep the scoped weaps from drawing reticles
		return;

	// set color based on health
	if (0) {  //( cg_crosshairHealth.integer ) {  //FIXME wolfcam-rtcw  colorforhealth? check
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor (cg.xhairColor);
	}

#if 0
    //FIXME wolfcam-rtcw crosshair color
 {
     vec4_t hg = { 0.0,  1.0,  0.0, 1.0 };

     //trap_R_SetColor (colorGreen);
     trap_R_SetColor (hg);
     //    CG_Printf ("green crosshair\n");
 }
#endif

	w = h = cg_crosshairSize.value;

#if 0  //FIXME wolfcam-rtcw
	// RF, crosshair size represents aim spread
	f = (float)cg.snap->ps.aimSpreadScale / 255.0;
	w *= ( 1 + f*2.0 );
	h *= ( 1 + f*2.0 );
#endif
	
	x = cg_crosshairX.integer;
	y = cg_crosshairY.integer;
	CG_AdjustFrom640( &x, &y, &w, &h );

    //FIXME wolfcam-rtcw
    //hShader = trap_R_RegisterShader( va("gfx/2d/crosshairf_OSP"));
	hShader = cgs.media.crosshairShader[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ];

	// NERVE - SMF - modified, fixes crosshair offset in shifted/scaled 3d views
	if (0) {  //(cg.limboMenu) { // JPW NERVE
		trap_R_DrawStretchPic( x /*+ cg.refdef.x*/ + 0.5 * (cg.refdef.width - w),
			y /*+ cg.refdef.y*/ + 0.5 * (cg.refdef.height - h), 
			w, h, 0, 0, 1, 1, hShader );
	}
	else {
		trap_R_DrawStretchPic( x + 0.5 * (cgs.glconfig.vidWidth - w), // JPW NERVE for scaled-down main windows
			y + 0.5 * (cgs.glconfig.vidHeight - h), 
			w, h, 0, 0, 1, 1, hShader );
	}
	// NERVE - SMF
	if ( cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] ) {
		w = h = cg_crosshairSize.value;
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
		CG_AdjustFrom640( &x, &y, &w, &h );

        if(cg_crosshairHealth.integer == 0) {
            trap_R_SetColor(cg.xhairColorAlt);
        }

		if (0) {  //(cg.limboMenu) { // JPW NERVE
			trap_R_DrawStretchPic( x + 0.5 * (cg.refdef.width - w), y + 0.5 * (cg.refdef.height - h), 
				w, h, 0, 0, 1, 1, cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] );
		}
		else {
			trap_R_DrawStretchPic( x + 0.5 * (cgs.glconfig.vidWidth - w), y + 0.5 * (cgs.glconfig.vidHeight - h), // JPW NERVE fix for small main windows (dunno why people still do this, but it's supported)
				w, h, 0, 0, 1, 1, cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] );
		}
	}
	// -NERVE - SMF
}

/*
=================
CG_ScanForCrosshairEntity
=================
*/
extern centity_t *cg_solidEntities[];
extern int cg_numSolidEntities;

static void Wolfcam_ScanForCrosshairEntity( void ) {
	trace_t		trace;
//	gentity_t	*traceEnt;
	vec3_t		start, end;
	int			content;

	// DHM - Nerve :: We want this in multiplayer
	if ( cgs.gametype == GT_SINGLE_PLAYER )
		return;	//----(SA)	don't use any scanning at the moment.

	VectorCopy( cg.refdef.vieworg, start );
	VectorMA( start, 8192, cg.refdef.viewaxis[0], end );	//----(SA)	changed from 8192

    //FIXME wolfcam-rtcw hack
    cg_solidEntities[cg_numSolidEntities] = &cg_entities[cgs.clientinfo[cg.snap->ps.clientNum].clientNum];
    cg_numSolidEntities++;
    cg_entities[cgs.clientinfo[cg.snap->ps.clientNum].clientNum].currentState.solid = 5249042;

	//CG_Trace( &trace, start, vec3_origin, vec3_origin, end, 
	//	, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_ITEM );
    CG_Trace( &trace, start, NULL, NULL, end, cgs.clientinfo[wcg.clientNum].clientNum, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_ITEM );

    cg_entities[cgs.clientinfo[cg.snap->ps.clientNum].clientNum].currentState.solid = 0;
    cg_numSolidEntities--;

	if ( trace.entityNum >= MAX_CLIENTS ) {
		return;
	}

//	traceEnt = &g_entities[trace.entityNum];


	// if the player is in fog, don't show it
	content = trap_CM_PointContents( trace.endpos, 0 );
	if ( content & CONTENTS_FOG ) {
		return;
	}

	// if the player is invisible, don't show it
	if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_INVIS ) ) {
		return;
	}

	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;
	if ( cg.crosshairClientNum != cg.identifyClientNum && cg.crosshairClientNum != ENTITYNUM_WORLD ) {
		cg.identifyClientRequest = cg.crosshairClientNum;
	}
}



/*
==============
CG_DrawDynamiteStatus
==============
*/
static void Wolfcam_DrawDynamiteStatus(void) {
	float		color[4];
	char		*name;
	int			timeleft;
	float		w;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;
    wclient_t *wc;
    int weapon;
    int grenadeTimeLeft;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;
    wc = &wclients[wcg.clientNum];

    weapon = es->weapon;
    grenadeTimeLeft = -(cg.time - wc->grenadeDynoTime) + 4000;  //FIXME wolfcam-rtcw

	if(weapon != WP_DYNAMITE && weapon != WP_DYNAMITE2)
		return;

	if(grenadeTimeLeft <=0)
		return;

	timeleft = grenadeTimeLeft;

//	color = g_color_table[ColorIndex(COLOR_RED)];
	color[0] = color[3] = 1.0f;

	// fade red as it pulses past seconds
	color[1] = color[2] = 1.0f - ((float)(timeleft%1000) * 0.001f);

	if(timeleft < 300)			// fade up the text
		color[3] = (float)timeleft/300.0f;

	trap_R_SetColor( color );

	timeleft *=5;
	timeleft -= (timeleft%5000);
	timeleft += 5000;
	timeleft /= 1000;

//	name = va("Timer: %d", timeleft);
	name = va("Timer: 30");
	w = CG_DrawStrlen( name ) * BIGCHAR_WIDTH;

	color[3] *= cg_hudAlpha.value;
	CG_DrawBigStringColor( 320 - w / 2, 170, name, color);

	trap_R_SetColor( NULL );
}


#if 1
#define CH_KNIFE_DIST		48	// from g_weapon.c
#define CH_LADDER_DIST		100
#define CH_WATER_DIST		100
#define CH_BREAKABLE_DIST	64
#define CH_DOOR_DIST		96

#define CH_DIST				100 //128		// use the largest value from above

/*
==============
CG_CheckForCursorHints
	concept in progress...
==============
*/
void Wolfcam_CheckForCursorHints(void) {
	trace_t		trace;
	vec3_t		start, end;
	centity_t	*tracent;
	vec3_t		pforward, eforward;
	float		dist;


    //FIXME wolfcam-rtcw
    return;

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if(cg.snap->ps.serverCursorHint) {	// server is dictating a cursor hint, use it.
		cg.cursorHintTime = cg.time;
		cg.cursorHintFade = 500;	// fade out time
		cg.cursorHintIcon = cg.snap->ps.serverCursorHint;
		cg.cursorHintValue = cg.snap->ps.serverCursorHintVal;
		return;
	}


	// From here on it's client-side cursor hints.  So if the server isn't sending that info (as an option)
	// then it falls into here and you can get basic cursorhint info if you want, but not the detailed info
	// the server sends.

	// the trace
	VectorCopy( cg.refdef.vieworg, start );
	VectorMA( start, CH_DIST, cg.refdef.viewaxis[0], end );

//	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, cg.snap->ps.clientNum, MASK_ALL &~CONTENTS_MONSTERCLIP);
	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, cg.snap->ps.clientNum, MASK_PLAYERSOLID);

	if(trace.fraction == 1)
		return;

	dist = trace.fraction * CH_DIST;

	tracent = &cg_entities[ trace.entityNum ];

	//
	// world
	//
	if(trace.entityNum == ENTITYNUM_WORLD) {
		// if looking into water, warn if you don't have a breather
		if ((trace.contents & CONTENTS_WATER) && !(cg.snap->ps.powerups[PW_BREATHER])) {
			if(dist <= CH_WATER_DIST) {
				cg.cursorHintIcon = HINT_WATER;
				cg.cursorHintTime = cg.time;
				cg.cursorHintFade = 500;
			}
		}
		
		// ladder
		else if( (trace.surfaceFlags & SURF_LADDER) && !(cg.snap->ps.pm_flags & PMF_LADDER) ) {
			if(dist <= CH_LADDER_DIST) {
				cg.cursorHintIcon = HINT_LADDER;
				cg.cursorHintTime = cg.time;
				cg.cursorHintFade = 500;
			}
		}


	}

	//
	// people
	//
	else if(trace.entityNum < MAX_CLIENTS ) {

		// knife
		if(trace.entityNum < MAX_CLIENTS && (cg.snap->ps.weapon == WP_KNIFE || cg.snap->ps.weapon == WP_KNIFE2) ) {
			if(dist <= CH_KNIFE_DIST) {

				AngleVectors (cg.snap->ps.viewangles,	pforward, NULL, NULL);
				AngleVectors (tracent->lerpAngles,		eforward, NULL, NULL);

				if(DotProduct( eforward, pforward ) > 0.9f)	{	// from behind
					cg.cursorHintIcon = HINT_KNIFE;
					cg.cursorHintTime = cg.time;
					cg.cursorHintFade = 100;
				}
			}
		}
	}

	//
	// other entities
	//
	else {
		if(tracent->currentState.dmgFlags) {
			switch(tracent->currentState.dmgFlags) {
				case HINT_DOOR:
					if(dist<CH_DOOR_DIST) {
						cg.cursorHintIcon = HINT_DOOR;
						cg.cursorHintTime = cg.time;
						cg.cursorHintFade = 500;
					}
					break;
				//case HINT_CHAIR:
				case HINT_MG42:
					if(dist<CH_DOOR_DIST && !(cg.snap->ps.eFlags & EF_MG42_ACTIVE)) {
						cg.cursorHintIcon = HINT_ACTIVATE;
						cg.cursorHintTime = cg.time;
						cg.cursorHintFade = 500;
					}
					break;
			}
		} else {

			if(tracent->currentState.eType == ET_EXPLOSIVE) {
				if ( (dist < CH_BREAKABLE_DIST) && (cg.cursorHintIcon != HINT_FORCENONE) ) { // JPW NERVE so we don't get breakables in trigger_objective_info fields for wrong team (see code chunk in g_main.c)
					cg.cursorHintIcon = HINT_BREAKABLE;
					cg.cursorHintTime = cg.time;
					cg.cursorHintFade = 500;
				}

			}
		}
	}
}
#endif

/*
=====================
CG_DrawCrosshairNames
=====================
*/
static void Wolfcam_DrawCrosshairNames( void ) {
	float		*color;
	char		*name;
	float		w;
	// NERVE - SMF
	const char	*s, *playerClass;
	int			playerHealth, val;
	vec4_t		c;
	float		barFrac;
    team_t team;
	// -NERVE - SMF

	if ( cg_drawCrosshair.integer < 0 ) {
		return;
	}
	if ( !cg_drawCrosshairNames.integer ) {
		return;
	}

	// NERVE - SMF - we don't want to do this in warmup
	if ( cgs.gamestate != GS_PLAYING && cgs.gametype == GT_WOLF_STOPWATCH ) {
		return;
	}

	// Ridah
	if ( cg_gameType.integer == GT_SINGLE_PLAYER ) {
		return;
	}
	// done.

    team = cgs.clientinfo[wcg.clientNum].team;

	// scan the known entities to see if the crosshair is sighted on one
	Wolfcam_ScanForCrosshairEntity();

	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );

	if ( !color ) {
		trap_R_SetColor( NULL );
		return;
	}

	// NERVE - SMF
	if ( cg.crosshairClientNum > MAX_CLIENTS )
		return;

	// we only want to see players on our team
	if ( team != TEAM_SPECTATOR
		&& cgs.clientinfo[ cg.crosshairClientNum ].team != team )
		return;

	// determine player class
	val = cg_entities[ cg.crosshairClientNum ].currentState.teamNum;
	if ( val == 0 )
		playerClass = "S";
	else if ( val == 1 )
		playerClass = "M";
	else if ( val == 2 )
		playerClass = "E";
	else if ( val == 3 )
		playerClass = "L";
	else
		playerClass = "";

	name = cgs.clientinfo[ cg.crosshairClientNum ].name;

	s = va( "[%s] %s", CG_TranslateString( playerClass ), name );
	if ( !s )
		return;
	w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

	// draw the name and class
	CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );

	// draw the health bar
	playerHealth = 0;  //FIXME wolfcam-rtcw cg.identifyClientHealth;

	if ( cg.crosshairClientNum == cg.identifyClientNum ) {
		barFrac = (float)playerHealth / 100;

		if ( barFrac > 1.0 )
			barFrac = 1.0;
		else if ( barFrac < 0 )
			barFrac = 0;

		c[0] = 1.0f;
		c[1] = c[2] = barFrac;
		c[3] = 0.25 + barFrac * 0.5 * color[3];

		CG_FilledBar( 320 - w / 2, 190, 110, 10, c, NULL, NULL, barFrac, 16 );
	}
	// -NERVE - SMF

	trap_R_SetColor( NULL );
}



//==============================================================================

/*
=================
CG_DrawVote
=================
*/
#if 0  //FIXME wolfcam
void CG_DrawVote(void) {
	char	*s;
	char str1[32], str2[32];
	float color[4] = { 1, 1, 0, 1 };
	int		sec;

	if ( cgs.complaintEndTime > cg.time ) {

		if ( cgs.complaintClient == -1 ) {
			s = "Your complaint has been filed";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if ( cgs.complaintClient == -2 ) {
			s = "Complaint dismissed";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if ( cgs.complaintClient == -3 ) {
			s = "Server Host cannot be complained against";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}
		if ( cgs.complaintClient == -4 ) {
			s = "You were team-killed by the Server Host";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
			return;
		}

		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		if ( !Q_stricmp( str1, "???" ) ) {
			Q_strncpyz( str1, "vote yes", 32 );
		}
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );
		if ( !Q_stricmp( str2, "???" ) ) {
			Q_strncpyz( str2, "vote no", 32 );
		}

		s = va( CG_TranslateString( "File complaint against %s for team-killing?" ), cgs.clientinfo[cgs.complaintClient].name);
		CG_DrawStringExt( 8, 200, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 80 );
		return;
	}

	if ( !cgs.voteTime ) {
		return;
	}

	Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
	if ( !Q_stricmp( str1, "???" ) ) {
		Q_strncpyz( str1, "vote yes", 32 );
	}
	Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );
	if ( !Q_stricmp( str2, "???" ) ) {
		Q_strncpyz( str2, "vote no", 32 );
	}

	// play a talk beep whenever it is modified
	if ( cgs.voteModified ) {
		cgs.voteModified = qfalse;
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
	}

	sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}

	if ( !(cg.snap->ps.eFlags & EF_VOTED) ) {
		s = va( CG_TranslateString( "VOTE(%i):%s" ), sec, cgs.voteString);
		CG_DrawStringExt( 8, 200, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 60 );

		s = va( CG_TranslateString( "YES(%s):%i, NO(%s):%i" ), str1, cgs.voteYes, str2, cgs.voteNo );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 60 );
	}
	else {
		s = va( CG_TranslateString( "Y:%i, N:%i" ), cgs.voteYes, cgs.voteNo );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 20 );
	}
}
#endif



/*
=================
CG_DrawWarmup
=================
*/
void Wolfcam_DrawWarmup( void ) {
	int			w;
	int			sec;
	int			cw;
	const char	*s, *s1, *s2;
    team_t team;

	if (cgs.gametype == GT_SINGLE_PLAYER) {
		return;		// (SA) don't bother with this stuff in sp
	}

    team = cgs.clientinfo[wcg.clientNum].team;

	sec = cg.warmup;
	if ( !sec ) {
		if ( cgs.gamestate == GS_WAITING_FOR_PLAYERS ) {
			cw = 10;

			s = CG_TranslateString( "Game Stopped - Waiting for more players" );

			w = CG_DrawStrlen( s );
			CG_DrawStringExt( 320 - w * 6, 120, s, colorWhite, qfalse, qtrue, 12, 18, 0 );


			s1 = va( CG_TranslateString( "Waiting for %i players" ), cgs.minclients );
			s2 = CG_TranslateString( "or call a vote to start match" );

			w = CG_DrawStrlen( s1 );
			CG_DrawStringExt( 320 - w * cw/2, 160, s1, colorWhite, 
				qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

			w = CG_DrawStrlen( s2 );
			CG_DrawStringExt( 320 - w * cw/2, 180, s2, colorWhite, 
				qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

			return;
		}

		return;
	}

	sec = ( sec - cg.time ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}

	if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
		s = va( "%s %i", CG_TranslateString( "(WARMUP) Match begins in:" ), sec + 1 );
	}
	else {
		s = va( "%s %i", CG_TranslateString( "(WARMUP) Match begins in:" ), sec + 1 );
	}

	w = CG_DrawStrlen( s );
	CG_DrawStringExt( 320 - w * 6, 120, s, colorWhite, qfalse, qtrue, 12, 18, 0 );

	// NERVE - SMF - stopwatch stuff
	s1 = "";
	s2 = "";

	if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
		const char	*cs;
		int		defender;

		s = va( "%s %i", CG_TranslateString( "Stopwatch Round" ), cgs.currentRound + 1 );

		cs = CG_ConfigString( CS_MULTI_INFO );
		defender = atoi( Info_ValueForKey( cs, "defender" ) );

		if ( !defender ) {
			if ( team == TEAM_RED ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Axis team";
					s2 = "Keep the Allies from beating the clock!";
				}
				else {
					s1 = "You are on the Axis team";
				}
			}
			else if ( team == TEAM_BLUE ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Allied team";
					s2 = "Try to beat the clock!";
				}
				else {
					s1 = "You are on the Allied team";
				}
			}
		}
		else {
			if ( team == TEAM_RED ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Axis team";
					s2 = "Try to beat the clock!";
				}
				else {
					s1 = "You are on the Axis team";
				}
			}
			else if ( team == TEAM_BLUE ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Allied team";
					s2 = "Keep the Axis from beating the clock!";
				}
				else {
					s1 = "You are on the Allied team";
				}
			}
		}

		if ( strlen( s1 ) )
			s1 = CG_TranslateString( s1 );

		if ( strlen( s2 ) )
			s2 = CG_TranslateString( s2 );

		cw = 10;

		w = CG_DrawStrlen( s );
		CG_DrawStringExt( 320 - w * cw/2, 140, s, colorWhite, 
			qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

		w = CG_DrawStrlen( s1 );
		CG_DrawStringExt( 320 - w * cw/2, 160, s1, colorWhite, 
			qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

		w = CG_DrawStrlen( s2 );
		CG_DrawStringExt( 320 - w * cw/2, 180, s2, colorWhite, 
			qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
	}
}

//==================================================================================

/*
=================
CG_DrawFlashFade
=================
*/
static void Wolfcam_DrawFlashFade( void ) {
	static int lastTime;
	int elapsed, time;
	vec4_t col;

	if (cgs.fadeStartTime + cgs.fadeDuration < cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	} else if (cgs.fadeAlphaCurrent != cgs.fadeAlpha) {
		elapsed = (time = trap_Milliseconds()) - lastTime;	// we need to use trap_Milliseconds() here since the cg.time gets modified upon reloading
		lastTime = time;
		if (elapsed < 500 && elapsed > 0) {
			if (cgs.fadeAlphaCurrent > cgs.fadeAlpha) {
				cgs.fadeAlphaCurrent -= ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent < cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			} else {
				cgs.fadeAlphaCurrent += ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent > cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			}
		}
	}
	// now draw the fade
	if (cgs.fadeAlphaCurrent > 0.0) {
		VectorClear( col );
		col[3] = cgs.fadeAlphaCurrent;
		CG_FillRect( -10, -10, 650, 490, col );
	}
}



/*
==============
CG_DrawFlashZoomTransition
	hide the snap transition from regular view to/from zoomed

  FIXME: TODO: use cg_fade?
==============
*/
static void Wolfcam_DrawFlashZoomTransition(void) {
	vec4_t	color;
	float	frac;
	int		fadeTime;

	if (!cg.snap)
		return;

    return;  //FIXME wolfcam-rtcw

	if(cg.snap->ps.eFlags & EF_MG42_ACTIVE) {	// don't draw when on mg_42
		// keep the timer fresh so when you remove yourself from the mg42, it'll fade
		cg.zoomTime = cg.time;
		return;
	}

	if (cgs.gametype != GT_SINGLE_PLAYER) { // JPW NERVE
		fadeTime = 400;
	}
	else {
		if(cg.zoomedScope)
			fadeTime = cg.zoomedScope;	//----(SA)	
		else
			fadeTime = 300;
	}
	// jpw

	frac = cg.time - cg.zoomTime;

	if(frac < fadeTime) {
		frac = frac/(float)fadeTime;

		if(cg.weaponSelect == WP_SNOOPERSCOPE)
//			Vector4Set( color, 0.7f, 0.3f, 0.7f, 1.0f - frac );
//			Vector4Set( color, 1, 0.5, 1, 1.0f - frac );
//			Vector4Set( color, 0.5f, 0.3f, 0.5f, 1.0f - frac );
			Vector4Set( color, 0.7f, 0.6f, 0.7f, 1.0f - frac );
		else
			Vector4Set( color, 0, 0, 0, 1.0f - frac );

		CG_FillRect( -10, -10, 650, 490, color );
	}
}


extern void CG_DamageBlendBlob( void );

void Wolfcam_DamageBlendBlob (void)
{
    int                     t,i;
    int                     maxTime;
    refEntity_t             ent;
    qboolean        pointDamage;
    viewDamage_t *vd;
    float           redFlash;

    float damageX, damageY, damageValue;
    int damageTime;

    float left, front, up;
    //    float kick;
    vec3_t dir;
    vec3_t angles;
    float dist;
    float yaw, pitch;

    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;
    wclient_t *wc;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;
    wc = &wclients[wcg.clientNum];

    //return;  //FIXME wolfcam can't get this to work :(
    //CG_DamageBlendBlob ();


    // ragePro systems can't fade blends, so don't obscure the screen
    if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
        return;
    }


    //FIXME wolfcam-rtcw this isn't working.  RT_SPRITE a problem in third person??
    //return;

    //if (cg.time - wolfcam_bulletDamageTime > 100)
    //    return;

    for (i = 0;  i < 20;  i++) {

    memset (&ent, 0, sizeof(ent));
    ent.reType = RT_SPRITE;
    //ent.renderfx = RF_FIRST_PERSON;

    //ent.renderfx = RF_FIRST_PERSON | RF_DEPTHHACK | RF_HILIGHT;
    //ent.renderfx = RF_THIRD_PERSON | RF_DEPTHHACK | RF_HILIGHT;

    //return ;
    //FIXME wolfcam-rtcw just putting in numbers
    damageTime =  wc->bulletDamageTime;  // + 444;
    t = cg.time - wc->bulletDamageTime;

    maxTime = 444;  // vd->damageDuration;
    //t = cg.time - wolfcam_bulletDamageTime;
    //damageTime = cg.time;
    //t = 200;
    //if (t <= 0  ||  t >= maxTime)
    //    return;

    //CG_Printf ("not getting here???\n");

#if 0
    damageX = -0.12;
    damageY = 0.24798;
    damageValue = 10;
#endif

    damageValue = 10;
    if (wclients[wcg.clientNum].bulletDamagePitch == 255  &&  wclients[wcg.clientNum].bulletDamageYaw == 255) {
        damageX = 0;
        damageY = 0;
    } else {
        pitch = wclients[wcg.clientNum].bulletDamagePitch / 255.0 * 360;
        yaw = wclients[wcg.clientNum].bulletDamageYaw / 255.0 * 360;

        angles[PITCH] = pitch;
        angles[YAW] = yaw;
        angles[ROLL] = 0;

        AngleVectors( angles, dir, NULL, NULL );
        VectorSubtract( vec3_origin, dir, dir );

        front = DotProduct (dir, cg.refdef.viewaxis[0] );
        left = DotProduct (dir, cg.refdef.viewaxis[1] );
        up = DotProduct (dir, cg.refdef.viewaxis[2] );

        dir[0] = front;
        dir[1] = left;
        dir[2] = 0;
        dist = VectorLength( dir );
        if ( dist < 0.1 ) {
            dist = 0.1;
        }

        //cg.v_dmg_roll = kick * left;
                
        //cg.v_dmg_pitch = -kick * front;

        if ( front <= 0.1 ) {
            front = 0.1;
        }
        //damageX = crandom()*0.3 + -left / front;
        //damageY = crandom()*0.3 + up / dist;
        damageX = 0.08 + -left / front;
        damageY = 0.08 + up / dist;
    }

    maxTime = 500 * (1 + 2*(!damageX && !damageY));
    if (t <= 0  ||  t >= maxTime)                                                                     
        return;  
    //CG_Printf ("damageX: %f  damageY: %f  maxTime: %d\n", damageX, damageY, maxTime);
    //return;
#if 1
    VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], ent.origin );
    VectorMA( ent.origin, damageX * -8, cg.refdef.viewaxis[1], ent.origin );
    VectorMA( ent.origin, damageY * 8, cg.refdef.viewaxis[2], ent.origin );
#endif

 
     
    //VectorCopy (cent->lerpOrigin, ent.origin);

    ent.radius = damageValue * 0.4 * (0.5 + 0.5*(float)t/maxTime
                                      ) * (0.75 + 0.5 * fabs(sin(damageTime)));
    
    //CG_Printf ("ent.radius:%f\n", ent.radius);

    ent.customShader = cgs.media.viewBloodAni[(int)(floor(((float)t / maxTime)*4.9))];//cgs.media.viewBloodShader;
    //ent.customShader = cgs.media.waterBubbleShader;

    ent.shaderRGBA[0] = 255;
    ent.shaderRGBA[1] = 255;
    ent.shaderRGBA[2] = 255;
    ent.shaderRGBA[3] = 255;
    
#if 0

    {
        refEntity_t e;
        memset (&e, 0, sizeof(e));
        VectorCopy (cg.predictedPlayerState.origin, e.origin);
        e.origin[2] += 200;
        e.reType = RT_SPRITE;
        //e.customShader = cgs.media.waterBubbleShader;
        e.customShader = cgs.media.connectionShader;
        e.radius = 6.66;
        e.shaderRGBA[0] = 255;
        e.shaderRGBA[1] = 255;
        e.shaderRGBA[2] = 255;
        e.shaderRGBA[3] = 255;
        trap_R_AddRefEntityToScene( &e );

    }
#endif
    //CG_Printf ("adding to scene\n");
    //CG_Printf ("Wolfcam_DamageBlendBlob: duration:%d  cg.time:%d  time:%d  damageX:%f  damageY:%f  value:%f\n", maxTime, cg.time, damageTime, damageX, damageY, damageValue);
    trap_R_AddRefEntityToScene( &ent );
    }

    return;


    redFlash = 0;

    for (i=0; i<MAX_VIEWDAMAGE; i++) {

        vd = &cg.viewDamage[i];

        if ( !vd->damageValue ) {
            continue;
        }

        maxTime = vd->damageDuration;
        t = cg.time - vd->damageTime;
        if ( t <= 0 || t >= maxTime ) {
            vd->damageValue = 0;
            continue;
        }

        pointDamage = !(!vd->damageX && !vd->damageY);

        // if not point Damage, only do flash blend
        if (!pointDamage) {
            redFlash += 10.0 * (1.0 - (float)t/maxTime);
            continue;
        }

        memset( &ent, 0, sizeof( ent ) );
        ent.reType = RT_SPRITE;
        ent.renderfx = RF_FIRST_PERSON;

        VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], ent.origin );
        VectorMA( ent.origin, vd->damageX * -8, cg.refdef.viewaxis[1], ent.origin );
        VectorMA( ent.origin, vd->damageY * 8, cg.refdef.viewaxis[2], ent.origin );

        ent.radius = vd->damageValue * 0.4 * (0.5 + 0.5*(float)t/maxTime
                                              ) * (0.75 + 0.5 * fabs(sin(vd->damageTime)));

        ent.customShader = cgs.media.viewBloodAni[(int)(floor(((float)t 
                                                               / maxTime)*4.9))];//cgs.media.viewBloodShader;
        ent.shaderRGBA[0] = 255;
        ent.shaderRGBA[1] = 255;
        ent.shaderRGBA[2] = 255;
        ent.shaderRGBA[3] = 255;
        trap_R_AddRefEntityToScene( &ent );

        redFlash += ent.radius;
    }
}

/*
=================
CG_DrawFlashDamage
=================
*/
static void Wolfcam_DrawFlashDamage( void ) {
	vec4_t		col;
	float		redFlash;
    wclient_t *wc;

    wc = &wclients[wcg.clientNum];

    //CG_Printf ("Wolfcam_DrawFlashDamage\n");
	if (!cg.snap)
		return;

    //Wolfcam_DamageBlendBlob ();
    //CG_DamageBlendBlob ();
    //return ;

    //FIXME wolfcam-rtcw, should be painTime not just bulletDamate
    if (cg.time - wc->bulletDamageTime < DAMAGE_TIME) {
        redFlash = fabs(-5 * ((cg.time - wc->bulletDamageTime) / DAMAGE_TIME));

        //        CG_Printf ("flashing\n");

        if (redFlash > 5)
            redFlash = 5;

        redFlash = 2.5;  //FIXME wolfcam-rtcw

        VectorSet( col, 0.2, 0, 0 );
        col[3] =  0.7 * (redFlash/5.0);

        CG_FillRect( -10, -10, 650, 490, col );
    }

    if (cg.time - wc->ev_pain_time < DAMAGE_TIME) {
        redFlash = fabs(-5 * ((cg.time - wc->ev_pain_time) / DAMAGE_TIME));

        //        CG_Printf ("flashing\n");                                                             

        if (redFlash > 5)
            redFlash = 5;

        redFlash = 2.5;  //FIXME wolfcam-rtcw                                                           

        VectorSet( col, 0.2, 0, 0 );
        col[3] =  0.7 * (redFlash/5.0);

        CG_FillRect( -10, -10, 650, 490, col );
    }

#if 0
	if (cg.v_dmg_time > cg.time) {
		redFlash = fabs(cg.v_dmg_pitch * ((cg.v_dmg_time - cg.time) / DAMAGE_TIME));

		// blend the entire screen red
		if (redFlash > 5)
			redFlash = 5;

		VectorSet( col, 0.2, 0, 0 );
		col[3] =  0.7 * (redFlash/5.0);
		
		CG_FillRect( -10, -10, 650, 490, col );
	}
#endif
}


/*
=================
CG_DrawFlashFire
=================
*/
static void Wolfcam_DrawFlashFire( void ) {
	vec4_t		col={1,1,1,1};
	float		alpha, max, f;

	if (!cg.snap)
		return;

    //FIXME wolfcam-rtcw
    return;

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if (!cg.snap->ps.onFireStart) {
		cg.v_noFireTime = cg.time;
		return;
	}

	alpha = (float)((FIRE_FLASH_TIME-1000) - (cg.time - cg.snap->ps.onFireStart))/(FIRE_FLASH_TIME-1000);
	if (alpha > 0) {
		if (alpha >= 1.0) {
			alpha = 1.0;
		}

		// fade in?
		f = (float)(cg.time - cg.v_noFireTime)/FIRE_FLASH_FADEIN_TIME;
		if (f >= 0.0 && f < 1.0)
			alpha = f;

		max = 0.5 + 0.5*sin((float)((cg.time/10)%1000)/1000.0);
		if (alpha > max)
			alpha = max;
		col[0] = alpha;
		col[1] = alpha;
		col[2] = alpha;
		col[3] = alpha;
		trap_R_SetColor( col );
		CG_DrawPic( -10, -10, 650, 490, cgs.media.viewFlashFire[(cg.time/50)%16] );
		trap_R_SetColor( NULL );

		trap_S_AddLoopingSound( cg.snap->ps.clientNum, cg.snap->ps.origin, vec3_origin, cgs.media.flameSound, (int)(255.0*alpha) );
		trap_S_AddLoopingSound( cg.snap->ps.clientNum, cg.snap->ps.origin, vec3_origin, cgs.media.flameCrackSound, (int)(255.0*alpha) );
	} else {
		cg.v_noFireTime = cg.time;
	}
}

/*
=================
CG_DrawFlashLightning
=================
*/
static void Wolfcam_DrawFlashLightning( void ) {
	float		alpha;
	centity_t *cent;
	qhandle_t shader;

    //FIXME wolfcam-rtcw
    return;

	if (!cg.snap)
		return;

	if (cg_thirdPerson.integer)
		return;

	cent = &cg_entities[cg.snap->ps.clientNum];

	if (!cent->pe.teslaDamagedTime || (cent->pe.teslaDamagedTime > cg.time)) {
		return;
	}

	alpha = 1.0 - (float)(cg.time - cent->pe.teslaDamagedTime)/LIGHTNING_FLASH_TIME;
	if (alpha > 0) {
		if (alpha >= 1.0) {
			alpha = 1.0;
		}

		if ((cg.time/50)%(2+(cg.time%2)) == 0)
			shader = cgs.media.viewTeslaAltDamageEffectShader;
		else
			shader = cgs.media.viewTeslaDamageEffectShader;

		CG_DrawPic( -10, -10, 650, 490, shader );
	}
}



/*
==============
CG_DrawFlashBlendBehindHUD
	screen flash stuff drawn first (on top of world, behind HUD)
==============
*/
static void Wolfcam_DrawFlashBlendBehindHUD(void) {
	Wolfcam_DrawFlashZoomTransition();
}


/*
=================
CG_DrawFlashBlend
	screen flash stuff drawn last (on top of everything)
=================
*/
static void Wolfcam_DrawFlashBlend( void ) {
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

    if (es->eFlags & EF_DEAD)
        return;

	Wolfcam_DrawFlashLightning();
	Wolfcam_DrawFlashFire();
	Wolfcam_DrawFlashDamage();
	Wolfcam_DrawFlashFade();
}


void Wolfcam_DrawObjectiveIcons (void) 
{
	float x, y, w, h, xx, fade;// JPW NERVE
	float startColor[4];
	const char *s, *buf;
	char teamstr[32];
	float captureHoldFract;
	int i, num, status,barheight;
	vec4_t hcolor = { 0.2f, 0.2f, 0.2f, 1.f };
	int msec, mins, seconds, tens; // JPW NERVE
    team_t team;
    int blueReinfTime, redReinfTime;

    team = cgs.clientinfo[wcg.clientNum].team;

// JPW NERVE added round timer
	y = 48;
	x = 5;
	msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;
	if ( msec < 0 ) {
		fade = fabs(sin(cg.time * 0.002)) * cg_hudAlpha.value;
		s = va( "0:00" );
	}
	else {
        blueReinfTime = cg_bluelimbotime.integer - (cg.time % cg_bluelimbotime.integer) + 1000;
        blueReinfTime /= 1000;
        redReinfTime = cg_redlimbotime.integer - (cg.time % cg_redlimbotime.integer) + 1000;
        redReinfTime /= 1000;

		s = va( "%i:%i%i  ^4%d  ^1%d", mins, tens, seconds, blueReinfTime, redReinfTime ); // float cast to line up with reinforce time
		fade = cg_hudAlpha.value;
	}

	CG_DrawSmallString(x,y,s,fade);

// jpw	

	x = 5;
	y = 68;
	w = 24;
	h = 14;

	// draw the stopwatch
	if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
		if ( cgs.currentRound == 0 ) {
			CG_DrawPic( 3, y, 30, 30, trap_R_RegisterShader( "sprites/stopwatch1.tga" ) );
		}
		else {
			CG_DrawPic( 3, y, 30, 30, trap_R_RegisterShader( "sprites/stopwatch2.tga" ) );
		}
		y += 34;
	}

	// determine character's team
	if ( team == TEAM_RED )
		strcpy( teamstr, "axis_desc" );
	else
		strcpy( teamstr, "allied_desc" );

	s = CG_ConfigString( CS_MULTI_INFO );
	buf = Info_ValueForKey( s, "numobjectives" );

	if ( buf && atoi( buf ) ) {
		num = atoi( buf );

		VectorSet( hcolor, 0.3f, 0.3f, 0.3f );
		hcolor[3] = 0.7f*cg_hudAlpha.value; // JPW NERVE
		CG_DrawRect( x-1, y-1, w+2, ( h + 4 ) * num - 4 + 2, 1, hcolor );

		VectorSet( hcolor, 1.0f, 1.0f, 1.0f );
		hcolor[3] = 0.2f * cg_hudAlpha.value; // JPW NERVE
		trap_R_SetColor( hcolor );
		CG_DrawPic( x, y, w , ( h + 4 ) * num - 4, cgs.media.teamStatusBar );
		trap_R_SetColor( NULL );

		for ( i = 0; i < num; i ++ ) {
			s = CG_ConfigString( CS_MULTI_OBJECTIVE1 + i );
			buf = Info_ValueForKey( s, teamstr );

			xx = x;

			hcolor[0] = 0.25f;
			hcolor[1] = 0.38f;
			hcolor[2] = 0.38f;
			hcolor[3] = 0.5f*cg_hudAlpha.value; // JPW NERVE
			trap_R_SetColor( hcolor );
			CG_DrawPic( xx, y, w, h, cgs.media.teamStatusBar );
			trap_R_SetColor( NULL );

			// draw text
			VectorSet( hcolor, 1.f, 1.f, 1.f );
			hcolor[3] = cg_hudAlpha.value; // JPW NERVE
//			CG_DrawStringExt( xx, y, va( "%i", i+1 ), hcolor, qtrue, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 ); // JPW NERVE pulled per id request

//			xx += 10;

			// draw status flags
			s = CG_ConfigString( CS_MULTI_OBJ1_STATUS + i );
			status = atoi( Info_ValueForKey( s, "status" ) );

			VectorSet( hcolor, 1, 1, 1 );
			hcolor[3] = 0.7f * cg_hudAlpha.value; // JPW NERVE
			trap_R_SetColor( hcolor );

			if ( status == 0 ) {
				CG_DrawPic( xx, y, w, h, trap_R_RegisterShaderNoMip( "ui_mp/assets/ger_flag.tga" ) );
			}
			else if ( status == 1 ) {
				CG_DrawPic( xx, y, w, h, trap_R_RegisterShaderNoMip( "ui_mp/assets/usa_flag.tga" ) );
			}

			VectorSet( hcolor, 0.2f, 0.2f, 0.2f );
			hcolor[3] = cg_hudAlpha.value; // JPW NERVE
			trap_R_SetColor( hcolor );

//			CG_DrawRect( xx, y, w, h, 2, hcolor );

			y += h + 4;
		}
	}

// JPW NERVE compute & draw hold bar
	if (0) {  //(cgs.gametype == GT_WOLF_CPH) {  //FIXME wolfcam-rtcw
		if (cg.snap->ps.stats[STAT_CAPTUREHOLD_RED] || cg.snap->ps.stats[STAT_CAPTUREHOLD_BLUE]) 
			captureHoldFract = (float)cg.snap->ps.stats[STAT_CAPTUREHOLD_RED] / 
				(float)(cg.snap->ps.stats[STAT_CAPTUREHOLD_RED] + cg.snap->ps.stats[STAT_CAPTUREHOLD_BLUE]);
		else
			captureHoldFract = 0.5;

		barheight = y - 72; // (68+4)


		startColor[0] = 1;
		startColor[1] = 1;
		startColor[2] = 1;
		startColor[3] = 1;
		if (captureHoldFract > 0.5)
			startColor[3] = cg_hudAlpha.value;
		else
			startColor[3] = cg_hudAlpha.value*((captureHoldFract)+0.15);
//		startColor[3] = 0.25;
		trap_R_SetColor(startColor);
		CG_DrawPic(32,68,5,barheight*captureHoldFract,cgs.media.redColorBar);

		if (captureHoldFract < 0.5)
			startColor[3] = cg_hudAlpha.value;
		else
			startColor[3] = cg_hudAlpha.value*(0.15+(1.0f-captureHoldFract));
//		startColor[3] = 0.25;
		trap_R_SetColor(startColor);
		CG_DrawPic(32,68+barheight*captureHoldFract,5,barheight-barheight*captureHoldFract,cgs.media.blueColorBar);
	}
// jpw


	// draw treasure icon if we have the flag
	y += 4;

	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = cg_hudAlpha.value;
	trap_R_SetColor( hcolor );
	if ( cgs.clientinfo[wcg.clientNum].powerups & (1 << PW_REDFLAG) ||
		 cgs.clientinfo[wcg.clientNum].powerups & (1 << PW_BLUEFLAG)) {
		CG_DrawPic( -7, y, 48, 48, trap_R_RegisterShader( "models/multiplayer/treasure/treasure" ) );
		y += 50;
	}
}
// -NERVE - SMF

//==================================================================================



// JPW NERVE
void Wolfcam_Draw2D2(void) {
	qhandle_t weapon;
	vec4_t hcolor;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

	VectorSet( hcolor, 1.f,1.f,1.f );
//	VectorSet(hcolor, cg_hudAlpha.value, cg_hudAlpha.value, cg_hudAlpha.value);
	hcolor[3] = cg_hudAlpha.value; 
	trap_R_SetColor( hcolor );

	CG_DrawPic( 0,480, 640, -70, cgs.media.hud1Shader );

	if(!(es->eFlags & EF_MG42_ACTIVE)) {
		switch (es->weapon) {
		case WP_COLT:
		case WP_LUGER:
			weapon = cgs.media.hud2Shader;
			break;
		case WP_KNIFE:
			weapon = cgs.media.hud5Shader;
			break;
		case WP_VENOM:
			weapon = cgs.media.hud4Shader;
			break;
		default:
			weapon = cgs.media.hud3Shader;
		}
		CG_DrawPic( 220,410, 200,-200,weapon);
	}
  }
// jpw

/*
=================
CG_DrawCompassIcon

NERVE - SMF
=================
*/
void Wolfcam_DrawCompassIcon( int x, int y, int w, int h, vec3_t origin, vec3_t dest, qhandle_t shader ) {
	float angle, pi2 = M_PI * 2;
	vec3_t v1, angles;
	float len;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

	VectorCopy( dest, v1 );
	VectorSubtract( origin, v1, v1 );
	len = VectorLength( v1 );
	VectorNormalize( v1 );
	vectoangles( v1, angles );

	if ( v1[0] == 0 && v1[1] == 0 && v1[2] == 0 )
		return;

    //	angles[YAW] = AngleSubtract( cg.snap->ps.viewangles[YAW], angles[YAW] );
	angles[YAW] = AngleSubtract( cg.refdefViewAngles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;

	w /= 2;
	h /= 2;

	x += w;
	y += h;

	w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	x = x + ( cos( angle ) * w );
	y = y + ( sin( angle ) * w );

	len = 1 - min( 1.f, len / 2000.f );

	CG_DrawPic( x - (14 * len + 4)/2, y - (14 * len + 4)/2, 14 * len + 4, 14 * len + 4, shader );
}

/*
=================
CG_DrawCompass

NERVE - SMF
=================
*/
static void Wolfcam_DrawCompass( void ) {
	float basex = 290, basey = 420;
	float basew = 60, baseh = 60;
	snapshot_t	*snap;
	vec4_t hcolor;
	float angle;
	int i;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

	if ( cgs.gametype < GT_WOLF)  
		return;

    //if (wcg.clientNum != cg.snap->ps.clientNum  &&  !cent->currentValid)
    if (!wclients[wcg.clientNum].currentValid)
        return;

	//if ( cg.snap->ps.pm_flags & PMF_LIMBO || cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
	//	return;

	angle = ( cg.refdefViewAngles[YAW] + 180.f ) / 360.f - ( 0.25 / 2.f );

	VectorSet( hcolor, 1.f,1.f,1.f );
	hcolor[3] = cg_hudAlpha.value; 
	trap_R_SetColor( hcolor );

	CG_DrawRotatedPic( basex, basey, basew, baseh, trap_R_RegisterShader( "gfx/2d/compass2.tga" ), angle );
	CG_DrawPic( basex, basey, basew, baseh, trap_R_RegisterShader( "gfx/2d/compass.tga" ) );

	// draw voice chats
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		centity_t *cent = &cg_entities[i];

		if ( ci->clientNum == i || !cgs.clientinfo[i].infoValid || ci->team != cgs.clientinfo[i].team )
			continue;

		// also draw revive icons if cent is dead and player is a medic
		if ( cent->voiceChatSpriteTime < cg.time ) {
			continue;
		}

		if ( cgs.clientinfo[i].health <= 0 ) {
			// reset
			cent->voiceChatSpriteTime = cg.time;
			continue;
		}

        //		Wolfcam_DrawCompassIcon( basex, basey, basew, baseh, cg.snap->ps.origin, cent->lerpOrigin, cent->voiceChatSprite );
		Wolfcam_DrawCompassIcon( basex, basey, basew, baseh, cg.refdef.vieworg, cent->lerpOrigin, cent->voiceChatSprite );
	}

	// draw explosives if an engineer
	//if ( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
    if (es->teamNum == PC_ENGINEER) {  // PC_ENGINEER
		if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
			snap = cg.nextSnap;
		} else {
			snap = cg.snap;
		}

		for ( i = 0; i < snap->numEntities; i++ ) {
			centity_t *cent = &cg_entities[ snap->entities[ i ].number ];

			if ( cent->currentState.eType != ET_EXPLOSIVE_INDICATOR ) {
				continue;
			}

			if ( cent->currentState.teamNum == 1 && ci->team == TEAM_RED )
				continue;
			else if ( cent->currentState.teamNum == 2 && ci->team == TEAM_BLUE )
				continue;

			Wolfcam_DrawCompassIcon( basex, basey, basew, baseh, cg.refdef.vieworg, cent->lerpOrigin, trap_R_RegisterShader( "sprites/destroy.tga" ) );
		}
	}

	// draw revive medic icons
	//if ( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
    if (es->teamNum == PC_MEDIC) {
		if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
			snap = cg.nextSnap;
		} else {
			snap = cg.snap;
		}

		for ( i = 0; i < snap->numEntities; i++ ) {
			entityState_t *ent = &snap->entities[i];

			if ( ent->eType != ET_PLAYER )
				continue;

			if ( ( ent->eFlags & EF_DEAD ) && ent->number == ent->clientNum ) {
				if ( !cgs.clientinfo[ent->clientNum].infoValid || ci->team != cgs.clientinfo[ent->clientNum].team )
					continue;

				Wolfcam_DrawCompassIcon( basex, basey, basew, baseh, cg.refdef.vieworg, ent->pos.trBase, cgs.media.medicReviveShader );
			}
		}
	}
}
// -NERVE - SMF

/*
=================
CG_Draw2D
=================
*/
static void Wolfcam_Draw2D( void ) {
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

	// if we are taking a levelshot for the menu, don't draw anything
	if ( cg.levelShot ) {
		return;
	}

	if (cg_draw2D.integer == 0) {
		if (mm_demoCrosshair.integer) Wolfcam_DrawCrosshair();
		if (mm_demoPopUp.integer) {
			CG_DrawCenterString();
			CG_DrawNotify();
			//CG_DrawObjectiveInfo();
			//CG_DrawObjectiveIcons();
		}
		return;
	}

	if (mm_blood.integer) CG_ScreenFade();

	if(cg.cameraMode)	//----(SA)	no 2d when in camera view
		return;

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		CG_DrawIntermission();
		return;
	}

    //if (!(es->eFlags & EF_DEAD)  &&  cent->currentValid)
    if (!(es->eFlags & EF_DEAD)  &&  wclients[wcg.clientNum].currentValid)
		if (mm_blood.integer) Wolfcam_DrawFlashBlendBehindHUD();

#ifndef PRE_RELEASE_DEMO
	if (cg_uselessNostalgia.integer) {
		Wolfcam_DrawCrosshair();
		Wolfcam_Draw2D2();
		return;
	}
#endif

	if ( ci->team == TEAM_SPECTATOR ) {
		CG_DrawSpectator();
		Wolfcam_DrawCrosshair();
		Wolfcam_DrawCrosshairNames();

		// NERVE - SMF - we need to do this for spectators as well
		if ( cgs.gametype >= GT_TEAM ) {
			CG_DrawTeamInfo();
		}
	} else {
		// don't draw any status if dead
        //		if ( cg.snap->ps.stats[STAT_HEALTH] > 0 || (cg.snap->ps.pm_flags & PMF_FOLLOW) ) {
        if (!(es->eFlags & EF_DEAD)) { //  &&  cent->currentValid) {
            //if (cent->currentValid  ||  wcg.clientNum == cg.clientNum) {
            if (wclients[wcg.clientNum].currentValid) {

                Wolfcam_DrawCrosshair();

                Wolfcam_DrawDynamiteStatus();
                Wolfcam_DrawCrosshairNames();

                // DHM - Nerve :: Don't draw icon in upper-right when switching weapons
                //CG_DrawWeaponSelect();
                CG_DrawPickupItem();
            }
		}
		if ( cgs.gametype >= GT_TEAM ) {
			CG_DrawTeamInfo();
		}
		if ( cg_drawStatus.integer ) {
			Menu_PaintAll();
			CG_DrawTimedMenus();
		}
	}

	CG_DrawVote();

    //	CG_DrawLagometer();

	if (!cg_paused.integer) {
		CG_DrawUpperRight();
        //        CG_Printf ("upper right\n");
	}

	// don't draw center string if scoreboard is up
	if ( !CG_DrawScoreboard() ) {
		CG_DrawCenterString();

		//CG_DrawFollow();
		Wolfcam_DrawWarmup();

		//if ( cg_drawNotifyText.integer )
			CG_DrawNotify();

		// NERVE - SMF
		if ( cg_drawCompass.integer )
			Wolfcam_DrawCompass();

		CG_DrawObjectiveInfo();
		Wolfcam_DrawObjectiveIcons();

		//CG_DrawSpectatorMessage();

		//CG_DrawLimboMessage();

        Wolfcam_DrawFollowing ();
		Wolfcam_DrawDebugString ();
		// -NERVE - SMF
	}

	// Ridah, draw flash blends now
	if (mm_blood.integer) Wolfcam_DrawFlashBlend();
}


/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void Wolfcam_DrawActive (stereoFrame_t stereoView) 
{
	float		separation;
	vec3_t		baseOrg;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

	// optionally draw the info screen instead
	if ( !cg.snap ) {
		CG_DrawInformation();
		return;
	}

	// if they are waiting at the mission stats screen, show the stats
	if (cg_gameType.integer == GT_SINGLE_PLAYER)
	{
		if ( strlen( cg_missionStats.string ) > 1 ) {
			trap_Cvar_Set( "com_expectedhunkusage", "-2" );
			CG_DrawInformation();
			return;
		}
	}

	// optionally draw the tournement scoreboard instead
	if (0)  {  //( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR && ( cg.snap->ps.pm_flags & PMF_SCOREBOARD ) ) {
		CG_DrawTourneyScoreboard();
		return;
	}

	switch ( stereoView ) {
	case STEREO_CENTER:
		separation = 0;
		break;
	case STEREO_LEFT:
		separation = -cg_stereoSeparation.value / 2;
		break;
	case STEREO_RIGHT:
		separation = cg_stereoSeparation.value / 2;
		break;
	default:
		separation = 0;
		CG_Error( "CG_DrawActive: Undefined stereoView" );
	}


	// clear around the rendered view if sized down
	CG_TileClear();

	// offset vieworg appropriately if we're doing stereo separation
	VectorCopy( cg.refdef.vieworg, baseOrg );
	if ( separation != 0 ) {
		VectorMA( cg.refdef.vieworg, -separation, cg.refdef.viewaxis[1], cg.refdef.vieworg );
	}

	cg.refdef.glfog.registered = 0;	// make sure it doesn't use fog from another scene

	// NERVE - SMF - activate limbo menu and draw small 3d window
	CG_ActivateLimboMenu();

	if ( cg.limboMenu ) {
		float x, y, w, h;
		x = LIMBO_3D_X;
		y = LIMBO_3D_Y;
		w = LIMBO_3D_W;
		h = LIMBO_3D_H;

		cg.refdef.width = 0;
		CG_AdjustFrom640( &x, &y, &w, &h );

		cg.refdef.x = x;
		cg.refdef.y = y;
		cg.refdef.width = w;
		cg.refdef.height = h;
	}
	// -NERVE - SMF

	CG_ShakeCamera();		// NERVE - SMF

	trap_R_RenderScene( &cg.refdef );

	// restore original viewpoint if running stereo
	if ( separation != 0 ) {
		VectorCopy( baseOrg, cg.refdef.vieworg );
	}

	// draw status bar and other floating elements
    Wolfcam_Draw2D ();
}


