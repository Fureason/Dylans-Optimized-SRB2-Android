// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2023 by SRB2 Mobile Project.
// Copyright (C) 2025 by Bitten2Up.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------



#include "SDL.h"
#include "SDL_main.h"
#include "SDL_config.h"

#include "time.h"  

#include <jni_android.h>

#include "../sdlmain.h"

#include "../../doomdef.h"
#include "../../m_argv.h"
#include "../../d_main.h"
#include "../../m_misc.h"  
#include "../../i_system.h"

#ifdef SPLASH_SCREEN
#include "../../i_video.h"
#include "../ogl_es_sdl.h"
#endif

#ifdef HAVE_TTF
#include "i_ttf.h"
#endif

#ifdef LOGMESSAGES
FILE *logstream = NULL;
char logfilename[1024];
#endif

static boolean StorageInit(void)
{
	JNI_SharedStorage = I_SharedStorageLocation();
	return (JNI_SharedStorage != NULL);
}

static void StorageGrantedPermission(void)
{
	I_mkdir(JNI_SharedStorage, 0755);
	JNI_StoragePermission = true;
}

static boolean StorageCheckPermission(void)
{
	StorageGrantedPermission();
	return true;
}

#ifdef LOGMESSAGES
static void InitLogging(void)
{
	const char *logdir = NULL;
	time_t my_time;
	struct tm * timeinfo;
	const char *format;
	const char *reldir;
	int left;
	boolean fileabs;
#ifdef LOGSYMLINK
	const char *link;
#endif

	logdir = D_Home();

	my_time = time(NULL);
	timeinfo = localtime(&my_time);

	if (M_CheckParm("-logfile") && M_IsNextParm())
	{
		format = M_GetNextParm();
		fileabs = M_IsPathAbsolute(format);
	}
	else
	{
		format = "log-%Y-%m-%d_%H-%M-%S.txt";
		fileabs = false;
	}

	if (fileabs)
	{
		strftime(logfilename, sizeof logfilename, format, timeinfo);
	}
	else
	{
		if (M_CheckParm("-logdir") && M_IsNextParm())
			reldir = M_GetNextParm();
		else
			reldir = "logs";

		if (M_IsPathAbsolute(reldir))
		{
			left = snprintf(logfilename, sizeof logfilename,
					"%s"PATHSEP, reldir);
		}
		else if (logdir)
		{
			left = snprintf(logfilename, sizeof logfilename,
					"%s"PATHSEP "%s"PATHSEP, logdir, reldir);
		}
		else
		{
			left = snprintf(logfilename, sizeof logfilename,
					"."PATHSEP"%s"PATHSEP, reldir);
		}

		strftime(&logfilename[left], sizeof logfilename - left,
				format, timeinfo);
	}

	M_MkdirEachUntil(logfilename,
			M_PathParts(logdir) - 1,
			M_PathParts(logfilename) - 1, 0755);

	logstream = fopen(va("%s/latest-log.txt", I_SharedStorageLocation()), "wt+");
}
#endif

static INT32 I_StoragePermission(void)
{
#if defined(__ANDROID__)
    return (INT32)JNI_StoragePermissionGranted();
#else
    return 1;
#endif
}

#if defined(__ANDROID__)
static int Android_EventFilter(void *userdata, SDL_Event *event)
{
    (void)userdata;

    switch (event->type)
    {
        case SDL_APP_LOWMEMORY:
        case SDL_APP_TERMINATING:

            return 0;



        default:
            break;
    }

    return 1;
}
#endif

int main(int argc, char* argv[])
{
#ifdef LOGMESSAGES
	boolean logging = !M_CheckParm("-nolog");
#endif

	myargc = argc;
	myargv = argv;


	JNI_Startup();


	I_OutputMsg("I_StartupSystem()...\n");
	I_StartupSystem();


	Impl_InitVideoSubSystem();


	SDL_SetEventFilter(Android_EventFilter, NULL);


	if (StorageInit())
		StorageCheckPermission();  

#ifdef LOGMESSAGES

	if (logging && I_StoragePermission())
		InitLogging();
#endif

	CONS_Printf("Sonic Robo Blast 2 for Android\n");

#ifdef LOGMESSAGES
	if (logstream)
		CONS_Printf("Logfile: %s\n", logfilename);
#endif


	CONS_Printf("Setting up SRB2...\n");
	D_SRB2Main();

	CONS_Printf("Entering main game loop...\n");
	D_SRB2Loop();

	return 0;
}
