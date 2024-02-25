#include "Papyrus.h"

#include "Rings.h"

using namespace RE;

int GetNumEquippedRings(StaticFunctionTag*, Actor* actor)
{
	return Rings::GetSingleton()->GetNumEquippedRings(actor);
}
TESObjectARMO* GetEquippedRing(StaticFunctionTag*, Actor* actor, int index)
{
	return Rings::GetSingleton()->GetEquippedRing(actor, index);
}
void EquipRing(StaticFunctionTag*, Actor* actor, TESForm* ring, int index, bool preventRemoval, bool silent)
{
	Rings::GetSingleton()->EquipRing(actor, ring, index, preventRemoval, silent);
}
void UnequipRingSlot(StaticFunctionTag*, Actor* actor, int index, bool silent)
{
	Rings::GetSingleton()->UnequipRingSlot(actor, index, silent);
}

bool Papyrus::RegisterFunctions(BSScript::IVirtualMachine* registry)
{
	registry->RegisterFunction("GetNumEquippedRings", Plugin::NAME, GetNumEquippedRings);
	registry->RegisterFunction("GetEquippedRing", Plugin::NAME, GetEquippedRing);
	registry->RegisterFunction("EquipRing", Plugin::NAME, EquipRing);
	registry->RegisterFunction("UnequipRingSlot", Plugin::NAME, UnequipRingSlot);

	return true;
}