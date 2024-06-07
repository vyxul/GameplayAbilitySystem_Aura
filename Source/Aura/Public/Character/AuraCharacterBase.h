// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();

	/* IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* end IAbilitySystemInterface */
	
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

	/* ICombatInterface */
	virtual FVector GetCombatInterfaceLocation_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	/* end ICombatInterface */
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	UPROPERTY(EditAnywhere, Category= "Combat")
	TArray<FTaggedMontage> AttackMontages;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;

	bool bDead = false;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	virtual void InitAbilityActorInfo();

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;
	
	// Implemented in child classes (AuraCharacter and AuraEnemy)
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	// Dissolve Effects
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Combat")
	TObjectPtr<USoundBase> DeathSound; 
	
private:
	UPROPERTY(EditAnywhere, Category= "Abilties")
	TArray<TSubclassOf<UGameplayAbility>> StartUpAbilities;

	UPROPERTY(EditAnywhere, Category= "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
