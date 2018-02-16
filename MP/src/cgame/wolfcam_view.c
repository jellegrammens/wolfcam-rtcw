#include "cg_local.h"
#include "wolfcam_local.h"


#define FOCUS_DISTANCE  400     //800   //512
int Wolfcam_OffsetThirdPersonView (void)
{
        vec3_t          forward, right, up;
        vec3_t          view;
        vec3_t          focusAngles;
        trace_t         trace;
        static vec3_t   mins = { -4, -4, -4 };
        static vec3_t   maxs = { 4, 4, 4 };
        vec3_t          focusPoint;
        float           focusDist;
        float           forwardScale, sideScale;
        clientInfo_t *ci;
        centity_t *cent;

        ci = &cgs.clientinfo[wcg.clientNum];
        cent = &cg_entities[cgs.clientinfo[wcg.clientNum].clientNum];

        //cent->currentState.eFlags &= ~EF_NODRAW;
        //cent->nextState.eFlags &= ~EF_NODRAW;

        VectorCopy (cg.refdef.vieworg, wcg.vieworgDemo);
        VectorCopy (cg.refdefViewAngles, wcg.refdefViewAnglesDemo);

        VectorCopy (cent->lerpOrigin, cg.refdef.vieworg);
        VectorCopy (cent->lerpAngles, cg.refdefViewAngles);

        cg.refdef.vieworg[2] += DEFAULT_VIEWHEIGHT;
        VectorCopy (cg.refdefViewAngles, focusAngles);
        if (cent->currentState.eFlags & EF_DEAD) {
            // rain - #254 - force yaw to 0 if we're tracking a medic
            //if( cg.snap->ps.viewlocked != 7 ) {  //FIXME wolfcam
            if (1) {
                focusAngles[YAW] = SHORT2ANGLE(cg.predictedPlayerState.stats[STAT_DEAD_YAW]);  //FIXME wolfencam
                cg.refdefViewAngles[YAW] = SHORT2ANGLE(cg.predictedPlayerState.stats[STAT_DEAD_YAW]);
            }
        }

        if ( focusAngles[PITCH] > 45 )
            focusAngles[PITCH] = 45; 
        AngleVectors( focusAngles, forward, NULL, NULL );

        if (0) {  //FIXME wolfcam (cg_thirdPerson.integer == 2 ) 
            //VectorCopy( cg.predictedPlayerState.origin, focusPoint );
        } else
            VectorMA( cg.refdef.vieworg, FOCUS_DISTANCE, forward, focusPoint );

        VectorCopy (cg.refdef.vieworg, view);
        view[2] += 8;
        cg.refdefViewAngles[PITCH] *= 0.5;
        AngleVectors( cg.refdefViewAngles, forward, right, up );
        forwardScale = cos( cg_thirdPersonAngle.value / 180 * M_PI );
        sideScale = sin( cg_thirdPersonAngle.value / 180 * M_PI );
        VectorMA( view, -cg_thirdPersonRange.value * forwardScale, forward, view);
        VectorMA( view, -cg_thirdPersonRange.value * sideScale, right, view );
        // trace a ray from the origin to the viewpoint to make sure the view isn't
        // in a solid block.  Use an 8 by 8 block to prevent the view from near clipping anything

        CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, ci->clientNum, MASK_SOLID );
        if ( trace.fraction != 1.0 ) {
            VectorCopy( trace.endpos, view );
            view[2] += (1.0 - trace.fraction) * 32;
            // try another trace to this position, because a tunnel may have the ceiling close enough that this is poking out
            CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, ci->clientNum, MASK_SOLID);
            VectorCopy( trace.endpos, view );
        }
        VectorCopy( view, cg.refdef.vieworg);
        // select pitch to look at focus point from vieword
        VectorSubtract( focusPoint, cg.refdef.vieworg, focusPoint );
        focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );
        if ( focusDist < 1 ) {
            focusDist = 1;  // should never happen
        }
        cg.refdefViewAngles[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist);
        cg.refdefViewAngles[YAW] -= cg_thirdPersonAngle.value;

        AnglesToAxis (cg.refdefViewAngles, cg.refdef.viewaxis );

        //VectorCopy (cg.refdef.vieworg, cg.refdef_current->vieworg);
        //AnglesToAxis (cg.refdefViewAngles, cg.refdef_current->viewaxis);
        //FIXME wolfcam CG_CalcFov
        return 0;
}

/*
===============
Wolfcam_OffsetFirstPersonView

Sets cg.refdef view values if following other players
===============
*/
int Wolfcam_OffsetFirstPersonView (void)
{
    int clientNum;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;
    wclient_t *wc;
    float ratio;
    float v_dmg_pitch;
    //qboolean wasBinocZooming;
    int timeDelta;
    qboolean notDrawing;
    float bob;
    float speed;
    float delta;
    qboolean                useLastValidBob = qfalse;
    

    clientNum = wcg.clientNum;

    ci = &cgs.clientinfo[clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;
    wc = &wclients[wcg.clientNum];

	if (cg.snap->ps.clientNum != cg.clientNum) {
		//CG_Printf ("^3limbo following ^7%s\n", cgs.clientinfo[cg.snap->ps.clientNum].name);
		//CG_DrawBigString (20, 200, va("^3limbo following ^7%s", cgs.clientinfo[cg.snap->ps.clientNum].name), 1.0F);

		//strcpy (globalDebugString, va("limbo following %s", cgs.clientinfo[cg.snap->ps.clientNum].name));
	} else {
		globalDebugString[0] = '\0';
	}

#if 1
    if (wcg.clientNum == cg.snap->ps.clientNum) {  //(wcg.clientNum == cg.snap->ps.clientNum) {
        cg.renderingThirdPerson = 0;
        trap_Cvar_Set ("cg_thirdPerson", "0");
		cent->currentState.eFlags |= EF_NODRAW;
		cent->nextState.eFlags |= EF_NODRAW;
    } else {
        cg.renderingThirdPerson = 1;
        trap_Cvar_Set ("cg_thirdPerson", "1");
		cent->currentState.eFlags |= EF_NODRAW;
		cent->nextState.eFlags |= EF_NODRAW;

    }
#endif

    notDrawing = cent->currentState.eFlags & EF_NODRAW;


    VectorCopy (cent->lerpOrigin, cg.refdef.vieworg);
    VectorCopy (cent->lerpAngles, cg.refdefViewAngles);

    // CG_TransitionPlayerState for wcg.clientNum
    //FIXME wolfcam don't know
#if 0
    if (cent->currentState.eFlags & EF_DEAD) {
        trap_SendConsoleCommand ("-zoom\n");
    }
#endif

#if 0
    wasBinocZooming = qfalse;
    if (wolfcam_esPrev.eFlags & EF_ZOOMING  &&  wolfcam_esPrev.weapon == WP_BINOCULARS) {
        wasBinocZooming = qtrue;
    }

    if (wasBinocZooming == qfalse  &&  cent->currentState.weapon == WP_BINOCULARS  &&  cent->currentState.eFlags & EF_ZOOMING) {
        wolfcam_binocZoomTime = cg.time;
        Com_Printf ("ent zooming with binocs\n");
    }
#endif

#if 1
    // add view height
    if (es->eFlags & EF_CROUCHING) {
        cg.refdef.vieworg[2] += CROUCH_VIEWHEIGHT;
        //cg.refdef.vieworg[2] += CROUCH_VIEWHEIGHT;
        //Com_Printf ("test.....\n");
    }
    else {
        cg.refdef.vieworg[2] += DEFAULT_VIEWHEIGHT;
        //cg.refdef.vieworg[2] += DEFAULT_VIEWHEIGHT;
        //Com_Printf ("test.....\n");
    }
#endif

    timeDelta = cg.time - wc->duckTime;
    {
        if (timeDelta < 0)
            wc->duckTime = cg.time - DUCK_TIME;
        if (timeDelta < DUCK_TIME) {
            cg.refdef.vieworg[2] -= wc->duckChange * (DUCK_TIME - timeDelta) / DUCK_TIME;
        }
    }

#if 0
    //if (es->eFlags & EF_DEAD  &&  ci->infoValid  &&  !notDrawing) {
    if (!cent->currentValid  ||  es->eFlags & EF_DEAD) {
        return qfalse;
    }
#endif

    //FIXME  fall, stepOffset?

    // add angles based on bob

    //    if (cent->currentValid) {
    if (wclients[wcg.clientNum].currentValid) {
    // make sure the bob is visible even at low speeds
    speed = wc->xyspeed > 200 ? wc->xyspeed : 200;


    /////////////////////
    if( !wc->bobfracsin  &&  wc->lastvalidBobfracsin > 0 ) {
        // 200 msec to get back to center from 1
        // that's 1/200 per msec = 0.005 per msec
        wc->lastvalidBobfracsin -= 0.005 * cg.frametime;
        useLastValidBob = qtrue;
    }
       
    //    delta = useLastValidBob ? wolfcam_lastvalidBobfracsin * cg_bobpitch.value * speed : cg.bobfracsin * cg_bobpitch.value * speed;
    delta = useLastValidBob ? wc->lastvalidBobfracsin * cg_bobpitch.value * speed : wc->bobfracsin * cg_bobpitch.value * speed;

    if (es->eFlags & EF_CROUCHING)
        delta *= 3;             // crouching
    cg.refdefViewAngles[PITCH] += delta;
    delta = useLastValidBob ? wc->lastvalidBobfracsin * cg_bobroll.value * speed : wc->bobfracsin * cg_bobroll.value * speed;

    if (es->eFlags & EF_CROUCHING)
        delta *= 3;             // crouching accentuates roll
    if( useLastValidBob ) {
        if( wc->lastvalidBobcycle & 1 )
            delta = -delta;
    } else if (wc->bobcycle & 1)
        delta = -delta;
    cg.refdefViewAngles[ROLL] += delta;  //FIXME wolfcam ok for weapons, sucks for regular walking


    //FIXME wolfcam using v_dmg_pitch == kick == -5
    v_dmg_pitch = -5;
    ratio = cg.time - wc->bulletDamageTime;
    if (ratio < DAMAGE_TIME  &&  !(es->eFlags & EF_DEAD)  &&  ci->infoValid) {
        if (ratio < DAMAGE_DEFLECT_TIME) {
            ratio /= DAMAGE_DEFLECT_TIME;
            cg.refdefViewAngles[PITCH] += ratio * v_dmg_pitch;
            //FIXME wolfcam ROLL
        } else {
            ratio = 1.0 - (ratio - DAMAGE_DEFLECT_TIME) / DAMAGE_RETURN_TIME;
            if (ratio > 0) {
                cg.refdefViewAngles[PITCH] += ratio * v_dmg_pitch;
                //FIXME wolfcam ROLL
            }
        }
    }

    bob = wc->bobfracsin * wc->xyspeed * cg_bobup.value;
    if (bob > 6) {
        bob = 6;
    }
    cg.refdef.vieworg[2] += bob;  //FIXME wolfcam ok for weapons, not player movement
    }

    // ZoomSway
    if (wc->zoomval)
    {
        float spreadfrac, phase;
        
        if (es->eFlags & EF_MG42_ACTIVE) {  //  ||  es->eFlags & EF_AAGUN_ACTIVE) {
        } else {
            //spreadfrac = (float)cg.snap->ps.aimSpreadScale / 255.0;
            spreadfrac = 0.25;  //FIXME wolfcam
            phase = cg.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
            cg.refdefViewAngles[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * (spreadfrac+ZOOM_PITCH_MIN_AMPLITUDE);
            phase = cg.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
            cg.refdefViewAngles[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * (spreadfrac+ZOOM_YAW_MIN_AMPLITUDE);
        }
    }

    if (wcg.leanf != 0) {
        //add leaning offset
        vec3_t  right;
        cg.refdefViewAngles[2] += wcg.leanf/2.0f;
        AngleVectors(cg.refdefViewAngles, NULL, right, NULL);
        VectorMA(cg.refdef.vieworg, wcg.leanf, right, cg.refdef.vieworg);
    }

    AnglesToAxis (cg.refdefViewAngles, cg.refdef.viewaxis );

    //VectorCopy (cg.refdef.vieworg, cg.refdef_current->vieworg);
    //AnglesToAxis (cg.refdefViewAngles, cg.refdef_current->viewaxis);

    return 0;
}

static void Wolfcam_AdjustZoomVal (float val, int type)
{
    wclient_t *wc = &wclients[wcg.clientNum];

    wc->zoomval += val;
    if (wc->zoomval > zoomTable[type][ZOOM_OUT])
        wc->zoomval = zoomTable[type][ZOOM_OUT];
    if(wc->zoomval < zoomTable[type][ZOOM_IN])
        wc->zoomval = zoomTable[type][ZOOM_IN];
}


void Wolfcam_ZoomIn (void)
{
    centity_t *cent;

    cent = &cg_entities[cgs.clientinfo[wcg.clientNum].clientNum];

    if (cent->currentState.weapon == WP_SNIPERRIFLE) {
        Wolfcam_AdjustZoomVal (-(cg_zoomStepSniper.value), ZOOM_SNIPER);
    } else if (wclients[wcg.clientNum].zoomedBinoc) {
        Wolfcam_AdjustZoomVal (-(cg_zoomStepSniper.value), ZOOM_SNIPER);
    }
}

void Wolfcam_ZoomOut (void)
{
    centity_t *cent;

    cent = &cg_entities[cgs.clientinfo[wcg.clientNum].clientNum];

    if (cent->currentState.weapon == WP_SNIPERRIFLE) {
        Wolfcam_AdjustZoomVal(cg_zoomStepSniper.value, ZOOM_SNIPER);
    } else if (wclients[wcg.clientNum].zoomedBinoc) {
        Wolfcam_AdjustZoomVal(cg_zoomStepSniper.value, ZOOM_SNIPER); // JPW NERVE per atvi request BINOC);
    }
}


void Wolfcam_Zoom (void)
{
    entityState_t *es;
    wclient_t *wc;

    es = &(cg_entities[cgs.clientinfo[wcg.clientNum].clientNum].currentState);
    wc = &wclients[wcg.clientNum];

	// OSP - Fix for demo playback
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback ) {
		cg.predictedPlayerState.eFlags = cg.snap->ps.eFlags;
		cg.predictedPlayerState.weapon = cg.snap->ps.weapon;

		// check for scope wepon in use, and switch to if necessary
		// OSP - spec/demo scaling allowances
		if(es->weapon == WP_FG42SCOPE)
			wc->zoomval = (wc->zoomval == 0) ? cg_zoomDefaultSniper.value : wc->zoomval; // JPW NERVE was DefaultFG, changed per atvi req
		else if ( es->weapon == WP_SNIPERRIFLE )
			wc->zoomval = (wc->zoomval == 0) ? cg_zoomDefaultSniper.value : wc->zoomval;
		else if(!(es->eFlags & EF_ZOOMING))
			wc->zoomval = 0;
	}

	if(es->eFlags & EF_ZOOMING) {
		if ( wc->zoomedBinoc )
			return;
		wc->zoomedBinoc	= qtrue;
		wc->zoomTime	= cg.time;
		wc->zoomval = cg_zoomDefaultSniper.value; // JPW NERVE was DefaultBinoc, changed per atvi req
	}
	else {
		if (wc->zoomedBinoc) {
			wc->zoomedBinoc	= qfalse;
			wc->zoomTime	= cg.time;

			// check for scope weapon in use, and switch to if necessary
			//if( cg.weaponSelect == WP_FG42SCOPE ) {
            if (es->weapon == WP_FG42SCOPE) {
				wc->zoomval = cg_zoomDefaultSniper.value; // JPW NERVE was DefaultFG, changed per atvi req
			} else if ( es->weapon == WP_SNIPERRIFLE ) {
				wc->zoomval = cg_zoomDefaultSniper.value;
			} else {
				wc->zoomval = 0;
			}
		} else {
//bani - we now sanity check to make sure we can't zoom non-zoomable weapons
//zinx - fix for #423 - don't sanity check while following
			if (!((cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback)) {
				switch( es->weapon ) {
					case WP_FG42SCOPE:
					case WP_SNIPERRIFLE:
						break;
					default:
						wc->zoomval = 0;
						break;
				}
			}
		}
	}
}

#define	WAVE_AMPLITUDE	1
#define	WAVE_FREQUENCY	0.4

int Wolfcam_CalcFov (void) 
{
#if 1  //FIXME wolfcam-rtcw
	static float lastfov = 90;		// for transitions back from zoomed in modes
	float	x;
	float	phase;
	float	v;
	int		contents;
	float	fov_x, fov_y;
	float	zoomFov;
	float	f;
	int		inwater;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;
    wclient_t *wc;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;
    wc = &wclients[wcg.clientNum];

	Wolfcam_Zoom();

	//if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 && !(cg.snap->ps.pm_flags & PMF_FOLLOW) ) 
    if (es->eFlags & EF_DEAD)
	{
		wc->zoomedBinoc = qfalse;
		wc->zoomTime = 0;
		wc->zoomval = 0;
	}

	if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		// if in intermission, use a fixed value
		fov_x = 90;
	} else {
		fov_x = cg_fov.value;

		//if( !cg.renderingThirdPerson || developer.integer ) {  //FIXME wolfcam
        if ( !(es->eFlags & EF_DEAD) ) {  // ||  developer.integer) {
			// account for zooms
			if(wc->zoomval) {
				zoomFov = wc->zoomval;	// (SA) use user scrolled amount

				if ( zoomFov < 1 ) {
					zoomFov = 1;
				} else if ( zoomFov > 160 ) {
					zoomFov = 160;
				}
			} else {
					zoomFov = lastfov;
			}
			
			// do smooth transitions for the binocs
			if(wc->zoomedBinoc) {		// binoc zooming in
				f = ( cg.time - wc->zoomTime ) / (float)ZOOM_TIME;
				if ( f > 1.0 ) {
					fov_x = zoomFov;
				} else {
					fov_x = fov_x + f * ( zoomFov - fov_x );
				}
				lastfov = fov_x;
			} else if (wc->zoomval) {	// zoomed by sniper/snooper
				fov_x = wc->zoomval;
				lastfov = fov_x;
			} else {					// binoc zooming out
				f = ( cg.time - wc->zoomTime ) / (float)ZOOM_TIME;
				if ( f > 1.0 ) {
					fov_x = fov_x;
				} else {
					fov_x = zoomFov + f * ( fov_x - zoomFov);
				}
			}
		}
	}

    //FIXME wolfcam hmm
	cg.refdef.rdflags &= ~RDF_SNOOPERVIEW;

	// Arnout: mg42 zoom
    //FIXME wolfcam check to see if attached to a gun
	//if (0) {  //if (cg.snap->ps.persistant[PERS_HWEAPON_USE]) {
    if (es->eFlags & EF_MG42_ACTIVE) {
    
		fov_x = 55;
        //} else if( es->weapon == WP_MOBILE_MG42_SET ) {
		//fov_x = 55;
        //} else if( es->eFlags & EF_MOUNTEDTANK ) {
		//fov_x = 75;
	}

	//if( cg.showGameView ) {
	//	fov_x = fov_y = 60.f;
	//}

	// Arnout: this is weird... (but ensures square pixel ratio!)
	x = cg.refdef.width / tan( fov_x / 360 * M_PI );
	fov_y = atan2( cg.refdef.height, x );
	fov_y = fov_y * 360 / M_PI;
	// And this seems better - but isn't really
	//fov_y = fov_x / cgs.glconfig.windowAspect;

	// warp if underwater
	//if ( cg_pmove.waterlevel == 3 ) {
	contents = CG_PointContents( cg.refdef.vieworg, -1 );  // wolfcam this is ok I think
	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ){
		phase = cg.time / 1000.0 * WAVE_FREQUENCY * M_PI * 2;
		v = WAVE_AMPLITUDE * sin( phase );
		fov_x += v;
		fov_y -= v;
		inwater = qtrue;
		cg.refdef.rdflags |= RDF_UNDERWATER;
	} else {
		cg.refdef.rdflags &= ~RDF_UNDERWATER;
		inwater = qfalse;
	}

	// set it
	cg.refdef.fov_x = fov_x;
	cg.refdef.fov_y = fov_y;

/*
	if( cg.predictedPlayerState.eFlags & EF_PRONE ) {
		cg.zoomSensitivity = cg.refdef.fov_y / 500.0;
	} else
*/
	// rain - allow freelook when dead until we tap out into limbo
	if (es->eFlags & EF_DEAD) {  //if( cg.snap->ps.pm_type == PM_FREEZE || (cg.snap->ps.pm_type == PM_DEAD && (cg.snap->ps.pm_flags & PMF_LIMBO)) || cg.snap->ps.pm_flags & PMF_TIME_LOCKPLAYER ) {  //FIXME wolfcam
		// No movement for pauses
		cg.zoomSensitivity = 0;
	} else if ( !wc->zoomedBinoc ) {
		// NERVE - SMF - fix for zoomed in/out movement bug
		if ( wc->zoomval ) {
			cg.zoomSensitivity = 0.6 * ( wc->zoomval / 90.f );	// NERVE - SMF - changed to get less sensitive as you zoom in
//				cg.zoomSensitivity = 0.1;
		} else {
			cg.zoomSensitivity = 1;
		}
		// -NERVE - SMF
	} else {
		cg.zoomSensitivity = cg.refdef.fov_y / 75.0;
	}

	return inwater;
#endif
    
    return 0;

}
