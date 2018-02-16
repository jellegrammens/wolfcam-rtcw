#ifndef wolfcam_h_included
#define wolfcam_h_included

//#include "cg_local.h"

#define DEBUG_BUFFER_SIZE 4192

#define WOLFCAM_VERSION "0.0.5"

#define WOLFCAM_UNKNOWN 0
#define WOLFCAM_MAIN 1
//#define WOLFCAM_ETMAIN 1
//#define WOLFCAM_ETPRO 2

// rtcw
#define TEAM_AXIS TEAM_RED
#define TEAM_ALLIES TEAM_BLUE

#define PAIN_EV_TIME 700

#define MAX_SNAPSHOT_BACKUP     256
#define MAX_SNAPSHOT_MASK       (MAX_SNAPSHOT_BACKUP - 1)

#define WOLFCAM_MAX_OLD_CLIENTS (MAX_CLIENTS * 4)

#define WOLFCAM_NUM_WEAPONS    (WP_NUM_WEAPONS + 1)
#define WOLFCAM_WP_MG42        WP_NUM_WEAPONS

#define GetAmmoTableData(ammoIndex) ((ammotable_t*)(&ammoTable[ammoIndex]))

// wolfcam global vars
typedef struct {
    int selectedClientNum;  // client selected by user
    int clientNum;  // client currently following

    vec3_t vieworgDemo;
    vec3_t refdefViewAnglesDemo;

    snapshot_t snaps[MAX_SNAPSHOT_BACKUP];
    int curSnapshotNumber;

    float leanf;

    qboolean fastForward;
    qboolean fastForwarding;
    char oldTimescale[MAX_CVAR_VALUE_STRING];
    char oldVolume[MAX_CVAR_VALUE_STRING];
} wcg_t;

//FIXME wolfcam mg42
typedef struct {
    unsigned int atts;
    unsigned int hits;
    unsigned int teamHits;
    unsigned int corpseHits;  // gibbing
    unsigned int teamCorpseHits;
    unsigned int headshots;
    unsigned int kills;
    unsigned int deaths;
    unsigned int suicides;
} wweaponStats_t;


typedef struct {
    qboolean currentValid;

    int invalidTime;
    int validTime;

    int health;
    int healthTime;

    int eventHealth;  // guessed from EV_*
    //int eventHealthTime;
    int ev_pain_time;  // EV_PAIN
    int bulletDamagePitch;
    int bulletDamageYaw;

    int landChange;
    int landTime;

    int duckChange;
    int duckTime;

    qboolean zoomedBinoc;
    int binocZoomTime;
    int zoomTime;
    int zoomval;

    int event_fireWeaponTime;
    int event_akimboFire;
    int event_overheatTime;
    int event_reloadTime;
    int event_weaponSwitchTime;
    int event_newWeapon;
    int event_oldWeapon;
    
    int grenadeDynoTime;
    int grenadeDynoLastTime;

    int muzzleFlashTime;
    int bulletDamageTime;  //FIXME wolfcam tmp garbage

    entityState_t esPrev;
    int prevTime;

    int weapAnim;
    int weapAnimPrev;
    int weapAnimTimer;

    float xyspeed;
    float bobfracsin;
    int bobcycle;
    int psbobCycle;
    float lastvalidBobfracsin;
    int lastvalidBobcycle;

    qboolean crosshairClientNoShoot;

    // blend blobs
    viewDamage_t viewDamage[MAX_VIEWDAMAGE];
    float damageTime;  // last time any kind of damage was recieved
    int damageIndex;  // slot that was filled in
    float damageX, damageY, damageValue;

    float v_dmg_time;
    float v_dmg_pitch;
    float v_dmg_roll;

    int kills;
    int deaths;
    int suicides;

    //FIXME wolfcam ammo

    int warp;
    int nowarp;
    int warpAccum;
	int warpTime;

	int combatWarp;
	int combatNowarp;
	int combatWarpAccum;

    int pingAccum;
    int pingSamples;

    // for convenience
    int score;
    int ping;
    int time;  // time playing

    wweaponStats_t wstats[WOLFCAM_NUM_WEAPONS];
    
} wclient_t;

typedef struct {
    qboolean valid;

    int clientNum;
    clientInfo_t clientinfo;


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
    int ping;
    int time;

    wweaponStats_t wstats[WOLFCAM_NUM_WEAPONS];

} woldclient_t;

extern int wolfcam_gameVersion;

extern char globalDebugString[DEBUG_BUFFER_SIZE];
extern int wolfcam_initialSnapshotNumber;

extern qboolean wolfcam_following;

extern wcg_t wcg;
extern wclient_t wclients[MAX_CLIENTS];
extern woldclient_t woldclients[WOLFCAM_MAX_OLD_CLIENTS];  // clients that have disconnected
extern int wnumOldClients;

extern vmCvar_t wolfcam_debugErrors;
extern vmCvar_t wolfcam_debugDrawGun;
extern vmCvar_t wolfcam_fakelag;
extern vmCvar_t wolfcam_fast_forward_timescale;
extern vmCvar_t wolfcam_fast_forward_invalid_time;
extern vmCvar_t wolfcam_avoidDemoPov;
extern vmCvar_t wolfcam_tryToStickWithTeam;
extern vmCvar_t wolfcam_warp_shader_time;
extern vmCvar_t wolfcam_wh;

#if 0

#define WOLFCAM_MAXBACKUPS 16
extern vec3_t wolfcam_prevOrigins[WOLFCAM_MAXBACKUPS];
extern int wolfcam_prevTimes[WOLFCAM_MAXBACKUPS];
extern entityState_t wolfcam_prevEs[WOLFCAM_MAXBACKUPS];

#endif



void Wolfcam_Players_f (void);
void Wolfcam_Follow_f (void);
void Wolfcam_Server_Info_f (void);
void Wolfcam_Weapon_Stats_f (void);
void Wolfcam_Weapon_Statsall_f (void);

void Wolfcam_DemoClick (int key, qboolean down);

void Wolfcam_OwnerDraw (float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float speial, float scale, vec4_t color, qhandle_t shader, int textStyle);

//void Wolfcam_Draw2D (void);
void Wolfcam_ColorForHealth (vec4_t hcolor);
void Wolfcam_AddViewWeapon (void);

void Wolfcam_TransitionPlayerState (void);

void Wolfcam_ZoomIn (void);
void Wolfcam_ZoomOut (void);
int Wolfcam_CalcFov (void);
int Wolfcam_OffsetThirdPersonView (void);
int Wolfcam_OffsetFirstPersonView (void);

void Wolfcam_EntityEvent (centity_t *cent, vec3_t position);

//void Wolfcam_DrawFollowing (void);
//void Wolfcam_DrawCrosshairNames (void);

void wolfcam_etpro_command (const char *cmd);

void Wolfcam_DamageBlendBlob (void);
void Wolfcam_DrawActive( stereoFrame_t stereoView );
void Wolfcam_MarkValidEntities (void);

void Wolfcam_NextSnapShotSet (void);

char *Wolfcam_ConfigStringName (int num);
void Wolfcam_ScoreData (void);  // score data has arrived

void wolfcam_check_headshot (int sourceClientNum, int hitClientNum, vec3_t endPos);

qboolean Wolfcam_ServerCommand (void);

#endif // wolfcam_h_include
