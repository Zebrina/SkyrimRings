#pragma once

class Rings
{
private:
	static constexpr const RE::FormID AnyRingSlotFormID = 0x000800;
	static constexpr const size_t MaxAllowedRingSlots = 8u;

public:
	static bool IsObjectRing(RE::TESForm* object);

	Rings();
	Rings(const Rings&) = delete;
	Rings(Rings&&) = delete;

	static Rings* GetSingleton();

	void UpdateData(RE::TESDataHandler* data);

	size_t GetNumRingSlots() const;
	int GetRingSlotIndex(const RE::BGSEquipSlot* slot) const;
	const RE::BGSEquipSlot* GetRingSlot(int index) const;

	int GetNumEquippedRings(RE::Actor* actor) const;
	RE::TESObjectARMO* GetEquippedRing(RE::Actor* actor, int index) const;
	void EquipRing(RE::Actor* actor, RE::TESForm* ring, int index, bool preventRemoval, bool silent) const;
	void UnequipRingSlot(RE::Actor* actor, int index, bool silent) const;

	const RE::BGSEquipSlot* GetNextRingSlot(RE::Actor* actor, const RE::BGSEquipSlot* slot) const;

	void AttachRingModel(RE::Actor* actor, RE::TESBoundObject* ring, int index, bool firstPerson) const;
	void DetachRingModel(RE::Actor* actor, int index, bool firstPerson) const;
	void UpdateRingModels(RE::Actor* actor, bool firstPerson) const;
	void DetachAllRingModels(RE::Actor* actor, bool firstPerson) const;

private:
	RE::BSFixedString fingerBones[MaxAllowedRingSlots] = {
		"NPC R Finger10 [RF10]",
		"NPC L Finger10 [LF10]",
		"NPC R Finger30 [RF30]",
		"NPC L Finger30 [LF30]",
		"NPC R Finger20 [RF20]",
		"NPC L Finger20 [LF20]",
		"NPC R Finger40 [RF40]",
		"NPC L Finger40 [LF40]",
	};
	const RE::BSFixedString ringNodeNames[MaxAllowedRingSlots] = {
		"RING1",
		"RING2",
		"RING3",
		"RING4",
		"RING5",
		"RING6",
		"RING7",
		"RING8",
	};
	const RE::BSFixedString firstPersonRingNodeNames[MaxAllowedRingSlots] = {
		"RING1_1",
		"RING2_1",
		"RING3_1",
		"RING4_1",
		"RING5_1",
		"RING6_1",
		"RING7_1",
		"RING8_1",
	};
	RE::BGSEquipSlot* anyRingSlot{ nullptr };
	size_t numRingSlots{ 0 };
	size_t maxRingSlots{ 0 };
	RE::BGSEquipSlot* ringSlots[MaxAllowedRingSlots]{ nullptr };
	const RE::BGSEquipSlot* leftHand{ nullptr };
	mutable const RE::BGSEquipSlot* forcedRingSlot{ nullptr };

	static RE::BSSimpleList<RE::ExtraDataList*>* GetRingExtraLists(RE::Actor* actor, RE::TESForm* ring);

	static bool RingNodeEquals(const RE::NiNode* x, const RE::NiNode* y);
};