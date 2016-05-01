#if defined _spectate_included
	#endinput
#endif
#define _spectate_included

#include <foreach>
#include <YSI\y_hooks>
/*
* [Include] Spectate enhancer (spectate.inc)
* An advance spectate system include with camera freezer, crash free, bug free, auto redirecting, auto adjustments and supports callbacks.
*
* By Gammix - (c) Copyright 2015
* This file is provided as is (no warranties).
*/

/*
FUNCTIONS:
native
*/

#define SPECTATE_TYPE_NONE (0)
#define SPECTATE_TYPE_PLAYER (1)
#define SPECTATE_TYPE_VEHICLE (2)
#define SPECTATE_TYPE_FREE (3)

enum e_SPECTATE_DATA
{
	Spec_Id,
	Spec_Type,
	Spec_Mode,
	Spec_ClickTick
};
static pSpecData[MAX_PLAYERS][e_SPECTATE_DATA];
static pSpecWorldid[MAX_PLAYERS];

stock Spec_PlayerSpectatePlayer(playerid, targetplayerid, mode = SPECTATE_MODE_NORMAL)
{
	if (PlayerSpectatePlayer(playerid, targetplayerid, mode))
	{
	    pSpecData[playerid][Spec_Id] = targetplayerid;
	    pSpecData[playerid][Spec_Type] = SPECTATE_TYPE_PLAYER;
	    pSpecData[playerid][Spec_Mode] = mode;

		SetPlayerInterior(playerid, GetPlayerInterior(targetplayerid));
		SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetplayerid));

		if (IsPlayerInAnyVehicle(targetplayerid))
		{
  			PlayerSpectateVehicle(playerid, GetPlayerVehicleID(targetplayerid), mode);
		}

		return true;
	}

	return false;
}
#if defined _ALS_PlayerSpectatePlayer
    #undef PlayerSpectatePlayer
#else
    #define _ALS_PlayerSpectatePlayer
#endif
#define PlayerSpectatePlayer Spec_PlayerSpectatePlayer
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(pSpecData[playerid][Spec_Id] != INVALID_PLAYER_ID)
	{
		if(GetTickCountDifference(GetTickCount(), pSpecData[playerid][Spec_ClickTick]) < 1000) return 1;
		pSpecData[playerid][Spec_ClickTick] = GetTickCount();
		if(newkeys == 128) UpdatePlayerSpectate(playerid);
		if(newkeys == 4) UpdatePlayerSpectate(playerid, false);
	}
	return 1;
}
stock Spec_TogglePlayerSpectating(playerid, bool:toggle)
{
	if(TogglePlayerSpectating(playerid, toggle))
	{
	    pSpecData[playerid][Spec_Id] = INVALID_PLAYER_ID;
	    pSpecData[playerid][Spec_Type] = SPECTATE_TYPE_FREE;
	    pSpecData[playerid][Spec_Mode] = 0;

		return true;
	}

	return false;
}
#if defined _ALS_TogglePlayerSpectating
    #undef TogglePlayerSpectating
#else
    #define _ALS_TogglePlayerSpectating
#endif
#define TogglePlayerSpectating Spec_TogglePlayerSpectating

stock GetPlayerSpectateData(playerid, &spectatetype, &targetid, &mode)
{
	if(playerid < 0 || playerid >= MAX_PLAYERS) return false;
	spectatetype = pSpecData[playerid][Spec_Type];
    targetid = pSpecData[playerid][Spec_Id];
    mode = pSpecData[playerid][Spec_Mode];
    return true;
}
stock CanPlayerSpectate(playerid, targetid)
{
	if(targetid == playerid || !IsPlayerConnected(targetid) || GetPlayerState(targetid) == PLAYER_STATE_SPECTATING) return 0;
	return 1;
}
stock UpdatePlayerSpectate(playerid, bool:advance = true)
{
	switch(pSpecData[playerid][Spec_Type])
	{
	    case SPECTATE_TYPE_PLAYER:
	    {
			new targetid = pSpecData[playerid][Spec_Id];
			if(advance)
			{
				new iters;
				targetid++;
				if(targetid == MAX_PLAYERS) targetid = 0;
				while(targetid < MAX_PLAYERS && iters <= MAX_PLAYERS)
				{
					iters++;
					if(!CanPlayerSpectate(playerid, targetid))
					{
						targetid++;
						if(targetid >= MAX_PLAYERS - 1) targetid = 0;
						continue;
					}
					break;
				}
				PlayerSpectatePlayer(playerid, targetid, pSpecData[playerid][Spec_Mode]);
       			return targetid;
			}
			else
			{
				new iters;
				targetid--;
				if(targetid < 0) targetid = MAX_PLAYERS-1;
				while(targetid >= 0 && iters <= MAX_PLAYERS)
				{
					iters++;
					if(!CanPlayerSpectate(playerid, targetid))
					{
						targetid--;
						if(targetid < 0) targetid = MAX_PLAYERS - 1;
						continue;
					}
					break;
				}
				PlayerSpectatePlayer(playerid, targetid, pSpecData[playerid][Spec_Mode]);
			    return targetid;
			}
		}
	}
	return -1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	if (newstate == PLAYER_STATE_SPECTATING)
	{
		foreach(Player, i)
		{
			if(i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid) UpdatePlayerSpectate(i);
		}
  	}
	else if (((oldstate != PLAYER_STATE_DRIVER || oldstate != PLAYER_STATE_PASSENGER) && (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)) ||
			((oldstate != PLAYER_STATE_ONFOOT || oldstate != PLAYER_STATE_SPAWNED) && (newstate == PLAYER_STATE_ONFOOT || newstate == PLAYER_STATE_SPAWNED)))
	{
		foreach(Player, i)
		{
			if(i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid) PlayerSpectatePlayer(i, playerid, pSpecData[i][Spec_Mode]);
		}
  	}

	#if defined Spec_OnPlayerStateChange
       	return Spec_OnPlayerStateChange(playerid, newstate, oldstate);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerStateChange
    #undef OnPlayerStateChange
#else
    #define _ALS_OnPlayerStateChange
#endif
#define OnPlayerStateChange Spec_OnPlayerStateChange
#if defined Spec_OnPlayerStateChange
    forward Spec_OnPlayerStateChange(playerid, newstate, oldstate);
#endif

public OnPlayerUpdate(playerid)
{
	new worldid = GetPlayerVirtualWorld(playerid);
	if (pSpecWorldid[playerid] != worldid)
	{
 		pSpecWorldid[playerid] = worldid;

	    foreach (Player,i)
		{
			if(i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid) SetPlayerVirtualWorld(i, worldid);
		}
	}

 	#if defined Spec_OnPlayerUpdate
 		return Spec_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerUpdate
	#undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate Spec_OnPlayerUpdate
#if defined Spec_OnPlayerUpdate
    forward Spec_OnPlayerUpdate(playerid);
#endif

public OnPlayerSpawn(playerid)
{
	foreach(Player, i)
	{
		if (i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid) PlayerSpectateVehicle(i, playerid, pSpecData[i][Spec_Mode]);
	}

 	#if defined Spec_OnPlayerSpawn
 		return Spec_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn Spec_OnPlayerSpawn
#if defined Spec_OnPlayerSpawn
    forward Spec_OnPlayerSpawn(playerid);
#endif

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
	foreach(Player, i)
	{
		if (i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid) SetPlayerInterior(i, newinteriorid);
  	}

	#if defined Spec_OnPlayerInteriorChange
	  	return Spec_OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerInteriorChange
    #undef OnPlayerInteriorChange
#else
    #define _ALS_OnPlayerInteriorChange
#endif
#define OnPlayerInteriorChange Spec_OnPlayerInteriorChange
#if defined Spec_OnPlayerInteriorChange
    forward Spec_OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
#endif

public OnPlayerDeath(playerid, killerid, reason)
{
	new Float:cam[3];
	GetPlayerCameraPos(playerid, cam[0], cam[1], cam[2]);

	new Float:fpos[3];
	GetPlayerPos(playerid, fpos[0], fpos[1], fpos[2]);

	if(!GetPlayerInterior(playerid)) cam[2] += 5.0;
 	else cam[2] += 0.5;
	foreach(Player, i)
	{
		if (i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid)
		{
			SetPlayerCameraPos(i, cam[0], cam[1], cam[2]);
			SetPlayerCameraLookAt(i, fpos[0], fpos[1], fpos[2]);
		}
 	}

	#if defined Spec_OnPlayerDeath
	  	return Spec_OnPlayerDeath(playerid, killerid, reason);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerDeath
    #undef OnPlayerDeath
#else
    #define _ALS_OnPlayerDeath
#endif
#define OnPlayerDeath Spec_OnPlayerDeath
#if defined Spec_OnPlayerDeath
    forward Spec_OnPlayerDeath(playerid, killerid, reason);
#endif

public OnPlayerDisconnect(playerid, reason)
{
	foreach(Player, i)
	{
		if(i != playerid && pSpecData[i][Spec_Type] == SPECTATE_TYPE_PLAYER && pSpecData[i][Spec_Id] == playerid) UpdatePlayerSpectate(i);
	}

	#if defined Spec_OnPlayerDisconnect
	  	return Spec_OnPlayerDisconnect(playerid, reason);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect Spec_OnPlayerDisconnect
#if defined Spec_OnPlayerDisconnect
    forward Spec_OnPlayerDisconnect(playerid, reason);
#endif
