// Copyright Druid Mechanics


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Public/AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set up capsule component to ignore camera and not generate OnOverlapEvents (to prevent overlap events from duplicating)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	// Set up mesh component to ignore camera, overlap with custom projectile type, and to generate overlap events
	// Just a design decision to generate overlap events here instead of capsule component
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// Set up weapon component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FVector AAuraCharacterBase::GetCombatInterfaceLocation_Implementation()
{
	return GetActorLocation();
}

USkeletalMeshComponent* AAuraCharacterBase::GetMeshComponent_Implementation()
{
	return GetMesh();
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeaponMeshComponent_Implementation()
{
	return Weapon;
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (SocketTag.MatchesTagExact(GameplayTags.Combat_Socket_Weapon) && IsValid(Weapon))
		return Weapon->GetSocketLocation(WeaponTipSocketName);

	if (SocketTag.MatchesTagExact(GameplayTags.Combat_Socket_LeftHand))
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	
	if (SocketTag.MatchesTagExact(GameplayTags.Combat_Socket_RightHand))
		return GetMesh()->GetSocketLocation(RightHandSocketName);

	if (SocketTag.MatchesTagExact(GameplayTags.Combat_Socket_Tail))
		return GetMesh()->GetSocketLocation(TailSocketName);

	return FVector();
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// Some things have to be handled in both server and client, so we do it all in the multicast
	MulticastHandleDeath();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag.MatchesTagExact(MontageTag))
			return TaggedMontage;
	}
	
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::SetMinionCount_Implementation(const int32 Amount)
{
	MinionCount = Amount;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	/* Handle Collision and Weight Settings */
	// Weapon
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// Capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Death FX */
	// VFX
	Dissolve();
	// SFX
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	// Death State
	bDead = true;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	
	if(!HasAuthority())
		return;

	AuraASC->AddCharacterAbilities(StartUpAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartUpPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	// Character Mesh
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}

	// Weapon Mesh
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}
