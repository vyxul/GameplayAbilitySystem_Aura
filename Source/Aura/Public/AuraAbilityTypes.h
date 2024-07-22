#pragma once

#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageTagScalableFloat
{
	GENERATED_BODY()
	
	FDamageTagScalableFloat() = default;

	FDamageTagScalableFloat(const FGameplayTag& DamageTag, const FScalableFloat& DamageScalableFloat)
		: DamageTag(DamageTag),
		  DamageScalableFloat(DamageScalableFloat)
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag DamageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat DamageScalableFloat;
};

USTRUCT(BlueprintType)
struct FStatusEffectInfoArray
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FDamageTagScalableFloat> StatusEffectInfo;
};

USTRUCT(BlueprintType)
struct FAbilityStatusEffectStruct
{
	GENERATED_BODY()

	FAbilityStatusEffectStruct() = default;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat StatusEffectChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat StatusEffectLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag StatusEffectTag;
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
	TArray<FAbilityStatusEffectStruct> AbilityStatusEffects;

	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;
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