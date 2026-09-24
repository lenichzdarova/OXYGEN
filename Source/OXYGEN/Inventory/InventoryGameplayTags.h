#pragma once

#include "NativeGameplayTags.h"

namespace InventoryGameplayTags
{
	//Inventory
	OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Message_ItemAdded);
	OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Message_ItemRemoved);
	OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Message_ItemsChanged);
	
	//Quickbar
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_QuickBar_Message_SlotsChanged);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_QuickBar_Message_ActiveIndexChanged)
	
	//Loadout
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Loadout_Message_Changed);
	
	//Placement
	OXYGEN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Placement_Message_GridChanged);
	
	
}