#include "cg_local.h"
#include "../ui/keycodes.h"
#include "wolfcam_local.h"

#define LEAN_MAX 28.0f

extern void CG_ScoresUp_f (void);
extern void CG_ScoresDown_f (void);

// from http://www.quake3world.com/ubb/Forum4/HTML/006208.html by RR2DO2
// and ET sdk

void Wolfcam_DemoClick (int key, qboolean down)
{
    int t;

    t = trap_Milliseconds();

    //CG_Printf ("^3%d:%d\n", key, down);

    // Avoid active console keypress issues
    if (!down  &&  !cgs.fKeyPressed[key])
        return;

    cgs.fKeyPressed[key] = down;

    switch (key) {
    case K_TAB:
        if (down)
            CG_ScoresDown_f ();
        else
            CG_ScoresUp_f ();
        return;
    case 'q':
        //FIXME wolfcam maybe just add a lean flag and smooth out lean
        if (down)
            wcg.leanf -= LEAN_MAX;
        else
            wcg.leanf += LEAN_MAX;
        break;
    case 'e':
        if (down)
            wcg.leanf += LEAN_MAX;
        else
            wcg.leanf -= LEAN_MAX;
        break;
	case K_UPARROW:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "50");
		CG_Printf ("[cgnotify]^3timescale ^750\n");
		break;
	case K_DOWNARROW:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "0.1");
		CG_Printf ("[cgnotify]^3timescale ^70.1\n");
		break;
	case K_KP_LEFTARROW:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "0.4");
		CG_Printf ("[cgnotify]^3timescale ^70.4\n");
		break;
	case K_KP_END:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "0.1");
		CG_Printf ("[cgnotify]^3timescale ^70.1\n");
		break;
	case K_KP_INS:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "1");
		CG_Printf ("[cgnotify]^3timescale ^71\n");
		break;
	case K_KP_HOME:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "200");
		CG_Printf ("[cgnotify]^3timescale ^7200\n");
		break;
	case K_KP_RIGHTARROW:
		if (down)
			break;
		trap_Cvar_Set ("timescale", "4");
		CG_Printf ("[cgnotify]^3timescale ^74\n");
		break;
	case K_ENTER: 
    case K_KP_MINUS: {
		char buffer[1024];
		int i;
		if (down)
			break;
		trap_Cvar_VariableStringBuffer ("wolfcam_wh", buffer, sizeof(buffer));
		i = atoi (buffer);
		if (i)
			CG_Printf ("[cgnotify]^3wolfcam_wh ^1off\n");
		else
			CG_Printf ("[cgnotify]^3wolfcam_wh ^2on\n");
		trap_Cvar_Set ("wolfcam_wh", va("%d", !i));
		break;
	}
    default:
        break;
    }
    //CG_Printf ("^3%d:%d\n", key, down);

}
