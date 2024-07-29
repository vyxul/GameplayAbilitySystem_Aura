// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics", meta = (DisplayName = "Get XP for Class and Level"))
	static int32 GetXPForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

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

	/* Setters */
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | GameplayEffects")
	static void SetKnockback(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockback);
	// -END AuraEffectContextHandle Functions

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Targets, TArray<AActor*>& OutClosestTargets, const FVector& Origin);
	
	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static bool AreOpposingFactions(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary | Gameplay Mechanics")
	static FGameplayEffectContextHandle ApplyAbilityEffect(FDamageEffectParams DamageEffectParams);

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
};
