#include "InventoryGameplayTags.h"

namespace InventoryGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Message_ItemAdded, "Inventory.Message.ItemAdded", "Message for when an item is added to the inventory");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Message_ItemRemoved, "Inventory.Message.ItemRemoved", "Message for when an item is removed from the inventory");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Message_ItemsChanged, "Inventory.Message.ItemsChanged", "Message for when items in the inventory are changed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_Placement_Message_GridChanged, "Inventory.Placement.Message.GridChanged", "Message for grid changed events");
}