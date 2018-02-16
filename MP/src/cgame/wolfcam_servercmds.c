#include "cg_local.h"
#include "wolfcam_local.h"

qboolean Wolfcam_ServerCommand (void)
{

	return qfalse;
}

void Wolfcam_ScoreData (void)
{
    score_t *sc;
    wclient_t *wc;
    clientInfo_t *ci;
    int i;

    for (i = 0;  i < cg.numScores;  i++) {
        sc = &cg.scores[i];
        wc = &wclients[sc->client];
        if (!wc->currentValid)
            continue;
        ci = &cgs.clientinfo[sc->client];
        wc->ping = sc->ping;
        wc->pingAccum += sc->ping;
        wc->pingSamples++;
        wc->time = sc->time;
        wc->score = sc->score;
    }
}


