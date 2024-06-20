// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	/* End Combat Interface */

	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) override;
	virtual AAuraPlayerState* GetAuraPlayerState_Implementation() override;
	virtual UDataTable* GetLevelUpInfo_Implementation() override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void LevelUp_Implementation() override;
	virtual FOnExternalGameplayModifierDependencyChange* GetNeedRefreshAttributesDelegate() override;
	/* End Player Interface */

protected:
	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "FX | VFX")
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "FX | SFX")
	TObjectPtr<USoundBase> LevelUpSound;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;
	
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
