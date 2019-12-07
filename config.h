/* See LICENSE file for copyright and license details. */

#include <X11/DECkeysym.h>
#include <X11/HPkeysym.h>
#include <X11/Sunkeysym.h>
#include <X11/XF86keysym.h>
#include <X11/ap_keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_normfg[]      = "#bbbbbb";
static const char col_normbg[]      = "#222222";
static const char col_normborder[]  = "#000000";
static const char col_selfg[]       = "#ffffff";
static const char col_selbg[]       = "MediumOrchid4";
static const char col_selborder[]   = "MediumOrchid1";
static const char *colors[][3]      = {
	/*               fg          bg          border         */
	[SchemeNorm] = { col_normfg, col_normbg, col_normborder },
	[SchemeSel] =  { col_selfg,  col_selbg,  col_selborder  },
};

/* tagging */
static const char *tags[] = { "♈", "♉", "♊", "♋", "♌", "♍", "♎", "♏", "♐", "♑", "♒", "♓" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "MPlayer",  "xv",       NULL,       0,            1,           -1 },
	{ "mpv",      "gl",       NULL,       0,            1,           -1 },
	{ "mpv",      "xv",       NULL,       0,            1,           -1 },
	{ "SDL_App",  NULL,       NULL,       0,            1,           -1 },
	{ NULL,       NULL,   "Event Tester", 0,            1,           -1 },
};

/* layout(s) */
#define MFCT 0.60
static const float mfact     = MFCT; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_normbg, "-nf", col_normfg, "-sb", col_selbg, "-sf", col_selfg, NULL };
static const char *termcmd[]  = { "urxvtc", NULL };
static const char *mpctogglecmd[] = { "mpc", "toggle", NULL };
static const char *mpcstopcmd[] = { "mpc", "stop", NULL };
static const char *mpcprevcmd[] = { "mpc", "prev", NULL };
static const char *mpcnextcmd[] = { "mpc", "next", NULL };
#ifdef __linux__
static const char *amixer_mute_cmd[] = { "amixer", "sset", "Master,0", "toggle", NULL };
static const char *amixer_micmute_cmd[] = { "amixer", "sset", "Capture,0", "toggle", NULL };
static const char *amixer_lower_cmd[] = { "amixer", "-M", "sset", "Master,0", "2dB-", NULL };
static const char *amixer_raise_cmd[] = { "amixer", "-M", "sset", "Master,0", "2dB+", NULL };
static const char *amixer_shift_mute_cmd[] = { "amixer", "-R", "sset", "Master,0", "0", NULL };
static const char *amixer_shift_lower_cmd[] = { "amixer", "-M", "sset", "Master,0", "--", "-42dB", NULL };
static const char *amixer_shift_raise_cmd[] = { "amixer", "-M", "sset", "Master,0", "--", "-27dB", NULL };
#endif

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.005} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.005} },
	{ MODKEY,                       XK_s,      setmfact,       {.f = 1.0 + MFCT } },
	{ MODKEY|ShiftMask,             XK_s,      setmfact,       {.f = 1.0 + 0.50 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_a,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_a,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	TAGKEYS(                        XK_F1,                     0)
	TAGKEYS(                        XK_F2,                     1)
	TAGKEYS(                        XK_F3,                     2)
	TAGKEYS(                        XK_F4,                     3)
	TAGKEYS(                        XK_F5,                     4)
	TAGKEYS(                        XK_F6,                     5)
	TAGKEYS(                        XK_F7,                     6)
	TAGKEYS(                        XK_F8,                     7)
	TAGKEYS(                        XK_F9,                     8)
	TAGKEYS(                        XK_F10,                    9)
	TAGKEYS(                        XK_F11,                    10)
	TAGKEYS(                        XK_F12,                    11)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ 0,                            XF86XK_AudioPlay, spawn,   {.v = mpctogglecmd } },
	{ 0,                            XF86XK_AudioStop, spawn,   {.v = mpcstopcmd } },
	{ 0,                            XF86XK_AudioPrev, spawn,   {.v = mpcprevcmd } },
	{ 0,                            XF86XK_AudioNext, spawn,   {.v = mpcnextcmd } },
#ifdef __linux__
	{ 0,				XF86XK_AudioMute, spawn,   {.v = amixer_mute_cmd } },
	{ 0,				XF86XK_AudioLowerVolume, spawn,   {.v = amixer_lower_cmd } },
	{ 0,				XF86XK_AudioRaiseVolume, spawn,   {.v = amixer_raise_cmd } },
	{ 0,				XF86XK_AudioMicMute, spawn,   {.v = amixer_micmute_cmd } },
	{ ShiftMask,			XF86XK_AudioMute, spawn,   {.v = amixer_shift_mute_cmd } },
	{ ShiftMask,			XF86XK_AudioLowerVolume, spawn,   {.v = amixer_shift_lower_cmd } },
	{ ShiftMask,			XF86XK_AudioRaiseVolume, spawn,   {.v = amixer_shift_raise_cmd } },
#endif
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

