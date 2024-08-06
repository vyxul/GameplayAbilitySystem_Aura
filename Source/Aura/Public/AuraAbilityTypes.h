#pragma once

#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FVectorSpread
{
	GENERATED_BODY()
	
	FVectorSpread()
	{
		TargetVectorDirection = FVector();
		AngleSpread = 0.f;
		LeftEdgeVectorDirection = FVector();
		RightEdgeVectorDirection = FVector();
		ProjectileCount = 0;
	};

	FVectorSpread(const FVector& TargetVectorDirection, float AngleSpread, const FVector& LeftEdgeVectorDirection,
		const FVector& RightEdgeVectorDirection, int32 ProjectileCount, const TArray<FVector>& VectorDirectionGroup)
		: TargetVectorDirection(TargetVectorDirection),
		  AngleSpread(AngleSpread),
		  LeftEdgeVectorDirection(LeftEdgeVectorDirection),
		  RightEdgeVectorDirection(RightEdgeVectorDirection),
		  ProjectileCount(ProjectileCount),
		  VectorDirectionGroup(VectorDirectionGroup)
	{
	}

	/* Meta Info about vector spread */
	// Main vector to base spread off of
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector TargetVectorDirection = FVector();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AngleSpread = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector LeftEdgeVectorDirection = FVector();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector RightEdgeVectorDirection = FVector();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ProjectileCount = 0;

	/* Actual Info about the projectiles */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FVector> VectorDirectionGroup;
};

USTRUCT(BlueprintType)
struct FKnockbackStruct
{
	GENERATED_BODY()

	FKnockbackStruct() = default;

	FKnockbackStruct(const FScalableFloat& KnockbackChance, const FScalableFloat& KnockbackPower)
		: KnockbackChance(KnockbackChance),
		  KnockbackMagnitude(KnockbackPower)
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat KnockbackChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat KnockbackMagnitude;
};

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

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC;
	
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FAbilityStatusEffectStruct> AbilityStatusEffects;

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulseDirection = FVector::Zero();

	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockbackMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackDirection = FVector::Zero();
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
	FVector GetDeathImpulse() const { return DeathImpulse; }
	FVector GetKnockback() const { return Knockback; }
	// Setters
	void SetIsBlockedHit (bool bInIsBlockedHit)  { bIsBlockedHit  = bInIsBlockedHit;  }
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetDeathImpulse(const FVector& InDeathImpulse) { DeathImpulse = InDeathImpulse; }
	void SetKnockback(const FVector& InKnockback) { Knockback = InKnockback; }
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	FVector DeathImpulse = FVector::Zero();

	UPROPERTY()
	FVector Knockback = FVector::Zero();
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