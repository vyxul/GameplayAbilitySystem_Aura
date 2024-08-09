// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UPassiveNiagaraComponent;
class UStatusEffectNiagaraComponent;
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* end IAbilitySystemInterface */
	
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

	/* ICombatInterface */
	virtual FVector GetCombatInterfaceLocation_Implementation() override;
	virtual USkeletalMeshComponent* GetMeshComponent_Implementation() override;
	virtual USkeletalMeshComponent* GetWeaponMeshComponent_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void SetMinionCount_Implementation(const int32 Amount) override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual FOnDamageSignature& GetOnDamageSignature() override;
	virtual void SetBeingShocked_Implementation(bool InBeingShocked) override;
	virtual bool IsBeingShocked_Implementation() override;
	/* end ICombatInterface */

	FOnASCRegistered OnAscRegistered;
	FOnDeath OnDeath;
	FOnDamageSignature OnDamageDelegate;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, Category= "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(Replicated, BlueprintReadOnly, Category= "Combat")
	bool bIsStunned = false;

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Combat")
	float BaseWalkSpeed = 600.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category= "Combat")
	bool bBeingShocked = false;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
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

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

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

	/* Minions */
	
	int32 MinionCount = 0;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStatusEffectNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStatusEffectNiagaraComponent> StunDebuffComponent;
	
private:
	UPROPERTY(EditAnywhere, Category= "Abilties")
	TArray<TSubclassOf<UGameplayAbility>> StartUpAbilities;
	
	UPROPERTY(EditAnywhere, Category= "Abilties")
	TArray<TSubclassOf<UGameplayAbility>> StartUpPassiveAbilities;

	UPROPERTY(EditAnywhere, Category= "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(VisibleAnywhere, Category= "Combat")
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, Category= "Combat")
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category= "Combat")
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category= "Combat")
	TObjectPtr<USceneComponent> EffectAttachComponent;
};
