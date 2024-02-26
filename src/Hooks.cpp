#include "Hooks.h"

#include "Rings.h"
#include "UniqueFunctionHook.h"

using namespace RE;

BGSEquipSlot* Hooks::LeftHand;

RELUNIQUE(ActorEquipManager, EquipObject)
void ActorEquipManager_EquipObject_Hook(ActorEquipManager* this__, Actor* actor, TESBoundObject* object, ExtraDataList* extraData, uint32_t count, const BGSEquipSlot* slot, bool queueEquip, bool forceEquip, bool playSounds, bool applyNow)
{
#ifndef NDEBUG
	if (!(((ID == 19692 && Offset == 0xFF)) ||
		  ((ID == 418622 && Offset == 0x71C)) ||
		  ((ID == 38898 && Offset == 0x25F)) ||
		  ((ID == 38907 && Offset == 0x2E0)) ||
		  ((ID == 38913 && Offset == 0x117)) ||
		  ((ID == 39814 && Offset == 0xCB)) ||
		  ((ID == 41244 && Offset == 0x150)) ||
		  ((ID == 54661 && Offset == 0x14E))))
	{
		SKSE::log::debug("EquipObject<{}, 0x{:X}>(actor: \"{}\"|{:08X}, object: \"{}\"|{:08X}, extra: {}, count: {}, ...)"sv, ID, Offset, actor->GetDisplayFullName(), actor->formID, object->GetName(), object->formID, (extraData ? "yes" : "no"), count);
	}
#endif
	if (Rings::IsObjectRing(object))
	{
		slot = Rings::GetSingleton()->GetNextRingSlot(actor, slot);
	}
	REL_CallOriginal(this__, actor, object, extraData, count, slot, queueEquip, forceEquip, playSounds, applyNow);
}

RELUNIQUE(ActorEquipManager, UnequipObject)
bool ActorEquipManager_UnequipObject_Hook(ActorEquipManager* this__, Actor* actor, TESBoundObject* object, ExtraDataList* extraData, uint32_t count, const BGSEquipSlot* slot, bool queueEquip, bool forceEquip, bool playSounds, bool applyNow, const BGSEquipSlot* slotToReplace)
{
#ifndef NDEBUG
	if (!(((ID == 37975 && Offset == 0x189)) ||
		  ((ID == 38004 && Offset == 0x1CE)) ||
		  ((ID == 38905 && Offset == 0x101)) ||
		  ((ID == 38907 && Offset == 0x2A9)) ||
		  ((ID == 39814 && Offset == 0x11E)) ||
		  ((ID == 54774 && Offset == 0xD9))))
	{
		SKSE::log::debug("UnequipObject<{}, 0x{:X}>(actor: \"{}\"|{:08X}, object: \"{}\"|{:08X}, extra: {}, count: {}, ...)"sv, ID, Offset, actor->GetDisplayFullName(), actor->formID, object->GetName(), object->formID, (extraData ? "yes" : "no"), count);
	}
#endif
	if (Rings::IsObjectRing(object) && actor->currentProcess)
	{
		// Items equipped in slots other than the default ones are not removed, so we do it here.
		auto& equippedObjects = actor->currentProcess->equippedForms;
		for (const auto& data : equippedObjects)
		{
			if (data.object == object)
			{
				//ringSlot = data.slot;
				equippedObjects.erase(&data);
				break;
			}
		}
	}
	return REL_CallOriginal(this__, actor, object, extraData, count, slot, queueEquip, forceEquip, playSounds, applyNow, slotToReplace);
}

RELUNIQUE(AIProcess, Update3DModel)
void AIProcess_Update3DModel_Hook(AIProcess* this__, Actor* actor)
{
	if (this__->middleHigh && this__->middleHigh->update3DModel.all(RESET_3D_FLAGS::kModel))
	{
		const Rings* rings = Rings::GetSingleton();
		rings->UpdateRingModels(actor, false);
		if (actor->IsPlayerRef())
		{
			rings->UpdateRingModels(actor, true);
		}
	}

	REL_CallOriginal(this__, actor);
}

void Hooks::Init()
{

	// EQUIPPING

	/*
	
	19692, 0xFF
	418622, 0x71C
	38898, 0x25F
	38907, 0x2E0
	38913, 0x117
	39814, 0xCB
	41244, 0x150
	54661, 0x14E

	
	*/




	REL_WriteUniqueCall(16059, 0x296F, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(16104, 0xCD, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(19689, 0x1B1, ActorEquipManager_EquipObject_Hook);

	// New game equip armor
	REL_WriteUniqueCall(19692, 0xFF, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(22351, 0x205, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(418622, 0x310, ActorEquipManager_EquipObject_Hook);

	// New game equip first available weapon
	REL_WriteUniqueCall(418622, 0x71C, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(29346, 0x84D, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(29383, 0x6A8, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(29383, 0x7E1, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(29383, 0xCBB, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(37797, 0x12B, ActorEquipManager_EquipObject_Hook);

	// Equip bound weapon
	REL_WriteUniqueCall(38898, 0x25F, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(38906, 0xFD, ActorEquipManager_EquipObject_Hook);

	// Inventory menu equip item
	REL_WriteUniqueCall(38907, 0x2E0, ActorEquipManager_EquipObject_Hook);

	// Automatic re-equip right hand weapon from inventory menu after unequipping bow.
	REL_WriteUniqueCall(38913, 0x117, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(39637, 0xF47, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(39649, 0xB7D, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(39649, 0xD18, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(39650, 0x718, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(39650, 0x7B6, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(39650, 0x80C, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(39650, 0xB0B, ActorEquipManager_EquipObject_Hook);


	// Called after Papyrus Actor.EquipItem
	REL_WriteUniqueCall(39814, 0xCB, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(39948, 0x199, ActorEquipManager_EquipObject_Hook);

	// Automatic equip arrows when equipping bow.
	REL_WriteUniqueCall(41244, 0x150, ActorEquipManager_EquipObject_Hook);

	REL_WriteUniqueCall(48124, 0x1D6, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(48126, 0x136, ActorEquipManager_EquipObject_Hook);
	REL_WriteUniqueCall(52410, 0x6F, ActorEquipManager_EquipObject_Hook);

	// Papyrus Actor.EquipItem
	REL_WriteUniqueCall(54661, 0x14E, ActorEquipManager_EquipObject_Hook);



	// UNEQUIPPING

	REL_WriteUniqueCall(16059, 0x27E7, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(16154, 0x71, ActorEquipManager_UnequipObject_Hook);

	// Console unequip armor
	REL_WriteUniqueCall(22353, 0x15D, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(29383, 0x8A2, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(29383, 0xB56, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(29383, 0xC34, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(34227, 0xFF, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(37520, 0xD6, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(37524, 0x263, ActorEquipManager_UnequipObject_Hook);

	// Unequip bound weapon.
	REL_WriteUniqueCall(37975, 0x189, ActorEquipManager_UnequipObject_Hook);

	// Unequip already equipped armor in inventory menu.
	REL_WriteUniqueCall(38004, 0x1CE, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(38004, 0x3BE, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(38546, 0x1F2, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(38747, 0xF6, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(38900, 0xC0, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(38900, 0x147, ActorEquipManager_UnequipObject_Hook);

	// Unequip already equipped weapons in inventory menu.
	REL_WriteUniqueCall(38905, 0x101, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(38907, 0x110, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(38907, 0x146, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(38907, 0x188, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(38907, 0x195, ActorEquipManager_UnequipObject_Hook);

	// Unequip already equipped torch in inventory menu.
	// Unequip arrow + bow from inventory menu.
	REL_WriteUniqueCall(38907, 0x2A9, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(38942, 0xBE, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(38942, 0xFA, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(39637, 0xED0, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(39650, 0x96D, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(39650, 0xA78, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(39683, 0xDB, ActorEquipManager_UnequipObject_Hook);

	// Called after Papyrus Actor.UnequipItem
	REL_WriteUniqueCall(39814, 0x11E, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(39948, 0x236, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(40486, 0xF1, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(41244, 0x257, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(44282, 0x364, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(51825, 0xFE, ActorEquipManager_UnequipObject_Hook);
	REL_WriteUniqueCall(52470, 0x118, ActorEquipManager_UnequipObject_Hook);

	// Papyrus: Actor.UnequipItem
	REL_WriteUniqueCall(54774, 0xD9, ActorEquipManager_UnequipObject_Hook);

	REL_WriteUniqueCall(54774, 0x1E0, ActorEquipManager_UnequipObject_Hook);



	// AIProcess::Update3DModel
	REL_WriteUniqueCall(36991, 0x2102, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(37360, 0x2E, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(37640, 0x450, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(37651, 0x1E1, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(40255, 0x197, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(40447, 0x308, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueJump(40467, 0xA, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(51126, 0x550, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(51141, 0x58C, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(51141, 0x58C, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(51548, 0xDA, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(51848, 0xB5D, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(52353, 0x1C2, AIProcess_Update3DModel_Hook);
	REL_WriteUniqueCall(52392, 0x50, AIProcess_Update3DModel_Hook);
}