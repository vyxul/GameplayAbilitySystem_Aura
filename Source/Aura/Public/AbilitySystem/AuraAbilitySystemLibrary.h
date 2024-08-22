// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAuraSaveGame;
struct FGameplayTag;
struct FGameplayEffectContext;
struct FDamageEffectParams;
struct FGameplayEffectContextHandle;
class UAbilityInfo;
struct FWidgetControllerParams;
class USpellMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Widget Controller */
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	/* END Widget Controller */

	/* Ability System Component */
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static void InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, UAuraSaveGame* SaveGame);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);
	/* END Ability System Component */

	/* AuraEffectContextHandle Functions */
	/* Getters */
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static FVector GetKnockback(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	/* Setters */
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetKnockback(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockback);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsRadialDamage);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InRadialDamageInnerRadius);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float InRadialDamageOuterRadius);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin);
	/* END  AuraEffectContextHandle Functions */

	/* Damage Effect Params */
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static FGameplayEffectContextHandle ApplyAbilityEffect(FDamageEffectParams DamageEffectParams);
	
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void SetDamageTypesParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams, TMap<FGameplayTag, FScalableFloat> DamageTypes);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void SetRadialParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin);
	
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void SetKnockbackParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams, float KnockbackChance, float KnockbackMagnitude, FVector KnockbackDirection);
	
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void SetDeathImpulseParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams, float DeathImpulseMagnitude, FVector DeathImpulseDirection);

	/* END Damage Effect Params */

	/* Generic */
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics", meta = (DisplayName = "Get XP for Class and Level"))
	static int32 GetXPForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Targets, TArray<AActor*>& OutClosestTargets, const FVector& Origin, AActor* SourceActor);
	
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static bool AreOpposingFactions(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static float GetScalableFloatValueAtLevel(FScalableFloat ScalableFloat, float Level);

	// Made custom version in AuraProjectileSpell.h called GetFanSpread() and GetShotgunSpread() using FVectorSpread struct.
	// Making this just in case if being used elsewhere
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators);

	// Made custom version in AuraProjectileSpell.h called GetFanSpread() and GetShotgunSpread() using FVectorSpread struct.
	// Making this just in case if being used elsewhere
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static TArray<FVector> EvenlySpacedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors);
	/* END Generic */
};
