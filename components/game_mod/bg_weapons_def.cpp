#include "stdafx.h"

// /bgame/bg_weapons_def.cpp:62
WeaponVariantDef *BG_GetWeaponVariantDef(unsigned int weaponIndex)
{
	return ((WeaponVariantDef *(__cdecl *)(unsigned int))0x00444740)(weaponIndex);
}

// /bgame/bg_weapons_def.cpp:70
WeaponDef *BG_GetWeaponDef(unsigned int weaponIndex)
{
	return ((WeaponDef *(__cdecl *)(unsigned int))0x00425770)(weaponIndex);
}

// /bgame/bg_weapons_def.cpp:83
unsigned int BG_GetWeaponIndex(WeaponVariantDef *weapVariantDef)
{
	return ((unsigned int(__cdecl *)(WeaponVariantDef *))0x00553DF0)(weapVariantDef);
}

// /bgame/bg_weapons_def.cpp:???
void BG_SetWeaponUsed(int clientIndex, int weapIndex)
{
	((DWORD *)0xBDF508)[weapIndex / 32 + 4 * clientIndex] |= 1 << weapIndex % 32;
}