#pragma once

#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilityDebuffStruct
{
	GENERATED_BODY()

	FAbilityDebuffStruct() = default;
	FAbilityDebuffStruct(const FScalableFloat& DebuffChance, const FScalableFloat& DebuffLevel,
						 const TSubclassOf<UGameplayEffect>& DebuffGameplayEffect)
		: DebuffChance(DebuffChance),
		  DebuffLevel(DebuffLevel),
		  DebuffGameplayEffect(DebuffGameplayEffect)
	{
	}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat DebuffChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat DebuffLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DebuffGameplayEffect;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() {}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC;
	
	UPROPERTY()
	float AbilityLevel = 1.f;
	
	UPROPERTY()
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	UPROPERTY()
	TArray<FAbilityDebuffStruct> AbilityDebuffEffects;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	/* Needed Code for subclasses */
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	};

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/* Custom Code for sub classes */
	// Getters
	bool IsBlockedHit () const	{ return bIsBlockedHit;	}
	bool IsCriticalHit() const	{ return bIsCriticalHit; }
	// Setters
	void SetIsBlockedHit (bool bInIsBlockedHit)  { bIsBlockedHit  = bInIsBlockedHit;  }
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};