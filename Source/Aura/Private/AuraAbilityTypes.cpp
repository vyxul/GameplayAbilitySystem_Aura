#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	// Get the information that needs to be saved
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
			RepBits |= 1 << 0;

		if (bReplicateEffectCauser && EffectCauser.IsValid())
			RepBits |= 1 << 1;

		if (AbilityCDO.IsValid())
			RepBits |= 1 << 2;

		if (bReplicateSourceObject && SourceObject.IsValid())
			RepBits |= 1 << 3;

		if (Actors.Num() > 0)
			RepBits |= 1 << 4;

		if (HitResult.IsValid())
			RepBits |= 1 << 5;

		if (bHasWorldOrigin)
			RepBits |= 1 << 6;

		if (IsBlockedHit())
			RepBits |= 1 << 7;

		if (IsCriticalHit())
			RepBits |= 1 << 8;
	}

	// Pass in the number of bits being used to serialize
	Ar.SerializeBits(&RepBits, 9);

	// Save the data that is currently available based off of the previous save code segment
	if (RepBits & (1 << 0))
		Ar << Instigator;
	
	if (RepBits & (1 << 1))
		Ar << EffectCauser;
	
	if (RepBits & (1 << 2))
		Ar << AbilityCDO;
	
	if (RepBits & (1 << 3))
		Ar << SourceObject;
	
	if (RepBits & (1 << 4))
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
			if (!HitResult.IsValid())
				HitResult = TSharedPtr<FHitResult>(new FHitResult());

		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
		
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
		bHasWorldOrigin = false;
		
	if (RepBits & (1 << 7))
		Ar << bIsBlockedHit;
		
	if (RepBits & (1 << 8))
		Ar << bIsCriticalHit;

	// Deal with loading
	if (Ar.IsLoading())
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorASC
	
	bOutSuccess = true;
	return true;
}
