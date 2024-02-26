#include "Rings.h"

using namespace RE;

bool Rings::IsObjectRing(TESForm* object)
{
	return object->IsArmor() && static_cast<TESObjectARMO*>(object)->GetSlotMask() == BGSBipedObjectForm::BipedObjectSlot::kRing;
}

Rings::Rings()
{
	for (int i = 0; i < MaxAllowedRingSlots; i++)
	{
		std::stringstream nodeName;
		nodeName << "Ring" << (i + 1) << "AttachToNodeName";
		fingerBones[i] = Config::Instance().Get("Meshes", nodeName.str(), fingerBones[i].c_str());
	}
}

Rings* Rings::GetSingleton()
{
	static Rings instance;
	return &instance;
}

void Rings::UpdateData(TESDataHandler* data)
{
	FormID anyRingSlotFormID = Config::Instance().Get("Forms", "AnyRingSlotFormID", AnyRingSlotFormID);

	anyRingSlot = data->LookupForm<BGSEquipSlot>(anyRingSlotFormID, Config::Instance().GetPluginFileName());
	if (anyRingSlot == nullptr || anyRingSlot->formType != BGSEquipSlot::FORMTYPE)
	{
		SKSE::log::critical("Plugin file '{}' not enabled or AnyRingSlot form id is no longer XX{:06X}!", Config::Instance().GetPluginFileName(), anyRingSlotFormID);
		return;
	}

	// This might be dangerous.
#ifndef BETTER_SAFE_THAN_SORRY
	auto& equipSlots = data->formArrays[(uint32_t)FormType::EquipSlot];
	for (auto it = equipSlots.begin(); it != equipSlots.end(); ++it)
	{
		static_cast<BGSEquipSlot*>(*it)->pad3C = 0xFFFFFFFF; // -1
	}
#endif

	maxRingSlots = std::min(MaxAllowedRingSlots, (size_t)anyRingSlot->parentSlots.size());

	numRingSlots = Config::Instance().Get("Gameplay", "NumRingSlots", maxRingSlots);

	for (int i = 0; i < maxRingSlots; i++)
	{
		ringSlots[i] = anyRingSlot->parentSlots[i];
#ifndef BETTER_SAFE_THAN_SORRY
		ringSlots[i]->pad3C = i;
#endif
	}

	auto& armors = data->formArrays[(uint32_t)FormType::Armor];

	for (auto it = armors.begin(); it != armors.end(); ++it)
	{
		TESObjectARMO* armor = static_cast<TESObjectARMO*>(*it);

		if (IsObjectRing(armor))
		{
			armor->SetEquipSlot(anyRingSlot);
			armor->armorAddons.clear();
		}
	}

	leftHand = static_cast<BGSEquipSlot*>(TESForm::LookupByID(0x013F43));
}

size_t Rings::GetNumRingSlots() const
{
	return numRingSlots;
}
int Rings::GetRingSlotIndex(const BGSEquipSlot* slot) const
{
	if (!slot)
	{
		return -1;
	}
#ifndef BETTER_SAFE_THAN_SORRY
	int returnValue = *reinterpret_cast<const int*>(&(slot->pad3C));
	return *reinterpret_cast<const int*>(&(slot->pad3C));
#else
	for (int i = 0; i < maxRingSlots; i++)
	{
		if (ringSlots[i] == slot)
		{
			return i;
		}
	}
	return -1;
#endif
}
const BGSEquipSlot* Rings::GetRingSlot(int index) const
{
	return ringSlots[index];
}

int Rings::GetNumEquippedRings(Actor* actor) const
{
	if (!actor || !actor->currentProcess)
	{
		return 0;
	}

	auto& equippedObjects = actor->currentProcess->equippedForms;

	return std::count_if(equippedObjects.cbegin(), equippedObjects.cend(),
						 [this](const auto& equippedObject)
	{
		return GetRingSlotIndex(equippedObject.slot) >= 0;
	});
}
TESObjectARMO* Rings::GetEquippedRing(Actor* actor, int index) const
{
	if (!actor || !actor->currentProcess || index < 0 || index >= numRingSlots)
	{
		return nullptr;
	}

	BGSEquipSlot* ringSlot = ringSlots[index];

	auto& equippedObjects = actor->currentProcess->equippedForms;

	for (const auto& equippedObject : equippedObjects)
	{
		if (equippedObject.slot == ringSlot)
		{
			return static_cast<TESObjectARMO*>(equippedObject.object);
		}
	}

	return nullptr;
}
void Rings::EquipRing(Actor* actor, TESForm* ring, int index, bool preventRemoval, bool silent) const
{
	if (!actor || !ring || !actor->currentProcess || index < 0 || index >= numRingSlots || !IsObjectRing(ring))
	{
		return;
	}

	// This is the Actor.EquipItem Papyrus function.
	// The first two parameters don't seem to be used so it should be safe to use it like this.
	static REL::Relocation<void(*)(BSScript::IVirtualMachine*, VMHandle, Actor*, TESForm*, bool, bool)> PapyrusEquipItem{ REL::ID(54661).address() };

	forcedRingSlot = GetRingSlot(index);

	PapyrusEquipItem(nullptr, 0, actor, ring, preventRemoval, silent);

	forcedRingSlot = nullptr;
}
void Rings::UnequipRingSlot(Actor* actor, int index, bool silent) const
{
	if (!actor || !actor->currentProcess || index < 0 || index >= numRingSlots)
	{
		return;
	}

	TESObjectARMO* equippedRing = GetEquippedRing(actor, index);
	if (!equippedRing)
	{
		return;
	}

	// This is the Actor.UnequipItem Papyrus function.
	// The first two parameters don't seem to be used so it should be safe to use it like this.
	static REL::Relocation<void(*)(BSScript::IVirtualMachine*, VMHandle, Actor*, TESForm*, bool, bool)> PapyrusUnequipItem{ REL::ID(54774).address() };

	PapyrusUnequipItem(nullptr, 0, actor, equippedRing, false, silent);
}

const BGSEquipSlot* Rings::GetNextRingSlot(Actor* actor, const BGSEquipSlot* slot) const
{
	if (GetRingSlotIndex(slot) >= 0)
	{
		return slot;
	}

	if (forcedRingSlot)
	{
		return forcedRingSlot;
	}

	BGSEquipSlot* nextRingSlot = ringSlots[0];

	if (actor->currentProcess)
	{
		uint8_t occupied = 0x00;

		for (const auto& data : actor->currentProcess->equippedForms)
		{
			if (IsObjectRing(data.object))
			{
				occupied |= (1 << GetRingSlotIndex(data.slot));
			}
		}

		if (occupied == 0xFF)
		{
			// All slots are occupied. Find the last one that is not locked.
			for (int i = numRingSlots - 1; i >= 0; i--)
			{
				TESObjectARMO* equippedRing = GetEquippedRing(actor, i);
				if (equippedRing)
				{
					auto extraLists = GetRingExtraLists(actor, equippedRing);
					if (extraLists)
					{
						bool isLocked = false;
						for (auto extraList : *extraLists)
						{
							if (extraList->HasType<ExtraCannotWear>())
							{
								isLocked = true;
								break;
							}
						}
						if (!isLocked)
						{
							return ringSlots[i];
						}
					}
				}
			}
			// If for some reason they are all locked return the last one.
			return ringSlots[numRingSlots - 1];
		}

		nextRingSlot = ringSlots[numRingSlots - 1];
		for (int i = 0, n = numRingSlots - 1; i < n; i++)
		{
			if ((occupied & (1 << i)) == 0x0)
			{
				return ringSlots[i];
			}
		}
	}

	return nextRingSlot;
}

void Rings::AttachRingModel(Actor* actor, TESBoundObject* ring, int index, bool firstPerson) const
{
	BSFadeNode* actor3D = static_cast<BSFadeNode*>(actor->Get3D1(firstPerson));
	if (!actor3D)
	{
		return;
	}

	NiNode* fingerNode = static_cast<NiNode*>(actor3D->GetObjectByName(fingerBones[index]));
	if (!fingerNode)
	{
		return;
	}

	const BSFixedString& ringNodeName = firstPerson ? firstPersonRingNodeNames[index] : ringNodeNames[index];

	BSFadeNode* oldRingNode = static_cast<BSFadeNode*>(fingerNode->GetObjectByName(ringNodeName));
	if (oldRingNode)
	{
		if (RingNodeEquals(oldRingNode, static_cast<NiNode*>(ring->LoadGraphics(nullptr))))
		{
			// This ring model is already attached.
			return;
		}
		else
		{
			// Detach old ring model.
			fingerNode->DetachChild(oldRingNode);
		}
	}

	BSFadeNode* ringNode = ring->Clone3D(nullptr, false)->AsFadeNode();
	if (ringNode)
	{
		ringNode->name = ringNodeName;
		ringNode->local.translate.y = -0.22f;
		ringNode->local.translate.z = 0.813f;
		fingerNode->AttachChild(ringNode);
	}
}
void Rings::DetachRingModel(Actor* actor, int index, bool firstPerson) const
{
	BSFadeNode* actor3D = static_cast<BSFadeNode*>(actor->Get3D1(firstPerson));
	if (!actor3D)
	{
		return;
	}

	NiNode* fingerNode = static_cast<NiNode*>(actor3D->GetObjectByName(fingerBones[index]));
	if (!fingerNode)
	{
		return;
	}

	const BSFixedString& ringNodeName = firstPerson ? firstPersonRingNodeNames[index] : ringNodeNames[index];
	BSFadeNode* ringNode = static_cast<BSFadeNode*>(fingerNode->GetObjectByName(ringNodeName));
	if (!ringNode)
	{
		return;
	}

	fingerNode->DetachChild(ringNode);
}
void Rings::UpdateRingModels(Actor* actor, bool firstPerson) const
{
	if (!actor->Get3D1(firstPerson))
	{
		return;
	}

	if (!actor->currentProcess)
	{
		DetachAllRingModels(actor, firstPerson);
		return;
	}

	BSFadeNode* actor3D = static_cast<BSFadeNode*>(actor->Get3D1(firstPerson));
	if (!actor3D)
	{
		return;
	}

	uint8_t occupied = 0x00;

	for (const auto& data : actor->currentProcess->equippedForms)
	{
		int ringSlotIndex = GetRingSlotIndex(data.slot);
		if (ringSlotIndex >= 0)
		{
			AttachRingModel(actor, static_cast<TESBoundObject*>(data.object), ringSlotIndex, firstPerson);
			occupied |= (1 << ringSlotIndex);
		}
	}

	if (occupied == 0xFF)
	{
		return;
	}

	for (int i = 0; i < numRingSlots; i++)
	{
		if ((occupied & (1 << i)) == 0x0)
		{
			DetachRingModel(actor, i, firstPerson);
		}
	}
}
void Rings::DetachAllRingModels(Actor* actor, bool firstPerson) const
{
	for (int i = 0; i < maxRingSlots; i++)
	{
		DetachRingModel(actor, i, firstPerson);
	}
}

BSSimpleList<ExtraDataList*>* Rings::GetRingExtraLists(Actor * actor, TESForm * ring)
{
	ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(actor->extraList.GetByType(ExtraDataType::kContainerChanges));
	if (!containerChanges)
	{
		return nullptr;
	}

	BSSimpleList<InventoryEntryData*>* containerDataList = containerChanges->changes ? containerChanges->changes->entryList : nullptr;
	if (!containerDataList)
	{
		return nullptr;
	}

	for (auto invEntryData : *containerDataList)
	{
		if (invEntryData->object == ring)
		{
			return invEntryData->extraLists;
		}
	}

	return nullptr;
}

bool Rings::RingNodeEquals(const NiNode* x, const NiNode* y)
{
	return !x->children.empty() && !y->children.empty() && x->children.front()->name == y->children.front()->name;
}