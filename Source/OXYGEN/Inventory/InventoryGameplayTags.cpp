#include "InventoryGameplayTags.h"

namespace InventoryGameplayTags
{
	
	// Inventory
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Message_ItemAdded, "Inventory.Message.ItemAdded", "Message for when an item is added to the inventory");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Message_ItemRemoved, "Inventory.Message.ItemRemoved", "Message for when an item is removed from the inventory");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Message_ItemsChanged, "Inventory.Message.ItemsChanged", "Message for when items in the inventory are changed");
	
	// Quickbar
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_QuickBar_Message_SlotsChanged, "Inventory.QuickBar.Message.SlotsChanged", "Message for when the quickbar slots are changed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_QuickBar_Message_ActiveIndexChanged, "Inventory.QuickBar.Message.ActiveIndexChanged", "Message for when the active quickbar index is changed");
	
	//Loadout
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Loadout_Message_Changed, "Inventory.Loadout.Message.Changed", "Loadout slots changed");
	
	//Placement	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Placement_Message_GridChanged, "Inventory.Placement.Message.GridChanged", "Message for grid changed events");
}