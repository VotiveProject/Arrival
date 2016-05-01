//3DMenu. Author: SDraw

#include <YSI\y_hooks>
#include <streamer>
#include <foreach>
/* Fake natives. Thanks to TheArcher.
native Create3DMenu(Float:x,Float:y,Float:z,Float:rotation,boxes,playerid);
native SetBoxText(MenuID,box,text[],materialsize,fontface[],fontsize,bold,fontcolor,backcolor,textalignment);
native Select3DMenu(playerid,MenuID);
native CancelSelect3DMenu(playerid,MenuID);
native Destroy3DMenu(MenuID);
*/

#define MAX_MENUS (128)
#define MAX_BOXES (16)

new SelectedMenu[MAX_PLAYERS];
new SelectedBox[MAX_PLAYERS];
enum MenuParams { Float:Rotation,Boxes,bool:IsExist,Objects[MAX_BOXES],Float:AddingX,Float:AddingY };
new MenuInfo[MAX_MENUS][MenuParams];

//Callbacks
forward OnPlayerSelect3DMenuBox(playerid,MenuID,selected);
forward OnPlayerChange3DMenuBox(playerid,MenuID,boxid);

stock Create3DMenu(Float:x,Float:y,Float:z,Float:rotation,boxes,playerid)
{
	if(boxes > MAX_BOXES || boxes <= 0) return -1;
	for(new i = 0; i < MAX_MENUS; i++)
	{
	    if(!MenuInfo[i][IsExist])
	    {
	        MenuInfo[i][Rotation] = rotation;
			MenuInfo[i][Boxes] = boxes;
			
            if(rotation == 0 || rotation == 360) { MenuInfo[i][AddingX] = 0.0; MenuInfo[i][AddingY] = -0.25; }
			if(rotation == 180) { MenuInfo[i][AddingX] = 0.0; MenuInfo[i][AddingY] = 0.25; }
			if(rotation == 90) { MenuInfo[i][AddingX] = 0.25; MenuInfo[i][AddingY] = 0.0; }
			if(rotation == 270) { MenuInfo[i][AddingX] = -0.25; MenuInfo[i][AddingY] = 0.0; }
			if((rotation > 0 && rotation < 90) || (rotation > 270 && rotation < 360))
			{
				MenuInfo[i][AddingX] = 0.25*floatsin(rotation,degrees);
				MenuInfo[i][AddingY] = -floatcos(rotation,degrees)*0.25;
			}
			if((rotation > 90 && rotation < 180) || (rotation > 180 && rotation < 270))
			{
				MenuInfo[i][AddingX] = 0.25*floatsin(rotation,degrees);
				MenuInfo[i][AddingY] = -floatcos(rotation,degrees)*0.25;
			}
	        for(new b = 0; b < boxes; b++)
			{
				if(b < 4) MenuInfo[i][Objects][b] = CreateDynamicObject(2661,x,y,z+1.65-0.55*b,0,0,rotation,-1,-1,playerid,100.0);
				if(b >= 4)
				{
				    new Float:NextLineX,Float:NextLineY;
				    NextLineX = floatcos(rotation,degrees)+0.05*floatcos(rotation,degrees); NextLineY = floatsin(rotation,degrees)+0.05*floatsin(rotation,degrees);
				    if(b < 8) MenuInfo[i][Objects][b] = CreateDynamicObject(2661,x+NextLineX,y+NextLineY,z+1.65-0.55*(b-4),0,0,rotation,-1,-1,playerid,100.0);
				    if(b > 7 && b < 12) MenuInfo[i][Objects][b] = CreateDynamicObject(2661,x+NextLineX*2,y+NextLineY*2,z+1.65-0.55*(b-8),0,0,rotation,-1,-1,playerid,100.0);
				    if(b > 11 && b < 16) MenuInfo[i][Objects][b] = CreateDynamicObject(2661,x+NextLineX*3,y+NextLineY*3,z+1.65-0.55*(b-12),0,0,rotation,-1,-1,playerid,100.0);
	            }
			}
			MenuInfo[i][IsExist] = true;
			return i;
		}
	}
	return -1;
}

stock SetBoxText(MenuID,box,text[],materialsize,fontface[],fontsize,bold,fontcolor,backcolor,textalignment)
{
	if(!MenuInfo[MenuID][IsExist]) return -1;
	if(box == MenuInfo[MenuID][Boxes] || box < 0) return -1;
	if(MenuInfo[MenuID][Objects][box] == INVALID_OBJECT_ID) return -1;
	SetDynamicObjectMaterialText(MenuInfo[MenuID][Objects][box],0,text,materialsize,fontface,fontsize,bold,fontcolor,backcolor,textalignment);
	return 1;
}

stock Select3DMenu(playerid,MenuID)
{
	if(!MenuInfo[MenuID][IsExist]) return -1;
	new Float:x,Float:y,Float:z;
	SelectedBox[playerid] = 0;
	SelectedMenu[playerid] = MenuID;
 	GetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x,y,z);
	SetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x+MenuInfo[MenuID][AddingX],y+MenuInfo[MenuID][AddingY],z);
	return 1;
}

#if defined FILTERSCRIPT
hook OnFilterScriptInit()
{
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
 		SelectedMenu[i] = -1;
		SelectedBox[i] = -1;
	}
	for(new i = 0; i < MAX_MENUS; i++)
	{
	    for(new b = 0; b < MAX_BOXES; b++) MenuInfo[i][Objects][b] = INVALID_OBJECT_ID;
     	MenuInfo[i][Boxes] = 0;
	 	MenuInfo[i][IsExist] = false;
	 	MenuInfo[i][AddingX] = 0.0;
 		MenuInfo[i][AddingY] = 0.0;
	}
}
hook OnFilterScriptExit()
{
	for(new i = 0; i < MAX_MENUS; i++)
	{
		if(MenuInfo[i][IsExist]) Destroy3DMenu(i);
	}
}
#else
hook OnGameModeInit()
{
	for(new i = 0; i < MAX_MENUS; i++)
	{
	    for(new b = 0; b < MAX_BOXES; b++) MenuInfo[i][Objects][b] = INVALID_OBJECT_ID;
	    MenuInfo[i][Boxes] = 0;
	    MenuInfo[i][IsExist] = false;
	    MenuInfo[i][AddingX] = 0.0;
 		MenuInfo[i][AddingY] = 0.0;
	}
}
hook OnGameModeExit()
{
	for(new i = 0; i < MAX_MENUS; i++)
	{
		if(MenuInfo[i][IsExist]) Destroy3DMenu(i);
	}
}
#endif
hook OnPlayerConnect(playerid)
{
    SelectedMenu[playerid] = -1;
	SelectedBox[playerid] = -1;
}

hook OnPlayerKeyStateChange(playerid,newkeys,oldkeys)
{
	if(SelectedMenu[playerid] != -1)
	{
		new MenuID = SelectedMenu[playerid];
		new Float:x,Float:y,Float:z;
	    if(newkeys == KEY_NO)
	    {
	        GetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x,y,z);
			SetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x-MenuInfo[MenuID][AddingX],y-MenuInfo[MenuID][AddingY],z);
			SelectedBox[playerid]++;
			if(SelectedBox[playerid] == MenuInfo[MenuID][Boxes]) SelectedBox[playerid] = 0;
			GetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x,y,z);
			SetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x+MenuInfo[MenuID][AddingX],y+MenuInfo[MenuID][AddingY],z);
		}
		if(newkeys == KEY_YES)
	    {
	        GetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x,y,z);
			SetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x-MenuInfo[MenuID][AddingX],y-MenuInfo[MenuID][AddingY],z);
			SelectedBox[playerid]--;
			if(SelectedBox[playerid] < 0) SelectedBox[playerid] = MenuInfo[MenuID][Boxes]-1;
			GetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x,y,z);
			SetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x+MenuInfo[MenuID][AddingX],y+MenuInfo[MenuID][AddingY],z);
		}
		if(newkeys == KEY_NO || newkeys == KEY_YES) CallLocalFunction("OnPlayerChange3DMenuBox","idd",playerid,MenuID,SelectedBox[playerid]);
		if(newkeys == KEY_SPRINT) CallLocalFunction("OnPlayerSelect3DMenuBox","idd",playerid,MenuID,SelectedBox[playerid]);
	}
}

stock CancelSelect3DMenu(playerid,MenuID)
{
    if(!MenuInfo[MenuID][IsExist]) return -1;
	new Float:x,Float:y,Float:z;
 	GetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x,y,z);
	SetDynamicObjectPos(MenuInfo[MenuID][Objects][SelectedBox[playerid]],x-MenuInfo[MenuID][AddingX],y-MenuInfo[MenuID][AddingY],z);
	SelectedMenu[playerid] = -1;
	SelectedBox[playerid] = -1;
	return 1;
}

stock Destroy3DMenu(MenuID)
{
    if(!MenuInfo[MenuID][IsExist]) return -1;
    foreach(Player,i) if(SelectedMenu[i] == MenuID) CancelSelect3DMenu(i,MenuID);
    for(new i = 0; i < MenuInfo[MenuID][Boxes]; i++)
    {
		DestroyDynamicObject(MenuInfo[MenuID][Objects][i]);
		MenuInfo[MenuID][Objects][i] = INVALID_OBJECT_ID;
	}
 	MenuInfo[MenuID][Boxes] = 0;
 	MenuInfo[MenuID][IsExist] = false;
 	MenuInfo[MenuID][AddingX] = 0.0;
 	MenuInfo[MenuID][AddingY] = 0.0;
	return 1;
}
