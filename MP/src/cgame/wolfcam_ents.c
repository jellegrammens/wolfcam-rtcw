#include "cg_local.h"
#include "wolfcam_local.h"

void Wolfcam_MarkValidEntities (void)
{
    centity_t *cent;
    int num;
    int i;

    for (i = 0;  i < MAX_CLIENTS;  i++) {
        wclients[i].currentValid = qfalse;
    }

    for (num = 0;  num < cg.snap->numEntities;  num++) {
        cent = &cg_entities[cg.snap->entities[num].number];
        //FIXME ET_CORPSE ?

        //if ((cent->currentState.eType == ET_PLAYER  ||  cent->currentState.eType == ET_CORPSE)  &&
        //     cgs.clientinfo[cent->currentState.clientNum].infoValid) {
        if (cent->currentState.eType == ET_PLAYER) {
            wclients[cent->currentState.clientNum].currentValid = qtrue;
            wclients[cent->currentState.clientNum].validTime = cg.time;
        }
    }

    wclients[cg.snap->ps.clientNum].currentValid = qtrue;
    wclients[cg.snap->ps.clientNum].validTime = cg.time;
}

void Wolfcam_MarkValidEntities_old (void)
{
    int i;

    for (i = 0;  i < MAX_CLIENTS;  i++) {
        if (cgs.clientinfo[i].infoValid  &&  cg_entities[i].currentValid) {
            wclients[i].currentValid = qtrue;
            wclients[i].validTime = cg.time;
        } else {
            wclients[i].currentValid = qfalse;
            wclients[i].invalidTime = cg.time;
        }
    }

    wclients[cg.snap->ps.clientNum].currentValid = qtrue;
    wclients[cg.snap->ps.clientNum].validTime = cg.time;
}
