#define MAX_BARS				(84)
#define INVALID_BAR_VALUE		(Float:0xFFFFFFFF)
#define INVALID_BAR_ID			(Bar:-1)
#define pb_percent(%1,%2,%3,%4)	((%1 - 6.0) + ((((%1 + 6.0 + %2 - 2.0) - %1) / %3) * %4))

forward Bar:CreateProgressBar(playerid = INVALID_PLAYER_ID,Float:x, Float:y, Float:width=55.5, Float:height=3.2, color, Float:max=100.0);
forward Float:GetProgressBarValue(Bar:barid);

enum e_bar
{
	Float:pb_x,
	Float:pb_y,
	Float:pb_w,
	Float:pb_h,
	Float:pb_m,
	Float:pb_v,
	PlayerText:pb_t1,
	PlayerText:pb_t2,
	PlayerText:pb_t3,
	pb_color,
	bool:pb_created
}

static Bars[MAX_PLAYERS][MAX_BARS][e_bar];

stock Bar:CreateProgressBar(playerid = INVALID_PLAYER_ID,Float:x, Float:y, Float:width=55.5, Float:height=3.2, color, Float:max=100.0)
{
	new barid;
	for(barid = 0; barid < sizeof Bars; ++barid) if(!Bars[playerid][barid][pb_created]) break;
	if(Bars[playerid][barid][pb_created] || barid == sizeof Bars) return INVALID_BAR_ID;
	new PlayerText:in_t = CreatePlayerTextDraw(playerid,x, y, "_");
	Bars[playerid][barid][pb_t1] = in_t;
	PlayerTextDrawUseBox(playerid,in_t, 1);
	PlayerTextDrawTextSize(playerid,in_t, x + width, 0.0);
	PlayerTextDrawLetterSize(playerid,in_t, 1.0, height / 10);
	PlayerTextDrawBoxColor(playerid,in_t, 0x00000000 | (color & 0x000000FF));
	in_t = CreatePlayerTextDraw(playerid,x + 1.2, y + 2.15, "_");
	Bars[playerid][barid][pb_t2] = in_t;
	PlayerTextDrawUseBox(playerid,in_t, 1);
	PlayerTextDrawTextSize(playerid,in_t, x + width - 2.0, 0.0);
	PlayerTextDrawLetterSize(playerid,in_t, 1.0, height / 10 - 0.35);
	PlayerTextDrawBoxColor(playerid,in_t, (color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2)));
	in_t = CreatePlayerTextDraw(playerid,x + 1.2, y + 2.15, "_");
	Bars[playerid][barid][pb_t3] = in_t;
	PlayerTextDrawTextSize(playerid,in_t, pb_percent(x, width, max, 1.0), 0.0);
	PlayerTextDrawLetterSize(playerid,in_t, 1.0, height / 10 - 0.35);
	PlayerTextDrawBoxColor(playerid,in_t, color);
	Bars[playerid][barid][pb_x] = x;
	Bars[playerid][barid][pb_y] = y;
	Bars[playerid][barid][pb_w] = width;
	Bars[playerid][barid][pb_h] = height;
	Bars[playerid][barid][pb_m] = max;
	Bars[playerid][barid][pb_color] = color;
	Bars[playerid][barid][pb_created] = true;
	return Bar:barid;
}
stock DestroyProgressBar(playerid = INVALID_PLAYER_ID,Bar:barid)
{
	if(barid != INVALID_BAR_ID && Bar:-1 < barid < Bar:MAX_BARS)
	{
		if(!Bars[_:barid][pb_created]) return 0;
		PlayerTextDrawDestroy(playerid,Bars[playerid][_:barid][pb_t1]);
		PlayerTextDrawDestroy(playerid,Bars[playerid][_:barid][pb_t2]);
		PlayerTextDrawDestroy(playerid,Bars[playerid][_:barid][pb_t3]);
		Bars[playerid][_:barid][pb_t1] = PlayerText:0;
		Bars[playerid][_:barid][pb_t2] = PlayerText:0;
		Bars[playerid][_:barid][pb_t3] = PlayerText:0;
		Bars[playerid][_:barid][pb_x] = 0.0;
		Bars[playerid][_:barid][pb_y] = 0.0;
		Bars[playerid][_:barid][pb_w] = 0.0;
		Bars[playerid][_:barid][pb_h] = 0.0;
		Bars[playerid][_:barid][pb_m] = 0.0;
		Bars[playerid][_:barid][pb_v] = 0.0;
		Bars[playerid][_:barid][pb_color] = 0;
		Bars[playerid][_:barid][pb_created] = false;
		return 1;
	}
	return 0;
}

stock ShowProgressBarForPlayer(playerid = INVALID_PLAYER_ID, Bar:barid)
{
	if(IsPlayerConnected(playerid) && barid != INVALID_BAR_ID && Bar:-1 < barid < Bar:MAX_BARS)
	{
		if(!Bars[playerid][_:barid][pb_created])return 0;
		PlayerTextDrawShow(playerid, Bars[playerid][_:barid][pb_t1]);
		PlayerTextDrawShow(playerid, Bars[playerid][_:barid][pb_t2]);
		PlayerTextDrawShow(playerid, Bars[playerid][_:barid][pb_t3]);
		return 1;
	}
	return 0;
}

stock HideProgressBarForPlayer(playerid = INVALID_PLAYER_ID, Bar:barid)
{
	if(IsPlayerConnected(playerid) && barid != INVALID_BAR_ID && Bar:-1 < barid < Bar:MAX_BARS)
	{
		if(!Bars[playerid][_:barid][pb_created])return 0;
		PlayerTextDrawHide(playerid, Bars[playerid][_:barid][pb_t1]);
		PlayerTextDrawHide(playerid, Bars[playerid][_:barid][pb_t2]);
		PlayerTextDrawHide(playerid, Bars[playerid][_:barid][pb_t3]);
		return 1;
	}
	return 0;
}

stock SetProgressBarValue(playerid = INVALID_PLAYER_ID,Bar:barid, Float:value)
{
	if(barid == INVALID_BAR_ID || Bar:MAX_BARS < barid < Bar:-1) return 0;
	if(Bars[playerid][_:barid][pb_created])
	{
		value = (value < 0.0) ? (0.0) : (value > Bars[playerid][_:barid][pb_m]) ? (Bars[playerid][_:barid][pb_m]) : (value);
		PlayerTextDrawUseBox(playerid,Bars[playerid][_:barid][pb_t3], value > 0.0);
		Bars[playerid][_:barid][pb_v] = value;
		PlayerTextDrawTextSize(playerid,Bars[playerid][_:barid][pb_t3],pb_percent(Bars[playerid][_:barid][pb_x], Bars[playerid][_:barid][pb_w], Bars[playerid][_:barid][pb_m], value), 0.0);
		return 1;
	}
	return 0;
}

stock Float:GetProgressBarValue(Bar:barid)
{
	if(barid == INVALID_BAR_ID || Bar:MAX_BARS < barid < Bar:-1) return INVALID_BAR_VALUE;
	if(Bars[playerid][_:barid][pb_created]) return Bars[playerid][_:barid][pb_v];
	return INVALID_BAR_VALUE;
}

stock SetProgressBarMaxValue(playerid = INVALID_PLAYER_ID,Bar:barid, Float:max)
{
	if(barid == INVALID_BAR_ID || Bar:MAX_BARS < barid < Bar:-1) return 0;
	if(Bars[playerid][_:barid][pb_created])
	{
		Bars[playerid][_:barid][pb_m] = max;
		SetProgressBarValue(playerid, barid, Bars[playerid][_:barid][pb_v]);
		return 1;
	}
	return 0;
}

stock SetProgressBarColor(playerid = INVALID_PLAYER_ID,Bar:barid, color)
{
	if(barid == INVALID_BAR_ID || Bar:MAX_BARS < barid < Bar:-1) return 0;
	if(Bars[playerid][_:barid][pb_created])
	{
		Bars[playerid][_:barid][pb_color] = color;
		PlayerTextDrawBoxColor(playerid,Bars[playerid][_:barid][pb_t1], 0x00000000 | (color & 0x000000FF));
		PlayerTextDrawBoxColor(playerid,Bars[playerid][_:barid][pb_t2], (color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2)));
		PlayerTextDrawBoxColor(playerid,Bars[playerid][_:barid][pb_t3], color);
		return 1;
	}
	return 0;
}

stock ShowProgressBarForAll(Bar:barid)
{
	#if defined _foreach_included
	foreach(Player, i)
	#else
	for(new i = 0; i < MAX_PLAYERS; ++i)
		if(IsPlayerConnected(i))
	#endif
	#if defined IsPlayerNPC
		if(!IsPlayerNPC(i))
	#endif
	{
		ShowProgressBarForPlayer(i, barid);
	}
	return 1;
}

stock HideProgressBarForAll(Bar:barid)
{
	#if defined _foreach_included
	foreach(Player, i)
	#else
	for(new i = 0; i < MAX_PLAYERS; ++i)
		if(IsPlayerConnected(i))
	#endif
	#if defined IsPlayerNPC
		if(!IsPlayerNPC(i))
	#endif
	{
		HideProgressBarForPlayer(i, barid);
	}
	return 1;
}

stock UpdateProgressBar(Bar:barid, playerid=INVALID_PLAYER_ID)
{
	if(playerid == INVALID_PLAYER_ID)
	{
		return ShowProgressBarForAll(barid);
	} else {
		return ShowProgressBarForPlayer(playerid, barid);
	}
}
