#include "cg_local.h"
#include "wolfcam_local.h"

wcg_t wcg;
wclient_t wclients[MAX_CLIENTS];
woldclient_t woldclients[WOLFCAM_MAX_OLD_CLIENTS];
int wnumOldClients;

int wolfcam_initialSnapshotNumber;

int wolfcam_gameVersion = WOLFCAM_MAIN;

qboolean wolfcam_following;

vmCvar_t wolfcam_debugErrors;
vmCvar_t wolfcam_debugDrawGun;
vmCvar_t wolfcam_fakelag;
vmCvar_t wolfcam_fast_forward_timescale;
vmCvar_t wolfcam_fast_forward_invalid_time;
vmCvar_t wolfcam_avoidDemoPov;
vmCvar_t wolfcam_tryToStickWithTeam;
vmCvar_t wolfcam_warp_shader_time;
vmCvar_t wolfcam_wh;

char globalDebugString[DEBUG_BUFFER_SIZE];

#if 0

vec3_t wolfcam_prevOrigins[WOLFCAM_MAXBACKUPS];
int wolfcam_prevTimes[WOLFCAM_MAXBACKUPS];
entityState_t wolfcam_prevEs[WOLFCAM_MAXBACKUPS];

#endif

///////////////////////////////////////

#if 0
static char *csNames[] = {
    "CS_SERVERINFO",  // 0
    "CS_SYSTEMINFO",
    "CS_MUSIC",
    "CS_MESSAGE",
    "CS_MOTD",

    "CS_WARMUP",  // 5
CS_SCORES1
CS_SCORES2
CS_VOTE_TIME
CS_VOTE_STRING

    "CS_VOTE_YES",  // 10
CS_VOTE_NO
CS_GAME_VERSION
CS_LEVEL_START_TIME
CS_INTERMISSION

    "CS_MULTI_INFO",  // 15
CS_MULTI_MAPWINNER
CS_MULTI_MAPDESC
CS_MULTI_OBJECTIVE1
CS_MULTI_OBJECTIVE2

    "CS_MULTI_OBJECTIVE3",  // 20
CS_MULTI_OBJECTIVE4
CS_MULTI_OBJECTIVE5
CS_MULTI_OBJECTIVE6
CS_MULTI_OBJ1_STATUS

    "CS_MULTI_OBJ2_STATUS",  // 25
CS_MULTI_OBJ3_STATUS
CS_MULTI_OBJ4_STATUS
CS_MULTI_OBJ5_STATUS
CS_MULTI_OBJ6_STATUS

    "CS_SHADERSTATE",  // 30
CS_ITEMS
CS_SCREENFADE
CS_FOGVARS
CS_SKYBOXORG

    "CS_TARGETEFFECT",  // 35
CS_WOLFINFO
}
#endif

static char *csUnknown = "UKNOWN";

char *Wolfcam_ConfigStringName (int num)
{
    return csUnknown;
}

