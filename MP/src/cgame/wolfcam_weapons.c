#include "cg_local.h"
#include "wolfcam_local.h"


extern vec3_t  ejectBrassCasingOrigin;

extern qboolean CG_GetPartFramesFromWeap(centity_t *cent, refEntity_t *part, refEntity_t *parent, int partid, weaponInfo_t *wi);
extern void CG_RunWeapLerpFrame( clientInfo_t *ci, weaponInfo_t *wi, lerpFrame_t *lf,int newAnimation, float speedScale );
extern float CG_VenomSpinAngle( centity_t *cent );
extern void CG_FlamethrowerFlame( centity_t *cent, vec3_t origin );
extern void CG_PlayerTeslaCoilFire( centity_t *cent, vec3_t flashorigin );

void wolfcam_check_headshot (int sourceClientNum, int hitClientNum, vec3_t endPos)
{
	//orientation_t heador;
	orientation_t or;
	centity_t *targ;
	entityState_t *tes;

	targ = &cg_entities[hitClientNum];
	tes = &targ->currentState;

	if (cg_entities[hitClientNum].currentState.eFlags & EF_DEAD)
		return;
#if 0
	CG_Printf ("%d -> %d  %f %f %f\n", sourceClientNum, hitClientNum,
			   endPos[0],
			   endPos[1],
			   endPos[2]
			   );
#endif

#if 0
	CG_Printf ("%d -> %d   %s   ^7->   %s\n", sourceClientNum, hitClientNum,
			   cgs.clientinfo[sourceClientNum].name, cgs.clientinfo[hitClientNum].name);
#endif

	if (0) {  //(CG_GetTag(hitClientNum, "tag_head", &or)) {
		//CG_Printf ("got head\n");
		
	} else {
		float height, dest;
		vec3_t v, angles, forward, up, right;

		//CG_Printf ("^1couldn't get head %d %s\n", hitClientNum, cgs.clientinfo[hitClientNum].name);
		
		VectorCopy (targ->currentState.pos.trBase, or.origin);

		if (tes->eFlags & EF_CROUCHING)
			height = CROUCH_VIEWHEIGHT - 12;
		else
			height = DEFAULT_VIEWHEIGHT;
		//return;

		VectorCopy (tes->apos.trBase, angles);
		if (angles[PITCH] > 180) {
			dest = (-360 + angles[PITCH]) * 0.75;
		} else {
			dest = angles[PITCH] * 0.75;
		}
		angles[PITCH] = dest;

		AngleVectors (angles, forward, right, up);
		VectorScale (forward, 5, v);
		VectorMA (v, 18, up, v);

		VectorAdd (v, or.origin, or.origin);
		or.origin[2] += height / 2;
	}

	//CG_GetTag (hitClientNum, "tag_head", &heador);

	//CG_Printf ("ghd : %f %f %f\n", heador.origin[0], heador.origin[1], heador.origin[2]);

#if 0
	CG_Printf ("head: %f %f %f\n", or.origin[0], or.origin[1], or.origin[2]);
	CG_Printf ("epos: %f %f %f\n", endPos[0], endPos[1], endPos[2]);
	CG_Printf ("      %f %f %f\n", endPos[0] - or.origin[0],
			   endPos[1] - or.origin[1],
			   endPos[2] - or.origin[2]
			   );
#endif
	// r.mins, -6, -6, -2
	// r.maxs,  6,  6,  10

	//FIXME use direction of motion

#define MAXY (6.0 + 8.0)
#define MINX (6.0 + 8.0)

#define MAXX (6.0 + 8.0)
#define MINY (6.0 + 8.0)

#define MAXZ (10.0 + 1.0)
#define MINZ (2.0 + 1.0)

	if (endPos[0] >= (or.origin[0] - MINX)  &&  endPos[0] <= (or.origin[0] + MAXX)) {
		//CG_Printf ("x\n");
		if (endPos[1] >= (or.origin[1] - MINY)  &&  endPos[1] <= (or.origin[1] + MAXY)) {
			//CG_Printf ("y\n");
			if (endPos[2] >= (or.origin[2] - MINZ)  &&  endPos[2] <= (or.origin[2] + MAXZ)) {
				//CG_Printf ("^2HEADSHOT\n");
				//FIXME mg42
				wclients[sourceClientNum].wstats[cg_entities[sourceClientNum].currentState.weapon].headshots++;
				CG_Printf("%d -> %d   %s   ^7->   %s\n", sourceClientNum, hitClientNum,
					cgs.clientinfo[sourceClientNum].name, cgs.clientinfo[hitClientNum].name);

			}
		}
	}
		
}

#if 0
qboolean Wolfcam_isHeadShot (vec3_t end, int sourceEntityNum, vec3_t normal, int fleshEntityNum, int otherEntNum2)
{
    sfxHandle_t head;
    orientation_t or;
    trace_t tr;
    centity_t *cent;
    entityState_t *es;
    vec3_t torigin, tangles;
    vec3_t point;

    //    if (!IsHeadShotWeapon(...))  return qfalse;  // assuming is hs weapon
    //FIXME wolfcam mg42

    cent = &cg_entities[fleshEntityNum];
    es = &cent->currentState;

    VectorCopy (end, point);

    //FIXME wolfcam lerp ?
    VectorCopy (es->pos.trBase, torigin);
    VectorCopy (es->apos.trBase, tangles);

    head = G_Spawn ();

    if (CG_GetTag (fleshEntityNum, "tag_head", &or)) {
        G_SetOrigin (head, or.origin);
    } else {
        float height, dest;
        vec3_t v, angles, forward, up, right;

        G_SetOrigin (head, torigin);

        if (es->eFlags & EF_CROUCHING)  // closer fake offset for 'head' box when crouching
            height = CROUCH_VIEWHEIGHT - 12;
        else
            height = DEFAULT_VIEWHEIGHT;

        // NERVE - SMF - this matches more closely with WolfMP models
        VectorCopy (tangles, angles);
        if ( angles[PITCH] > 180 ) {
            dest = (-360 + angles[PITCH]) * 0.75;
        } else {
            dest = angles[PITCH] * 0.75;
        }
        angles[PITCH] = dest;

        AngleVectors( angles, forward, right, up );
        VectorScale( forward, 5, v );
        VectorMA( v, 18, up, v );

        VectorAdd (v, head->r.currentOrigin, head->r.currentOrigin);

        head->r.currentOrigin[2] += height / 2;
    }

    VectorCopy (head->r.currentOrigin, head->s.origin);
    VectorCopy (targ->r.currentAngles, head->s.angles); 
    VectorCopy (head->s.angles, head->s.apos.trBase);
    VectorCopy (head->s.angles, head->s.apos.trDelta);
    VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
    VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6
    head->clipmask = CONTENTS_SOLID;
    head->r.contents = CONTENTS_SOLID;

    trap_LinkEntity (head);
        
    // trace another shot see if we hit the head
    VectorCopy (point, start);
    VectorMA (start, 64, dir, end);
    //trap_Trace (&tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT);
    CG_Trace (&tr, start, NULL, NULL, point, -1, MASK_SHOT);

    traceEnt = &g_entites[ tr.entityNum];

    G_FreeEntity (head);

    if (traceEnt == head) {
        return qtrue;
    }

    return qfalse;
}
#endif
        // -NERVE - SMF
#if 0  //FIXME wolfcam
static void Wolfcam_AddPlayerFoot ( refEntity_t *parent, centity_t *cent )
{
	refEntity_t		wolfkick;
	vec3_t			kickangle;
	weaponInfo_t	*weapon;
	weapon_t		weaponNum;
	int				frame;
	static int		oldtime = 0;

	if (!(cg.snap->ps.persistant[PERS_WOLFKICK]))
	{
		oldtime = 0;
		return;
	}

	weaponNum = cent->currentState.weapon;
	weapon = &cg_weapons[weaponNum];

	memset (&wolfkick, 0, sizeof (wolfkick));
	
	VectorCopy (parent->lightingOrigin, wolfkick.lightingOrigin);
	wolfkick.shadowPlane = parent->shadowPlane;
	
	// note to self we want this to lerp and advance frame
	wolfkick.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON;; 
	wolfkick.hModel = wolfkickModel;

	VectorCopy( cg.refdef.vieworg, wolfkick.origin );
	//----(SA)	allow offsets for testing boot model
	if(cg_gun_x.value)	VectorMA( wolfkick.origin, cg_gun_x.value,	cg.refdef.viewaxis[0], wolfkick.origin );
	if(cg_gun_y.value)	VectorMA( wolfkick.origin, cg_gun_y.value,	cg.refdef.viewaxis[1], wolfkick.origin );
	if(cg_gun_z.value)	VectorMA( wolfkick.origin, cg_gun_z.value,	cg.refdef.viewaxis[2], wolfkick.origin );
	//----(SA)	end


	VectorCopy(cg.refdefViewAngles, kickangle);
	if(kickangle[0]<0)	kickangle[0] = 0;		//----(SA)	avoid "Rockette" syndrome :)
	AnglesToAxis (kickangle, wolfkick.axis);

	
	frame = cg.snap->ps.persistant[PERS_WOLFKICK];

//	CG_Printf("frame: %d\n", frame);
		
	wolfkick.frame = frame;
	wolfkick.oldframe = frame - 1;
	wolfkick.backlerp = 1 - cg.frameInterpolation;
	trap_R_AddRefEntityToScene( &wolfkick );
}
#endif
/*
========================
CG_AddWeaponWithPowerups
========================
*/
static void Wolfcam_AddWeaponWithPowerups( refEntity_t *gun, int powerups, playerState_t *ps, centity_t *cent ) {

	// add powerup effects
	// DHM - Nerve :: no powerup effects on weapons
	trap_R_AddRefEntityToScene( gun );
}

/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/
static qboolean debuggingweapon = qfalse;

static void Wolfcam_AddPlayerWeapon( refEntity_t *parent, centity_t *cent) {

	refEntity_t	gun;
	refEntity_t	barrel;
	refEntity_t	flash;
	vec3_t		angles;
	weapon_t	weaponNum;
	weaponInfo_t	*weapon;
	centity_t	*nonPredictedCent;
	qboolean	firing;	// Ridah

	qboolean	playerScaled;
	qboolean	drawpart;
	int			i;
	qboolean	isPlayer;
    clientInfo_t *ci;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    es = &cent->currentState;

	// (SA) might as well have this check consistant throughout the routine
	//isPlayer = (qboolean)(cent->currentState.clientNum == cg.snap->ps.clientNum);
    isPlayer = qtrue;

	weaponNum = cent->currentState.weapon;

	if( cg.cameraMode) {
		return;
	}

	// don't draw any weapons when the binocs are up
	if(cent->currentState.eFlags & EF_ZOOMING) {
        return;
	}

	// don't draw weapon stuff when looking through a scope
	if(weaponNum == WP_SNOOPERSCOPE || weaponNum == WP_SNIPERRIFLE) {
        //		if(isPlayer && !cg.renderingThirdPerson)  //FIXME wolfcam-rtcw third person
			return;
	}

	// no weapon when on mg_42
	if(cent->currentState.eFlags & EF_MG42_ACTIVE) {
		// Arnout: MG42 Muzzle Flash
		if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME ) {
			CG_MG42EFX( cent );
		}
		return;
	}

	CG_RegisterWeapon( weaponNum );
	weapon = &cg_weapons[weaponNum];

	// add the weapon
	memset( &gun, 0, sizeof( gun ) );
	VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
	gun.shadowPlane = parent->shadowPlane;
	gun.renderfx = parent->renderfx;
	
    if (wolfcam_debugDrawGun.integer)
        gun.renderfx = RF_FIRST_PERSON;

	// set custom shading for railgun refire rate
	if (1) {  //( ps ) {
		gun.shaderRGBA[0] = 255;
		gun.shaderRGBA[1] = 255;
		gun.shaderRGBA[2] = 255;
		gun.shaderRGBA[3] = 255;
	}

	if (1) {  //(ps) {
		gun.hModel = weapon->weaponModel[W_FP_MODEL];
		if ( ci->team == TEAM_RED ) {
			if ( weaponNum != WP_MP40 && weaponNum != WP_THOMPSON ) {
				if ( weapon->weaponModel[W_FP_MODEL_SWAP] )
					gun.hModel = weapon->weaponModel[W_FP_MODEL_SWAP];
			}
		}
	} else {
	}

	if (!gun.hModel) {
		if(debuggingweapon) CG_Printf("returning due to: !gun.hModel\n");
		return;
	}

#if 0  //FIXME wolfcam on ladder
	if(!ps && cg.snap->ps.pm_flags & PMF_LADDER && isPlayer)		//----(SA) player on ladder
	{
		if(debuggingweapon) CG_Printf("returning due to: !ps && cg.snap->ps.pm_flags & PMF_LADDER\n");
		return;
	}
#endif

	// Ridah
	firing = ((cent->currentState.eFlags & EF_FIRING) != 0);

	CG_PositionEntityOnTag( &gun, parent, "tag_weapon", 0, NULL);

	playerScaled = (qboolean)(cgs.clientinfo[ cent->currentState.clientNum ].playermodelScale[0] != 0);

	if (1) {  //(ps) {
		drawpart = CG_GetPartFramesFromWeap(cent, &gun, parent, W_MAX_PARTS, weapon);	// W_MAX_PARTS specifies this as the primary view model
	} else {
		drawpart = qtrue;
	}

	if(drawpart)
		Wolfcam_AddWeaponWithPowerups( &gun, cent->currentState.powerups, NULL, cent );

	if (isPlayer)
	{
		refEntity_t	brass;

		CG_PositionRotatedEntityOnTag( &brass, &gun, "tag_brass" );
		VectorCopy (brass.origin, ejectBrassCasingOrigin);
	}

	memset( &barrel, 0, sizeof( barrel ) );
	VectorCopy( parent->lightingOrigin, barrel.lightingOrigin );
	barrel.shadowPlane = parent->shadowPlane;
	barrel.renderfx = parent->renderfx;

	// add barrels
	// attach generic weapon parts to the first person weapon.
	// if a barrel should be attached for third person, add it in the (!ps) section below
	angles[YAW] = angles[PITCH] = 0;

	if (1) {  //(ps) {
		qboolean spunpart;

		for(i=W_PART_1; i<W_MAX_PARTS; i++) {

			spunpart = qfalse;
			barrel.hModel = weapon->partModels[W_FP_MODEL][i];

			// check for spinning
			if(weaponNum == WP_VENOM) {
				if( i == W_PART_1 ) {
					angles[ROLL] = CG_VenomSpinAngle( cent );
					spunpart = qtrue;
				}
				else if( i == W_PART_2 ) {
					angles[ROLL] = -CG_VenomSpinAngle( cent );
					spunpart = qtrue;
				}
				// 'blurry' barel when firing

				// (SA) not right now.  at the moment, just spin the belt when firing, no swapout
				else if( i == W_PART_3 ) {
					if((cent->pe.weap.animationNumber & ~ANIM_TOGGLEBIT) == WEAP_ATTACK1) {
						barrel.hModel = weapon->partModels[W_FP_MODEL][i];
						angles[ROLL] = -CG_VenomSpinAngle( cent );
						angles[ROLL] = -(angles[ROLL]/8.0f);
					} else {
						angles[ROLL] = 0;
					}
					spunpart = qtrue;
				}
			} else if (weaponNum == WP_MP40 || weaponNum == WP_THOMPSON) {
				if( i == W_PART_3 ) {
					if ( ci->team == TEAM_RED ) {
						if ( weapon->weaponModel[W_FP_MODEL_SWAP] )
							barrel.hModel = weapon->weaponModel[W_FP_MODEL_SWAP];
					}
				}
			}

			if(spunpart) {
				AnglesToAxis( angles, barrel.axis );
			}
			// end spinning


			if(barrel.hModel) {
				if(i==W_PART_1) {
					if(spunpart)	CG_PositionRotatedEntityOnTag( &barrel, parent, "tag_barrel");
					else			CG_PositionEntityOnTag( &barrel, parent, "tag_barrel", 0, NULL);
				} else {
					if(spunpart)	CG_PositionRotatedEntityOnTag( &barrel, parent, va("tag_barrel%d", i+1));
					else			CG_PositionEntityOnTag( &barrel, parent, va("tag_barrel%d", i+1), 0, NULL);
				}

				drawpart = CG_GetPartFramesFromWeap(cent, &barrel, parent, i, weapon);
					
				if(drawpart)
					Wolfcam_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, NULL, cent );
			}
		}
	} else {	// weapons with barrels drawn in third person
	}

	// add the scope model to the rifle if you've got it
	if (1) {  //(isPlayer && !cg.renderingThirdPerson) {		// (SA) for now just do it on the first person weapons  //FIXME wolfcam thirdperson
        //FIXME wolfcam with scope????
		if(weaponNum == WP_MAUSER) {
			if ( COM_BitCheck( cg.predictedPlayerState.weapons, WP_SNIPERRIFLE ) ) {
				barrel.hModel = weapon->modModel[0];
				if(barrel.hModel) {
					CG_PositionEntityOnTag( &barrel, &gun, "tag_scope", 0, NULL);
					Wolfcam_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, NULL, cent );
				}
			}
		} else if(weaponNum == WP_GARAND) {
			if ( COM_BitCheck( cg.predictedPlayerState.weapons, WP_SNOOPERSCOPE ) ) {
				barrel.hModel = weapon->modModel[0];
				if(barrel.hModel) {
					CG_PositionEntityOnTag( &barrel, &gun, "tag_scope", 0, NULL);
					Wolfcam_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, NULL, cent );
				}
			}
		}
	}

    //FIXME wolfcam don't understand about nonPredictedCent, ignored later

	// make sure we aren't looking at cg.predictedPlayerEntity for LG
	nonPredictedCent = &cg_entities[cent->currentState.clientNum];

	// if the index of the nonPredictedCent is not the same as the clientNum
	// then this is a fake player (like on the single player podiums), so
	// go ahead and use the cent
	if( ( nonPredictedCent - cg_entities ) != cent->currentState.clientNum ) {
		nonPredictedCent = cent;
	}


	// add the flash
	memset( &flash, 0, sizeof( flash ) );
	VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
	flash.shadowPlane = parent->shadowPlane;
	flash.renderfx = parent->renderfx;

	if (1) //(ps)
		flash.hModel = weapon->flashModel[W_FP_MODEL];
	else
		flash.hModel = weapon->flashModel[W_TP_MODEL];

	angles[YAW]		= 0;
	angles[PITCH]	= 0;
	angles[ROLL]	= crandom() * 10;
	AnglesToAxis( angles, flash.axis );

	if (0) {  //(ps && ps->weapon == WP_AKIMBO) {  //FIXME wolfcam hmm akimbo
#if 0  // wolfcam
		if(BG_AkimboFireSequence(ps))
			CG_PositionRotatedEntityOnTag( &flash, &gun, "tag_flash");
		else
			CG_PositionRotatedEntityOnTag( &flash, &gun, "tag_flash2");
#endif
	} else {
		CG_PositionRotatedEntityOnTag( &flash, &gun, "tag_flash");
	}

	// store this position for other cgame elements to access
	//cent->pe.gunRefEnt = gun;
	//cent->pe.gunRefEntFrame = cg.clientFrame;

	if ( ( weaponNum == WP_FLAMETHROWER || weaponNum == WP_TESLA) && ( nonPredictedCent->currentState.eFlags & EF_FIRING) ) 
	{
		// continuous flash

	} else {

		// continuous smoke after firing
#define BARREL_SMOKE_TIME 1000

		if (1) {  //( ps || cg.renderingThirdPerson || !isPlayer ) {  //FIXME wolfcam check
			if(weaponNum == WP_VENOM || weaponNum == WP_STEN) {
				// hot smoking gun
				if(cg.time - cent->overheatTime < 3000) {
					if(!(rand()%3)) {
						float alpha;
						alpha = 1.0f - ((float)(cg.time - cent->overheatTime)/3000.0f);
						alpha *= 0.25f;		// .25 max alpha
						if(weaponNum == WP_VENOM)	// silly thing that makes the smoke off the venom swirlier since it's spinning real fast
							CG_ParticleImpactSmokePuffExtended(cgs.media.smokeParticleShader, flash.origin, 1000, 8, 20, 70, alpha);
						else
							CG_ParticleImpactSmokePuffExtended(cgs.media.smokeParticleShader, flash.origin, 1000, 8, 20, 30, alpha);
					}
				}

			} else if(weaponNum == WP_VENOM_FULL || weaponNum == WP_PANZERFAUST || weaponNum == WP_ROCKET_LAUNCHER) {
				if (cg.time - cent->muzzleFlashTime < BARREL_SMOKE_TIME) {
					if(!(rand()%5)) {
						float alpha;
						alpha = 1.0f - ((float)(cg.time - cent->muzzleFlashTime)/(float)BARREL_SMOKE_TIME);	// what fraction of BARREL_SMOKE_TIME are we at
						alpha *= 0.25f;		// .25 max alpha
						CG_ParticleImpactSmokePuffExtended(cgs.media.smokeParticleShader, flash.origin, 1000, 8, 20, 30, alpha);
					}
				}
			}
		}

    if (wolfcam_debugDrawGun.integer) {
        qhandle_t oldflashmodel = flash.hModel;

        flash.hModel = cg_weapons[WP_MP40].flashModel[W_FP_MODEL];
        
        trap_R_AddRefEntityToScene( &flash );
        flash.hModel = oldflashmodel;
        trap_R_AddLightToScene( flash.origin, 200 + (rand()&31), cg_weapons[WP_MP40].flashDlightColor[0] + 100, cg_weapons[WP_MP40].flashDlightColor[1] + 100, cg_weapons[WP_MP40].flashDlightColor[2] + 100, 0 );
        //CG_Printf ("debugging drawGun\n");
    }

		// impulse flash
		if ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME ) {
			// Ridah, blue ignition flame if not firing flamer
			if ( weaponNum != WP_FLAMETHROWER && weaponNum != WP_TESLA) {
				return;
			}
		}

	}


	// weapons that don't need to go any further as they have no flash or light
	if(	weaponNum == WP_GRENADE_LAUNCHER || 
		weaponNum == WP_GRENADE_PINEAPPLE ||
		weaponNum == WP_SPEARGUN ||
		weaponNum == WP_SPEARGUN_CO2 ||
		weaponNum == WP_KNIFE ||
		weaponNum == WP_KNIFE2 ||
		weaponNum == WP_CROSS ||
		weaponNum == WP_DYNAMITE ||
		weaponNum == WP_DYNAMITE2 )
	{
		return;
	}

	if(weaponNum == WP_STEN) {	// sten has no muzzleflash
		flash.hModel = 0;
	}

	// weaps with barrel smoke
	if (1) {  //( ps || cg.renderingThirdPerson || !isPlayer ) {
		if(weaponNum == WP_STEN || weaponNum == WP_VENOM ) {
			if ( cg.time - cent->muzzleFlashTime < 100 )
//				CG_ParticleImpactSmokePuff (cgs.media.smokeParticleShader, flash.origin);
				CG_ParticleImpactSmokePuffExtended(cgs.media.smokeParticleShader, flash.origin, 500, 8, 20, 30, 0.25f);
		}
	}


	if(flash.hModel) {
		if(weaponNum != WP_FLAMETHROWER && weaponNum != WP_TESLA ) {	//Ridah, hide the flash also for now
			// RF, changed this so the muzzle flash stays onscreen for long enough to be seen
			if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME ) {
			//if (firing) {	// Ridah
				trap_R_AddRefEntityToScene( &flash );
			}
		}
	}

	// Ridah, zombie fires from his head
	//if (CG_MonsterUsingWeapon( cent, AICHAR_ZOMBIE, WP_MONSTER_ATTACK1 )) {
	//	CG_PositionEntityOnTag( &flash, parent, parent->hModel, "tag_head", NULL);
	//}

	if (1) {  //( ps || cg.renderingThirdPerson || !isPlayer ) {

		if (firing) {
			// Ridah, Flamethrower effect
			CG_FlamethrowerFlame( cent, flash.origin );

			// RF, Tesla coil
			CG_PlayerTeslaCoilFire( cent, flash.origin );

			// make a dlight for the flash
			if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] ) {
				trap_R_AddLightToScene( flash.origin, 200 + (rand()&31), weapon->flashDlightColor[0],
					weapon->flashDlightColor[1], weapon->flashDlightColor[2], 0 );
			}
		} else {
			if ( weaponNum == WP_FLAMETHROWER ) {
				vec3_t angles;
				AxisToAngles( flash.axis, angles );
// JPW NERVE
				weaponNum = BG_FindAmmoForWeapon(WP_FLAMETHROWER);
				if (1) {  //(ps) {
					if (1)  //(ps->ammoclip[weaponNum])  //FIXME wolfcam
						CG_FireFlameChunks( cent, flash.origin, angles, 1.0, qfalse );
				}
				else {
					CG_FireFlameChunks( cent, flash.origin, angles, 1.0, qfalse );
				}
// jpw
			}
		}
	}
}

static int Wolfcam_IdleAnimForWeapon ( int weapon ) {
    
    return WEAP_IDLE1;
#if 0
    switch( weapon ) {
    case WP_GPG40:
    case WP_M7:
    case WP_SATCHEL_DET:
    case WP_MORTAR_SET:
    case WP_MEDIC_ADRENALINE:
    case WP_MOBILE_MG42_SET:
        return WEAP_IDLE2;

    default:
        return WEAP_IDLE1;
    }
#endif
}

static int Wolfcam_AttackAnimForWeapon ( int weapon ) {
    return WEAP_ATTACK1;
}

static int Wolfcam_ReloadAnimForWeapon ( int weapon ) {
    return WEAP_RELOAD1;
}

static int Wolfcam_RaiseAnimForWeapon ( int weapon ) {
    return WEAP_RAISE;
}

static int Wolfcam_DropAnimForWeapon ( int weapon ) {
    return WEAP_DROP;
}

static int Wolfcam_GetWeaponAnim (void)
{
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;
    wclient_t *wc;
    int eflags;
    int rtime;
    //int anim;
    int newweapon;
    int oldweapon;

    //return WEAP_ATTACK1;  //FIXME wolfcam testing

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;
    eflags = es->eFlags;
    wc = &wclients[wcg.clientNum];

    newweapon = es->weapon;
    oldweapon = wc->event_oldWeapon;

    if (cg.time - wc->event_weaponSwitchTime < 250)
        return Wolfcam_DropAnimForWeapon(oldweapon);
    else if (cg.time - wc->event_weaponSwitchTime < 500)
        return Wolfcam_RaiseAnimForWeapon(newweapon);

    rtime = GetAmmoTableData(es->weapon)->reloadTime;
    if (cg.time - wc->event_reloadTime < rtime) {
        return Wolfcam_ReloadAnimForWeapon(es->weapon);
    }

    // delayed weapon anims
    //FIXME grenade anim after weapon fire
    switch (es->weapon) {  //FIXME packs
    case WP_PANZERFAUST:
        if (cg.time - wc->event_fireWeaponTime < 2000)  //FIXME wolfcam check panzer time bg_pmove.c
            return Wolfcam_AttackAnimForWeapon(es->weapon);
        else
            return Wolfcam_IdleAnimForWeapon(es->weapon);
        break;
    case WP_GRENADE_LAUNCHER:
    case WP_GRENADE_PINEAPPLE:
        //FIXME wolfcam find numbers wolfcam_event_fireWeaponTime
        if (eflags & EF_FIRING  ||  cg.time - wc->event_fireWeaponTime < 2000)  //FIXME wolfcam check bg_pmove.c
            return Wolfcam_AttackAnimForWeapon(es->weapon);
        break;
    default:
        break;
    }

    if (cg.time - wc->event_fireWeaponTime < 200)  //FIXME wolfcam
        return Wolfcam_AttackAnimForWeapon(es->weapon);

    return Wolfcam_IdleAnimForWeapon(es->weapon);
}


/*
==============
CG_WeaponAnimation
==============
*/

//----(SA)	modified.  this is now client-side only (server does not dictate weapon animation info)
static void Wolfcam_WeaponAnimation(weaponInfo_t *weapon, int *weapOld, int *weap, float *weapBackLerp) {
	clientInfo_t *ci = &cgs.clientinfo[wcg.clientNum];
	centity_t *cent = &cg_entities[ci->clientNum];
    wclient_t *wc = &wclients[wcg.clientNum];


	if ( cg_noPlayerAnims.integer ) {
		*weapOld = *weap = 0;
		return;
	}

    if (wc->weapAnimTimer > 0) {
        wc->weapAnim = wc->weapAnimPrev;
        CG_Printf ("^ewaiting for animation\n");
    } else {
        wc->weapAnim = ANIM_TOGGLEBIT;
        wc->weapAnim = ((wc->weapAnim & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT
                            ) | Wolfcam_GetWeaponAnim();

        //wolfcam_weapAnim = Wolfcam_GetWeaponAnim();
    }


    wc->weapAnimPrev = wc->weapAnim;

	CG_RunWeapLerpFrame( ci, weapon, &cent->pe.weap, wc->weapAnim | ANIM_TOGGLEBIT, 1 );

	*weapOld		= cent->pe.weap.oldFrame;
	*weap			= cent->pe.weap.frame;
	*weapBackLerp	= cent->pe.weap.backlerp;

	if ( cg_debugAnim.integer == 3) {
        CG_Printf( "oldframe: %d   frame: %d   backlerp: %f\n", cent->pe.weap.oldFrame, cent->pe.weap.frame, cent->pe.weap.backlerp);
        //CG_Printf ("animation name: %s\n", cent->pe.weap.animation->name);
	}
}

/*
==============
CG_CalculateWeaponPosition
==============
*/
static void Wolfcam_CalculateWeaponPosition( vec3_t origin, vec3_t angles ) {
	float	scale;
	//int		delta;
	float	fracsin;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;
    wclient_t *wc;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;
    wc = &wclients[wcg.clientNum];

	VectorCopy( cg.refdef.vieworg, origin );
	VectorCopy( cg.refdefViewAngles, angles );

#if 0  // wolfcam
	// adjust 'lean' into weapon
	if(cg.predictedPlayerState.leanf != 0)
	{
		vec3_t	right, up;
		float myfrac = 1.0f;

		if(cg.predictedPlayerState.weapon == WP_GARAND)
			myfrac = 3.0f;
		if(cg.predictedPlayerState.weapon == WP_FLAMETHROWER)
			myfrac = 2.0f;
		if(cg.predictedPlayerState.weapon == WP_TESLA)
			myfrac = 2.0f;
		if(cg.predictedPlayerState.weapon == WP_MAUSER)
			myfrac = 2.0f;

		// reverse the roll on the weapon so it stays relatively level
		angles[ROLL] -= cg.predictedPlayerState.leanf/(myfrac * 2.0f);
		AngleVectors(angles, NULL, right, up);
		VectorMA(origin, angles[ROLL], right, origin);

		// pitch the gun down a bit to show that firing is not allowed when leaning
		angles[PITCH] += (abs(cg.predictedPlayerState.leanf)/2.0f);

		// this gives you some impression that the weapon stays in relatively the same
		// position while you lean, so you appear to 'peek' over the weapon
		AngleVectors(cg.refdefViewAngles, NULL, right, NULL);
		VectorMA(origin, -cg.predictedPlayerState.leanf/4.0f, right, origin);
	}
#endif

	// on odd legs, invert some angles
    if ( wc->bobcycle & 1 ) {
        scale = -wc->xyspeed;
    } else {
        scale = wc->xyspeed;
    }

    //    if (wcg.clientNum == cg.clientNum) {  //FIXME wolfcam
    //    scale = 0;  
    //}

	// gun angles from bobbing
    if (es->groundEntityNum != ENTITYNUM_NONE) {
        angles[ROLL]  += scale             * wc->bobfracsin * 0.005;
        angles[YAW]   += scale             * wc->bobfracsin * 0.01;
        angles[PITCH] += wc->xyspeed   * wc->bobfracsin * 0.005;
    }


	// drop the weapon when landing
#if 0  //FIXME wolfcam
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		origin[2] += cg.landChange*0.25 * delta / LAND_DEFLECT_TIME;
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		origin[2] += cg.landChange*0.25 * 
			(LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
	}
#endif 


	// idle drift
//----(SA) adjustment for MAX KAUFMAN
//	scale = cg.xyspeed + 40;
	scale = 80;
//----(SA)	end
	fracsin = sin( cg.time * 0.001 );
	angles[ROLL] += scale * fracsin * 0.01;
	angles[YAW] += scale * fracsin * 0.01;
	angles[PITCH] += scale * fracsin * 0.01;

	// RF, subtract the kickAngles
	//VectorMA( angles, -1.0, cg.kickAngles, angles );  //FIXME wolfcam cg.kickAngles

}

// Ridah
/*
===============
CG_FlamethrowerFlame
===============
*/
static void Wolfcam_FlamethrowerFlame( centity_t *cent, vec3_t origin ) {

	if (cent->currentState.weapon != WP_FLAMETHROWER) {
		return;
	}

	CG_FireFlameChunks( cent, origin, cent->lerpAngles, 1.0, qtrue );  // wolfcam seems ok
}
// done.

/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void Wolfcam_AddViewWeapon (void)
{
	refEntity_t	hand;
	float		fovOffset;
	vec3_t		angles;
	vec3_t		gunoff;
	weaponInfo_t	*weapon;
    clientInfo_t *ci;
    centity_t *cent;
    entityState_t *es;

    ci = &cgs.clientinfo[wcg.clientNum];
    cent = &cg_entities[ci->clientNum];
    es = &cent->currentState;

	if ( ci->team == TEAM_SPECTATOR ) {
		return;
	}

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		return;
	}

    if (es->eFlags & EF_DEAD)
        return;

    //if (!cent->currentValid  &&  wcg.clientNum != cg.clientNum)
    if (!wclients[wcg.clientNum].currentValid)
        return;

#if 0  //FIXME wolfcam-rtcw third person
	// no gun if in third person view
	if ( cg.renderingThirdPerson ) {
		return;
	}
#endif

	// allow the gun to be completely removed
	if (( !cg_drawGun.integer ) || (cg_uselessNostalgia.integer)) {
		vec3_t		origin;

		if ( es->eFlags & EF_FIRING ) {
			// special hack for flamethrower...
			VectorCopy( cg.refdef.vieworg, origin );

			VectorMA( origin, 18, cg.refdef.viewaxis[0], origin );
			VectorMA( origin, -7, cg.refdef.viewaxis[1], origin );
			VectorMA( origin, -4, cg.refdef.viewaxis[2], origin );

			// Ridah, Flamethrower effect
			Wolfcam_FlamethrowerFlame( cent, origin );
		}
		return;
	}

	// don't draw if testing a gun model
	if ( cg.testGun ) {
		return;
	}

	if (es->eFlags & EF_MG42_ACTIVE)
		return;

	// drop gun lower at higher fov
	if ( cg_fov.integer > 90 ) {
		fovOffset = -0.2 * ( cg_fov.integer - 90 );
	} else {
		fovOffset = 0;
	}

	if ( es->weapon > WP_NONE) {
		CG_RegisterWeapon( es->weapon );  // wolfcam seems ok
		weapon = &cg_weapons[ es->weapon ];

		memset (&hand, 0, sizeof(hand));

		// set up gun position
		Wolfcam_CalculateWeaponPosition( hand.origin, angles );

		gunoff[0] = cg_gun_x.value;
		gunoff[1] = cg_gun_y.value;
		gunoff[2] = cg_gun_z.value;

//----(SA)	removed

		VectorMA( hand.origin, gunoff[0], cg.refdef.viewaxis[0], hand.origin );
		VectorMA( hand.origin, gunoff[1], cg.refdef.viewaxis[1], hand.origin );
		VectorMA( hand.origin, (gunoff[2]+fovOffset), cg.refdef.viewaxis[2], hand.origin );

		AnglesToAxis( angles, hand.axis );

		if ( cg_gun_frame.integer ) {  //FIXME wolfcam don't know what this is about
			hand.frame = hand.oldframe = cg_gun_frame.integer;
			hand.backlerp = 0;
            CG_Printf ("sssssssssssssssssss\n");
		}
		else {	// get the animation state
			Wolfcam_WeaponAnimation( weapon, &hand.oldframe, &hand.frame, &hand.backlerp);	//----(SA)	changed  
		}


		hand.hModel = weapon->handsModel;
		hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT;	//----(SA)	

		// add everything onto the hand
        Wolfcam_AddPlayerWeapon (&hand, cent);
		// Ridah

	}	// end 	"if ( ps->weapon > WP_NONE)"

	// Rafael
	// add the foot
	//Wolfcam_AddPlayerFoot (&hand, &cg.predictedPlayerEntity);  //FIXME wolfcam rewrite
	
    //    cg.predictedPlayerEntity.lastWeaponClientFrame = cg.clientFrame;  //FIXME wolfcam need?
}
