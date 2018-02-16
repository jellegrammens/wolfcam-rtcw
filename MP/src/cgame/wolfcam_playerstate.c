#include "cg_local.h"
#include "wolfcam_local.h"

#if 0
static int wolfcam_find_next_valid_client (int clientNum);

static int wolfcam_find_next_valid_team_client (int clientNum, team_t team)
{
    int i;
    qboolean foundNewClient;

    if (team == TEAM_FREE)
        return wolfcam_find_next_valid_client(clientNum);

    foundNewClient = qfalse;
    for (i = clientNum + 1;  i < MAX_CLIENTS;  i++) {
        if (wclients[i].currentValid  &&  cgs.clientinfo[i].team == team) {
            foundNewClient = qtrue;
            break;
        }
    }
    
    if (foundNewClient)
        return i;

    for (i = 0;  i < clientNum;  i++) {
        if (wclients[i].currentValid  &&  cgs.clientinfo[i].team == team) {
            foundNewClient = qtrue;
            break;
        }
    }

    if (foundNewClient)
        return i;

    //return clientNum;
    if (wclients[clientNum].currentValid)
        return clientNum;

    return wolfcam_find_next_valid_client(clientNum);
}

static int wolfcam_find_next_valid_client (int clientNum)
{
    int i;
    qboolean foundNewClient;

    foundNewClient = qfalse;
    for (i = clientNum + 1;  i < MAX_CLIENTS;  i++) {
#if 0
        if (cgs.clientinfo[i].infoValid  &&  cgs.clientinfo[i].team == cgs.clientinfo[clientNum].team) {
            if (cg_entities[cgs.clientinfo[i].clientNum].currentValid) {
                foundNewClient = qtrue;
                break;
            }
        }
#endif
        if (wclients[i].currentValid) {
            foundNewClient = qtrue;
            break;
        }
    }

    if (foundNewClient)
        return i;

    for (i = 0;  i < clientNum;  i++) {
#if 0
        if (cgs.clientinfo[i].infoValid  &&  cgs.clientinfo[i].team == cgs.clientinfo[clientNum].team) {
            if (cg_entities[cgs.clientinfo[i].clientNum].currentValid) {
                foundNewClient = qtrue;
                break;
            }
        }
#endif
        if (wclients[i].currentValid) {
            foundNewClient = qtrue;
            break;
        }
    }

    if (foundNewClient)
        return i;

    return clientNum;
}
#endif

int wolfcam_find_client_to_follow (void)
{                
    int i;
    qboolean foundNewClient;
    qboolean foundBackupClient;
    int backupClient = -1;  // cg.snap->ps.clientNum acts as second backup client

    if (wclients[wcg.selectedClientNum].currentValid)
        return wcg.selectedClientNum;

    foundNewClient = qfalse;
    foundBackupClient = qfalse;

    for (i = wcg.clientNum;  i < MAX_CLIENTS;  i++) {
        if (!cgs.clientinfo[i].infoValid)
            continue;
        if (!wclients[i].currentValid)
            continue;
        //if (i == cg.snap->ps.clientNum  &&  wolfcam_avoidDemoPov.integer)
        if (i == cg.clientNum  &&  wolfcam_avoidDemoPov.integer)
            continue;
        
        if (cgs.clientinfo[i].team != cgs.clientinfo[wcg.selectedClientNum].team  &&  wolfcam_tryToStickWithTeam.integer) {
            if (!foundBackupClient) {
                foundBackupClient = qtrue;
                backupClient = i;
            }
            continue;
        }
        foundNewClient = qtrue;
        break;
    }

    if (foundNewClient)
        return i;

    for (i = 0;  i < wcg.clientNum;  i++) {
        if (!cgs.clientinfo[i].infoValid)
            continue;
        if (!wclients[i].currentValid)
            continue;
        //if (i == cg.snap->ps.clientNum  &&  wolfcam_avoidDemoPov.integer)
        if (i == cg.clientNum  &&  wolfcam_avoidDemoPov.integer)
            continue;
        if (cgs.clientinfo[i].team != cgs.clientinfo[wcg.selectedClientNum].team  &&  wolfcam_tryToStickWithTeam.integer) {
            if (!foundBackupClient) {
                foundBackupClient = qtrue;
                backupClient = i;
            }
            continue;
        }
        foundNewClient = qtrue;
        break;
    }

    if (foundNewClient)
        return i;

    if (foundBackupClient)
        return backupClient;

    if (wclients[wcg.clientNum].currentValid)
        return wcg.clientNum;

    return cg.snap->ps.clientNum;
}


//FIXME wolfcam move stuff from Wolfcam_Offset* here
void Wolfcam_TransitionPlayerState (void)
{
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *cs, *os;
    wclient_t *wc;
    qboolean wasCrouching, wasStanding;
    qboolean isCrouching, isStanding;
    float duckChange;
    //vec3_t velocity;
    float bobmove = 0.0;
    //int i;
    //int grenadeTimeLeft;
    int oldClientNum;
    static int oldServerTime = -1;

    if (!cg.snap)
        return;

    if (cg.snap->serverTime <= oldServerTime)
        return;
    
    oldServerTime = cg.snap->serverTime;
    //CG_Printf ("serverTime: %d\n", cg.snap->serverTime);

    oldClientNum = wcg.clientNum;

#if 0
    if (wclients[wcg.selectedClientNum].currentValid) {
        wcg.clientNum = wcg.selectedClientNum;
    } else if (!wclients[wcg.clientNum].currentValid  ||  (wcg.clientNum == cg.snap->ps.clientNum  &&  wolfcam_avoidDemoPov.integer)  ||  (cgs.clientinfo[wcg.clientNum].team != cgs.clientinfo[wcg.selectedClientNum].team  &&  wolfcam_tryToStickWithTeam.integer)) { 
        wcg.clientNum = wolfcam_find_next_valid_team_client (wcg.clientNum, wolfcam_tryToStickWithTeam.integer ? cgs.clientinfo[wcg.selectedClientNum].team : TEAM_FREE);  //FIXME wolfcam team client
    } 
#endif

    wcg.clientNum = wolfcam_find_client_to_follow ();

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    cs = &cent->currentState;
    wc = &wclients[wcg.clientNum];
    os = &wc->esPrev;

    if (wcg.clientNum != oldClientNum)
        memcpy (os, cs, sizeof(entityState_t));

#if 0  // wolfcam testing
    //FIXME wolfcam, as 'follow' option, 
    //FIXME wolfcam not in this function
    //FIXME wolfcam finer grained cg_main timescale (don't skip too far ahead)
    if ((wcg.fastForwarding  &&  cg.clientNum == wcg.clientNum)  ||  ci->team == TEAM_SPECTATOR) {
        trap_Cvar_Set ("s_volume", wcg.oldVolume);
        trap_Cvar_Set ("timescale", wcg.oldTimescale);
        trap_SendConsoleCommand ("s_stop\n");
        wcg.fastForwarding = qfalse;
    }

    if (cg.clientNum != wcg.clientNum) {  //if (wcg.fastForward) {
        if (cg.time - wcg.validTime > (wolfcam_fast_forward_invalid_time.value * 1000.0)) {
            if (!wcg.fastForwarding) {
                //CG_Printf ("timescale: %f\n", cg_timescale.value);
                trap_Cvar_VariableStringBuffer ("s_volume", wcg.oldVolume, sizeof(wcg.oldVolume));
                trap_Cvar_VariableStringBuffer ("timescale", wcg.oldTimescale, sizeof(wcg.oldTimescale));
                trap_Cvar_Set ("s_volume", "0");
                trap_Cvar_Set ("timescale", wolfcam_fast_forward_timescale.string);
                wcg.fastForwarding = qtrue;
            }
        } else {
            if (wcg.fastForwarding) {
                //CG_Printf ("^3 setting timescale back: %s", wcg.oldTimescale);
                trap_Cvar_Set ("s_volume", wcg.oldVolume);
                trap_Cvar_Set ("timescale", wcg.oldTimescale);
                trap_SendConsoleCommand ("s_stop\n");
                wcg.fastForwarding = qfalse;
            }
        }
    }
#endif

    if (wc->weapAnimTimer > 0) {
        wc->weapAnimTimer -= cg.frametime;
        if (wc->weapAnimTimer < 0)
            wc->weapAnimTimer = 0;
    }
    //Com_Printf ("%d\n", cg.frametime);

#if 0  //FIXME wolfcam
    // OSP - MV client handling
    if(cg.mvTotalClients > 0) {
        if (ps->clientNum != ops->clientNum) {
            cg.thisFrameTeleport = qtrue;

            // clear voicechat
            cg.predictedPlayerEntity.voiceChatSpriteTime = 0;
            // CHECKME: should we do this here?
            cg_entities[ps->clientNum].voiceChatSpriteTime = 0;

            *ops = *ps;
        }
        CG_CheckLocalSounds( ps, ops );
        return;
    }
#endif

    // check for changing follow mode
    //FIXME wolfcam todo
#if 0
    if ( ps->clientNum != ops->clientNum ) {
        cg.thisFrameTeleport = qtrue;

        // clear voicechat
        cg.predictedPlayerEntity.voiceChatSpriteTime = 0;
        cg_entities[ps->clientNum].voiceChatSpriteTime = 0;

        // make sure we don't get any unwanted transition effects
        *ops = *ps;

        // DHM - Nerve :: After Limbo, make sure and do a CG_Respawn
        if ( ps->clientNum == cg.clientNum )
            ops->persistant[PERS_SPAWN_COUNT]--;
    }
#endif

#if 0  // wolfcam handled in events
    if (cs->eFlags & EF_FIRING) {
        wolfcam_lastFiredWeaponTime = 0;
        wolfcam_weaponFireTime += cg.frametime;
    } else {
        if (wolfcam_weaponFireTime > 500  && wolfcam_weaponFireTime) {  // wolfcam huh?
            wolfcam_lastFiredWeaponTime = cg.time;
        }
        wolfcam_weaponFireTime = 0;
    }
#endif

    //FIXME wolfcam EF_FIRING problem with rapid taps
    if (cs->eFlags & EF_FIRING  &&  !(os->eFlags & EF_FIRING)) {
        //CG_Printf ("^3now firing...  %d  old:%d\n", cs->weapon, os->weapon);
        if (cs->weapon == WP_GRENADE_LAUNCHER   ||
            cs->weapon == WP_GRENADE_PINEAPPLE  ||
            cs->weapon == WP_DYNAMITE) {
            //CG_Printf ("grenade time set: %d\n", cg.time);
            wc->grenadeDynoTime = cg.time;
        }
    }

    if (!(cs->eFlags & EF_FIRING))
        wc->grenadeDynoTime = 0;

#if 0
    if (os->weapon != cs->weapon) {
        wolfcam_weaponSwitchTime = cg.time;
        wolfcam_oldweapon = os->weapon;
    }
#endif
    // damage events (player is getting wounded)
    //FIXME wolfcam
#if 0
    if( ps->damageEvent != ops->damageEvent && ps->damageCount ) {
        CG_DamageFeedback( ps->damageYaw, ps->damagePitch, ps->damageCount );
    }
#endif

    // respawning 
    //FIXME wolfcam
#if 0
    if( ps->persistant[PERS_SPAWN_COUNT] != ops->persistant[PERS_SPAWN_COUNT]) {
        CG_Respawn( ps->persistant[PERS_REVIVE_COUNT] != ops->persistant[PERS_REVIVE_COUNT] ? qtrue : qfalse );
    }
#endif

#if 0  //FIXME wolfcam
    if ( cg.mapRestart ) {
        CG_Respawn( qfalse );
        cg.mapRestart = qfalse;
    }

    if ( cg.snap->ps.pm_type != PM_INTERMISSION 
         && ps->persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
        CG_CheckLocalSounds( ps, ops );
    }

#endif

    // check for going low on ammo
    //CG_CheckAmmo();



    //FIXME wolfcam
    // run events
    //CG_CheckPlayerstateEvents( ps, ops );

    // smooth the ducking viewheight change

    wasCrouching = wasStanding = qfalse;

    if (os->eFlags & EF_CROUCHING)
        wasCrouching = qtrue;
    else
        wasStanding = qtrue;

    isCrouching = isStanding = qfalse;

    if (cs->eFlags & EF_CROUCHING)
        isCrouching = qtrue;
    else
        isStanding = qtrue;

    duckChange = 0;

    if (isCrouching) {
        if (wasStanding)
            duckChange = CROUCH_VIEWHEIGHT - DEFAULT_VIEWHEIGHT;
    } else if (isStanding) {
        if (wasCrouching)
            duckChange = DEFAULT_VIEWHEIGHT - CROUCH_VIEWHEIGHT;
    }

    if (duckChange) {
        wc->duckChange = duckChange;
        wc->duckTime = cg.time;
    }

#if 1  //FIXME wolfcam doesn't seem to work
 {
     vec3_t velocity;

     //BG_EvaluateTrajectoryDelta (&cs->pos, cg.time, velocity, qfalse, -1);
     //BG_EvaluateTrajectoryDelta (&cs->pos, cg.time, velocity);
     VectorCopy (cs->pos.trDelta, velocity);
 
     if (wcg.clientNum != cg.snap->ps.clientNum)
         wc->xyspeed = sqrt (velocity[0] * velocity[0] + velocity[1] * velocity[1]);
     else  {// view what demo pov
         playerState_t *ps = &cg.predictedPlayerState;
         wc->xyspeed = sqrt( ps->velocity[0] * ps->velocity[0] +
                             ps->velocity[1] * ps->velocity[1] );
     }
 }
#endif
    //    Com_Printf ("%f %f %f\n", wolfcam_prevOrigin[0], wolfcam_prevOrigin[1], wolfcam_prevOrigin[2]);
#if 0
    {
        float xv, yv;

        //xv = (cent->lerpOrigin[0] - wolfcam_prevOrigin[0]) / cg.frametime;
        //yv = (cent->lerpOrigin[1] - wolfcam_prevOrigin[1]) / cg.frametime;
        //FIXME wolfcam 16 - 8
        xv = (cent->lerpOrigin[0] - wolfcam_prevOrigins[16 - 16][0]) / (cg.time - wolfcam_prevTimes[16 - 16]) * 1000;
        yv = (cent->lerpOrigin[1] - wolfcam_prevOrigins[16 - 16][1]) / (cg.time - wolfcam_prevTimes[16 - 16]) * 1000;
        wolfcam_xyspeed = sqrt ( xv * xv   +   yv * yv);
        //wolfcam_xyspeed = sqrt(xv * xv)   +  sqrt(yv * yv);
        //wolfcam_xyspeed = sqrt (xv * yv);
        //wolfcam_xyspeed *= 1000;
        //wolfcam_xyspeed *= 1000;
    }
    //wolfcam_xyspeed = 300;
#endif

    // wc->psbobCyle from bg_pmove.c
#if 0  //FIXME wolfcam detect "crash landing"
    if (crash_land)  // PM_CrsahLand ()
        wc->psbobCycle = 0;
#endif
    // PM_Footsteps ()
    //  if mg42 pm->ps->persistant[PERS_HWEAPON_USE], swimming, DEAD, in the air, then return

    //FIXME wolfcam, if not trying to move:
#if 0
    if ( !pm->cmd.forwardmove && !pm->cmd.rightmove ) {
        if (  pm->xyspeed < 5 ) {
            pm->ps->bobCycle = 0;   // start at beginning of cycle again
        }
        if (pm->xyspeed > 120) {
            return; // continue what they were doing last frame, until we stop
        }
        /* etc .. */
    }
#endif
    if (wc->xyspeed < 5) {
        wc->psbobCycle = 0;
        bobmove = 0.0;
    } else if (cs->eFlags & EF_CROUCHING)
        bobmove = 0.5;
    /*FIXME wolfcam else if (walking) {
      bobmove = 0.3;
}
    */
    else
        bobmove = 0.4;
    //pm->ps->bobCycle = (int)( old + bobmove * pml.msec ) & 255;
    //FIXME wolfcam  pml.msec relation to cg.frametime ?
    //    wolfcam_psbobCycle = (int) (wolfcam_psbobCycle + bobmove * cg.frametime ) & 255;
    //wolfcam_psbobCycle = (int) (wolfcam_psbobCycle + bobmove * (cg.frametime * 1000)) & 255;
    wc->psbobCycle = (int) (wc->psbobCycle + bobmove * (cg.frametime * 1)) & 255;
    //Com_Printf ("wolfcam_psbobCycle: %d    ps->bobCycle: %d\n", wolfcam_psbobCycle, cg.predictedPlayerState.bobCycle);
    //Com_Printf ("wolfcam_xyspeed: %f  cg.xyspeed: %f\n", wolfcam_xyspeed, cg.xyspeed);


    if (wc->bobfracsin > 0  &&  !(wc->psbobCycle)) {
        wc->lastvalidBobcycle = wc->bobcycle;
        wc->lastvalidBobfracsin = wc->bobfracsin;
    }
    wc->bobcycle = (wc->psbobCycle & 128) >> 7;
    wc->bobfracsin = fabs( sin( (wc->psbobCycle & 127 ) / 127.0 * M_PI ));

    //Com_Printf ("wolfcam_bobcycle: %d  cg.bobcycle: %d\n", wolfcam_bobcycle, cg.bobcycle);
    //    Com_Printf ("wolfcam_bobfracsin: %f  cg.bobfracsin: %f\n", wolfcam_bobfracsin, cg.bobfracsin);

#if 0
    for (i = 1;  i < WOLFCAM_MAXBACKUPS;  i++) {
        memcpy (&(wolfcam_prevOrigins[i - 1]), &(wolfcam_prevOrigins[i]), sizeof(vec3_t));
        wolfcam_prevTimes[i - 1] = wolfcam_prevTimes[i];
        memcpy (&(wolfcam_prevEs[i - 1]), &(wolfcam_prevEs[i]), sizeof(entityState_t));
    }
    memcpy (&(wolfcam_prevOrigins[WOLFCAM_MAXBACKUPS - 1]), cent->lerpOrigin, sizeof(vec3_t));
    wolfcam_prevTimes[WOLFCAM_MAXBACKUPS - 1] = cg.time;
    memcpy (&wolfcam_esPrev, cs, sizeof(entityState_t));
    memcpy (&(wolfcam_prevEs[WOLFCAM_MAXBACKUPS - 1]), cs, sizeof(entityState_t));
#endif

    memcpy (os, cs, sizeof(entityState_t));

#if 0
    /////////////////////  sldkfjsldkfj
    if ((cs->eFlags & EF_TELEPORT_BIT) != (wolfcam_prevEs[WOLFCAM_MAXBACKUPS - 1].eFlags & EF_TELEPORT_BIT)) {
        Com_Printf ("^3teleport\n");
    }
#endif
}
