#include "cg_local.h"
#include "wolfcam_local.h"


/*
===============
Wolfcam_Players_f

print some info about players
===============
*/
void Wolfcam_Players_f (void)
{
    int i;
    clientInfo_t *ci;
    char color;

    for (i = 0;  i < MAX_CLIENTS;  i++) {
        ci = &cgs.clientinfo[i];
        if (!ci->infoValid)
            continue;
        switch (ci->team) {
        case TEAM_AXIS:
            color = '1';
            break;
        case TEAM_ALLIES:
            color = '4';
            break;
        case TEAM_SPECTATOR:
            color = '3';
            break;
        case TEAM_FREE:
            color = '7';
            break;
        default:
            color = '6';
        }
        Com_Printf ("^%c X^7  %02d: %s\n", color, i, ci->name);
    }
}

void Wolfcam_Follow_f (void)
{
    int clientNum;

    if (trap_Argc() < 2) {
        Com_Printf ("currently following [%d]  ", wcg.selectedClientNum);
        if (wcg.selectedClientNum == -1)
            Com_Printf ("^3<demo pov>\n");
        else
            Com_Printf ("%s\n", cgs.clientinfo[wcg.selectedClientNum].name);
        Com_Printf ("^7use 'follow -1' to return to demo taker's pov\n");
        return;
    }

    clientNum = atoi(CG_Argv(1));
    if (clientNum > MAX_CLIENTS  ||  clientNum < -1) {
        Com_Printf ("bad client number\n");
        return;
    }
    if (clientNum == -1) {
        wcg.selectedClientNum = -1;
        wcg.clientNum = -1;
        cg.renderingThirdPerson = 0;
        wolfcam_following = qfalse;
        trap_Cvar_Set ("cg_thirdPerson", "0");  //FIXME wolfcam
        return;
    }
    if (!cgs.clientinfo[clientNum].infoValid) {
        Com_Printf ("invalid client\n");
        return;
    }
    
    wcg.selectedClientNum = clientNum;
    wcg.clientNum = clientNum;
    wolfcam_following = qtrue;

    //memset (&wolfcam_esPrev, 0, sizeof(entityState_t));  //FIXME wolfcam per snap for all clients, if invalid use same snap
}

void Wolfcam_Server_Info_f (void)
{
    const char *info;

    info = CG_ConfigString( CS_SERVERINFO );

    CG_Printf ("^4server config string: ^2%s\n", info);
}

static void print_ind_weap_stats (wweaponStats_t *w)
{
    if (w->kills < 1000)
        CG_Printf (" ");
    if (w->kills < 100)
        CG_Printf (" ");
    if (w->kills < 10)
        CG_Printf (" ");
    CG_Printf ("^2%d", w->kills);

    if (w->deaths < 1000)
        CG_Printf (" ");
    if (w->deaths < 100)
        CG_Printf (" ");
    if (w->deaths < 10)
        CG_Printf (" ");
    CG_Printf ("^1%d", w->deaths);

    CG_Printf ("^2       ");

    if (w->hits < 10000)
        CG_Printf (" ");
    if (w->hits < 1000)
        CG_Printf (" ");
    if (w->hits < 100)
        CG_Printf (" ");
    if (w->hits < 10)
        CG_Printf (" ");
    CG_Printf ("^2%d ^2/ ", w->hits);

    if (w->atts < 10000)
        CG_Printf (" ");
    if (w->atts < 1000)
        CG_Printf (" ");
    if (w->atts < 100)
        CG_Printf (" ");
    if (w->atts < 10)
        CG_Printf (" ");
    CG_Printf ("^2%d", w->atts);
    
    CG_Printf ("    ");

    if (w->atts  &&  w->hits)
        CG_Printf ("^3%0.2f", (float)(w->hits) / (float)(w->atts));
    else {
        //CG_Printf ("0.00");
    }

    if (w->headshots) {
        CG_Printf ("    ");
        CG_Printf ("^5%d", w->headshots);
    }
    
    CG_Printf ("\n");
}

#if 0
static void wolfcam_print_weapon_statsxxx (int clientNum)
{
    wweaponStats_t *w;
    //int weap;

    CG_Printf ("^7Stats for %s^3:\n", cgs.clientinfo[clientNum].name);

    w = &wclients[clientNum].wstats[WP_SNIPERRIFLE];
    if (w->atts) {
        CG_Printf ("^2sniper rifle: ^7%d/%d %d   ^3%0.2f/%0.2f\n", w->hits, w->atts, w->headshots, (float)(w->hits) / (float)(w->atts), (float)(w->headshots) / (float)(w->hits));
    }

    w = &wclients[clientNum].wstats[WP_MP40];
    if (w->atts) {
        CG_Printf ("^2mp40: ^7%d/%d %d   ^3%0.2f/%0.2f\n", w->hits, w->atts, w->headshots, (float)(w->hits) / (float)(w->atts), (float)(w->headshots) / (float)(w->hits));
    }

    w = &wclients[clientNum].wstats[WP_THOMPSON];
    if (w->atts) {
        CG_Printf ("^2thompson: ^7%d/%d %d   ^3%0.2f/%0.2f\n", w->hits, w->atts, w->headshots, (float)(w->hits) / (float)(w->atts), (float)(w->headshots) / (float)(w->hits));
    }

    w = &wclients[clientNum].wstats[WP_STEN];
    if (w->atts) {
        CG_Printf ("^2sten: ^7%d/%d %d    ^3%0.2f/%0.2f\n", w->hits, w->atts, w->headshots, (float)(w->hits) / (float)(w->atts), (float)(w->headshots) / (float)(w->hits));
    }

    w = &wclients[clientNum].wstats[WP_LUGER];
    if (w->atts) {
        CG_Printf ("^2luger: ^7%d/%d %d   ^3%0.2f/%0.2f\n", w->hits, w->atts, w->headshots, (float)(w->hits) / (float)(w->atts), (float)(w->headshots) / (float)(w->hits));
    }

    w = &wclients[clientNum].wstats[WP_COLT];
    if (w->atts) {
        CG_Printf ("^2colt: ^7%d/%d %d   ^3%0.2f/%0.2f\n", w->hits, w->atts, w->headshots, (float)(w->hits) / (float)(w->atts), (float)(w->headshots) / (float)(w->hits));
    }
}
#endif

static void wolfcam_print_weapon_stats (int clientNum, qboolean oldclient)
{
    char *name;
    int kills;
    int deaths;
    int suicides;
    int warp;
    int nowarp;
    int warpAccum;
	int combatWarp;
	int combatNowarp;
	int combatWarpAccum;
    int pingAccum;
    int pingSamples;
    int score;
    int time;

    wweaponStats_t *w;
    wweaponStats_t *ws;  // [WOLFCAM_NUM_WEAPONS];

    if (oldclient) {
        woldclient_t *w = &woldclients[clientNum];

        name = woldclients[clientNum].clientinfo.name;
        kills = woldclients[clientNum].kills;
        deaths = woldclients[clientNum].deaths;
        suicides = woldclients[clientNum].suicides;
        warp = woldclients[clientNum].warp;
        nowarp = woldclients[clientNum].nowarp;
        warpAccum = woldclients[clientNum].warpAccum;
		combatWarp = woldclients[clientNum].combatWarp;
		combatNowarp = woldclients[clientNum].combatNowarp;
		combatWarpAccum = woldclients[clientNum].combatWarpAccum;
        pingAccum = w->pingAccum;
        pingSamples = w->pingSamples;
        score = w->score;
        time = w->time;

        ws = woldclients[clientNum].wstats;
    } else {
        wclient_t *w = &wclients[clientNum];

        name = cgs.clientinfo[clientNum].name;
        kills = wclients[clientNum].kills;
        deaths = wclients[clientNum].deaths;
        suicides = wclients[clientNum].suicides;
        warp = wclients[clientNum].warp;
        nowarp = wclients[clientNum].nowarp;
        warpAccum = wclients[clientNum].warpAccum;
		combatWarp = wclients[clientNum].combatWarp;
		combatNowarp = wclients[clientNum].combatNowarp;
		combatWarpAccum = w->combatWarpAccum;
        pingAccum = w->pingAccum;
        pingSamples = w->pingSamples;
        score = w->score;
        time = w->time;

        ws = wclients[clientNum].wstats;
    }

    CG_Printf ("^7Stats for  %s\n", name);
    CG_Printf ("^7----------------------------------------------\n");
    //    CG_Printf ("^7kills: %d  ^7deaths: %d  ^7suicides: %d  ^7warp: %d  %f  %d\n", kills, deaths, suicides, warp, (float)warp / (float)nowarp, warpAccum / warp);
    CG_Printf ("^7kills: ^7%d  ^7deaths: ^7%d  ^7suicides: ^7%d  ^7warp: %d  nowarp: %d",
               kills, deaths, suicides, warp, nowarp);
    if ((nowarp) > 0)
        CG_Printf ("  ^2%f", (float)warp / (float)(nowarp));
    else
        CG_Printf ("         ");
    if (warp > 0)
        CG_Printf ("  ^5%d\n", warpAccum / warp);
    else
        CG_Printf ("\n");

	CG_Printf ("        ^7combatWarp: %d  combatNowarp: %d", combatWarp, combatNowarp);
	if ((combatNowarp) > 0)
        CG_Printf ("  ^2%f", (float)combatWarp / (float)(combatNowarp));
    else
        CG_Printf ("         ");
    if (combatWarp > 0)
        CG_Printf ("  ^5%d\n", combatWarpAccum / combatWarp);
    else
        CG_Printf ("\n");
		
    CG_Printf ("                     ^7score: %d  time: %d  ping: ^2%d ^7(%d %s)\n", score, time, pingSamples ? pingAccum / pingSamples : -1, pingSamples, pingSamples == 1 ? "sample" : "samples");

    w = &ws[WP_SNIPERRIFLE];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2sniperrifle   ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_MAUSER];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2mauser        ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_MP40];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2mp40          ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_THOMPSON];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2thompson      ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_STEN];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2sten          ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_LUGER];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2luger         ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_COLT];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2colt          ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_VENOM];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2venom         ");
        print_ind_weap_stats (w);
    }

    w = &ws[WOLFCAM_WP_MG42];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2mg42          ");
        print_ind_weap_stats (w);
    }

    w = &ws[WP_KNIFE];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2knife         ");                    
        print_ind_weap_stats (w);                            
    }

    w = &ws[WP_DYNAMITE];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2dynamite      ");                    
        print_ind_weap_stats (w);                            
    }

    w = &ws[WP_PANZERFAUST];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2panzer        ");                    
        print_ind_weap_stats (w);                            
    }

    w = &ws[WP_GRENADE_PINEAPPLE];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2grenade       ");                    
        print_ind_weap_stats (w);                            
    }

    w = &ws[WP_FLAMETHROWER];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2flamethrower  ");                    
        print_ind_weap_stats (w);                            
    }

    //FIXME mg42

    w = &ws[WP_ARTY];
    if (w->kills  ||  w->deaths  ||  w->atts  ||  w->suicides) {
        CG_Printf ("  ^2arty          ");                    
        print_ind_weap_stats (w);                            
    }

}
void Wolfcam_Weapon_Stats_f (void)
{
    
    int clientNum;

    if (trap_Argc() < 2) {
        if (wolfcam_following)
            clientNum = wcg.clientNum;
        else
            clientNum = cg.clientNum;
    } else {
        clientNum = atoi(CG_Argv(1));
    }

    if (clientNum == -1)
        clientNum = cg.clientNum;

    if (clientNum < 0  ||  clientNum > MAX_CLIENTS) {
        CG_Printf ("invalid client number\n");
        return;
    }

    wolfcam_print_weapon_stats (clientNum, qfalse);
}

void Wolfcam_Weapon_Statsall_f (void)
{
    int i;

    for (i = 0;  i < MAX_CLIENTS;  i++) {
        if (cgs.clientinfo[i].infoValid) {
            wolfcam_print_weapon_stats (i, qfalse);
            CG_Printf ("\n");
        }
    }

    if (wnumOldClients) {
        CG_Printf ("disconnected clients:\n\n");
        for (i = 0;  i < wnumOldClients;  i++) {
            wolfcam_print_weapon_stats (i, qtrue);
            CG_Printf ("\n");
        }
    }
}
