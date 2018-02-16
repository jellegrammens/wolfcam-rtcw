#include "cg_local.h"
#include "wolfcam_local.h"

static void Wolfcam_CheckWarp (void)
{
    int clientNum;
    int i;
    snapshot_t *oldsnap;

    if (wcg.curSnapshotNumber < 2) 
        return;

    //return;
    //FIXME wolfcam this is where you do player transitions, although no lerp yet
    oldsnap = &wcg.snaps[(wcg.curSnapshotNumber - 2) & MAX_SNAPSHOT_MASK];

    // check for warping
    for (clientNum = 0;  clientNum < MAX_CLIENTS;  clientNum++) {
        clientInfo_t *ci;
        centity_t *cent;
        entityState_t *es;
        entityState_t *oldes;  // = NULL;
        vec3_t oldPosition;
        vec3_t newPosition;
        float xv;
        float yv;
        float xyspeed;
        int timeDiff;
        int oldGroundEntityNum;
        qboolean foundOldClient;

        ci = &cgs.clientinfo[clientNum];
        cent = &cg_entities[clientNum];
        es = &cent->currentState;

        if (!ci->infoValid  ||  !cent->currentValid) {
            if (clientNum != cg.snap->ps.clientNum)
                continue;
        }

        if (es->groundEntityNum == -1)
            continue;  // in air, don't check

        foundOldClient = qfalse;

        // find in old snapshot
        if (oldsnap->ps.clientNum == clientNum) {
            VectorCopy (oldsnap->ps.origin, oldPosition);
            SnapVector (oldPosition);
            oldGroundEntityNum = oldsnap->ps.groundEntityNum;
            foundOldClient = qtrue;
        } else {
            for (i = 0;  i < oldsnap->numEntities;  i++) {
                oldes = &oldsnap->entities[i];
                if (oldes->number == clientNum) {
                    VectorCopy (oldes->pos.trBase, oldPosition);
                    oldGroundEntityNum = oldes->groundEntityNum;
                    foundOldClient = qtrue;
                    break;
                }
            }
        }

        if (!foundOldClient)
            continue;

        if (oldGroundEntityNum == -1)  //(oldes->groundEntityNum == -1)
            continue;

        // have old client, do the calculation
        if (clientNum == cg.snap->ps.clientNum) {
            VectorCopy (cg.snap->ps.origin, newPosition);
            SnapVector (newPosition);
        } else
            VectorCopy (es->pos.trBase, newPosition);

        if (oldPosition[2] != newPosition[2])
            continue;

#if 0
        CG_Printf ("%02d (%f, %f, %f)  (%f, %f, %f)   %d  %s\n",
                   clientNum,
                   oldPosition[0], oldPosition[1], oldPosition[2],
                   newPosition[0], newPosition[1], newPosition[2],
                   cg.snap->serverTime - oldsnap->serverTime,
                   cgs.clientinfo[clientNum].name
                   );
#endif
        xv = (newPosition[0] - oldPosition[0]);   // *   1000.0 / (float)(cg.snap->serverTime - oldsnap->serverTime);  // / 50.0;  // 50 ms
        yv = (newPosition[1] - oldPosition[1]);  //  *   1000.0 / (float)(cg.snap->serverTime - oldsnap->serverTime);  // / 50.0;

        // snap the vectors in case positions are from playerState
        xv = (int)xv;
        yv = (int)yv;

        xyspeed = sqrt (xv * xv  +  yv * yv);

        timeDiff = cg.snap->serverTime - oldsnap->serverTime;
        xyspeed *= (1000.0 / (float)timeDiff);
		wclients[clientNum].xyspeed = xyspeed;
        
        //CG_Printf ("%s: xv:%f  yv:%f  xyspeed:%f\n", cgs.clientinfo[clientNum].name, xv, yv, xyspeed);

		
        //FIXME take into account snapped vectors and give the benifit of the doubt
        // assume max error 4
        if (xyspeed > 352.0 + 40.0) {
			wclients[clientNum].warpTime = cg.time;
            wclients[clientNum].warp++;
            //wclients[clientNum].warpAccum = (xyspeed + wclients[clientNum].warpAccum) / wclients[clientNum].warp;
            wclients[clientNum].warpAccum += xyspeed;
            //CG_Printf ("%02d   %s  ^7%f\n", clientNum, cgs.clientinfo[clientNum].name, xyspeed);
			if (es->eFlags & EF_FIRING) {
				wclients[clientNum].combatWarp++;
				wclients[clientNum].combatWarpAccum += xyspeed;
			}
#if 0
            if (clientNum == wcg.clientNum)
                CG_Printf ("^3warp %f  ^7%s\n", xyspeed, ci->name);
#endif
        } else if (xyspeed > 352.0 - 40.0) {  // give the the benifit of the doubt
            wclients[clientNum].nowarp++;
			if (es->eFlags & EF_FIRING) {
				wclients[clientNum].combatNowarp++;
			}
        }
    }
}

void Wolfcam_NextSnapShotSet (void)
{
    //return;
	if (wcg.curSnapshotNumber > 1) {
		snapshot_t *oldsnap;

		oldsnap = &wcg.snaps[(wcg.curSnapshotNumber - 2) & MAX_SNAPSHOT_MASK];
		cg.serverFrameTime = cg.snap->serverTime - oldsnap->serverTime;
		//CG_Printf ("%d frame time\n", cg.serverFrameTime);
	}
    Wolfcam_MarkValidEntities ();
    Wolfcam_CheckWarp ();
}

//
