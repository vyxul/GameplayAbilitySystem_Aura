// Copyright Druid Mechanics


#include "Character/AuraEnemy.h"
#include <Aura/Aura.h>
#include <AbilitySystem/AuraAbilitySystemComponent.h>
#include <AbilitySystem/AuraAttributeSet.h>

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();

	// controlling enemy facing direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	// Set up Health Bar Widget
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	// Defaults
	BaseWalkSpeed = 250.f;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
		return;
	
	AuraAIController = Cast<AAuraAIController>(NewController);

	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	if (CharacterClass != ECharacterClass::Warrior)
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), true);
}

void AAuraEnemy::HighlightActor_Implementation()
{
	// highlight enemy mesh
	GetMesh()->SetRenderCustomDepth(true);

	// highlight enemy weapon mesh
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	// unhighlight enemy mesh
	GetMesh()->SetRenderCustomDepth(false);

	// unhighlight enemy weapon mesh
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	// Do not change OutDestination
}

int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

ECharacterClass AAuraEnemy::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (AuraAIController)
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);

	SpawnLoot();
	Super::Die(DeathImpulse);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

int32 AAuraEnemy::GetCharacterLevel() const
{
	return Level;
}

void AAuraEnemy::SetCharacterLevel(const int32 InLevel)
{
	this->Level = InLevel;
}

ECharacterClass AAuraEnemy::GetCharacterClass() const
{
	return CharacterClass;
}

void AAuraEnemy::SetCharacterClass(const ECharacterClass InCharacterClass)
{
	this->CharacterClass = InCharacterClass;
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	if (HasAuthority())
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);

	// Set WidgetController for the Health Bar
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
		AuraUserWidget->SetWidgetController(this);
	
	// Handle AS
	if (const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet))
	{
		/* Bind AS Delegates to Callbacks */
		// Bind AS Health Attribute
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute())
			.AddLambda(
				[this](const FOnAttributeChangeData& Data)
				{
					OnHealthChanged.Broadcast(Data.NewValue);
				}
			);

		// Bind AS MaxHealth Attribute
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute())
			.AddLambda(
				[this](const FOnAttributeChangeData& Data)
				{
					OnMaxHealthChanged.Broadcast(Data.NewValue);
				}
			);

		// Bind tag change for Event.HitReact
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AAuraEnemy::HitReactTagChanged
		);
		
		// Broadcast Initial Values
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	}
	
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;

	// Stop character movement
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
		InitializeDefaultAttributes();

	OnAscRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().StatusEffect_Debuff_Lightning_Stun,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AAuraEnemy::StunTagChanged);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
