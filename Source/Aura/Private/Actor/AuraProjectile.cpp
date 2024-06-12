// Copyright Druid Mechanics


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Character/AuraCharacterBase.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	if (LoopingSound)
	{
		// Only arguments I actually needed to provide are Sound, AttachToComponent, and bStopWhenAttachedToDestroyed
		// Location, Rotation, and LocationType are just using defaults, had to explicitly declare them to reach bStopWhenAttachedToDestroyed
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(
			LoopingSound,
			GetRootComponent(),
			NAME_None,
			FVector(ForceInit),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true);
	}
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (!DamageEffectSpecHandle.IsValid())
		return;
	
	if (DamageEffectSpecHandle.Data.Get()->GetContext().GetInstigator() == OtherActor)
		return;

	// If collides with basic mesh
	if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(OtherActor))
	{
		ECollisionChannel ActorCollisionObjectType = StaticMeshActor->GetStaticMeshComponent()->GetCollisionObjectType();
		if (ActorCollisionObjectType == ECC_WorldStatic || ActorCollisionObjectType == ECC_WorldDynamic)
		{
			ProjectileImpactEffects();
			Destroy();
		}
	}

	// If collides with bp actor mesh
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	OtherActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
	if (!StaticMeshComponents.IsEmpty())
	{
		UStaticMeshComponent* StaticMeshComponent = StaticMeshComponents.Top();
		ECollisionChannel ActorCollisionObjectType = StaticMeshComponent->GetCollisionObjectType();
		if (ActorCollisionObjectType == ECC_WorldStatic || ActorCollisionObjectType == ECC_WorldDynamic)
		{
			ProjectileImpactEffects();
			Destroy();
		}
	}
	
	/*
	ProjectileImpactEffects();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			AActor* TargetActor = TargetASC->GetAvatarActor();
			if (UAuraAbilitySystemLibrary::AreOpposingFactions(this, TargetActor))
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			
				Destroy();
			}
		}
	}
	else
		bHit = true;
	*/


	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		AActor* TargetActor = TargetASC->GetAvatarActor();
		if (UAuraAbilitySystemLibrary::AreOpposingFactions(this, TargetActor))
		{
			ProjectileImpactEffects();
			if (HasAuthority())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			
				Destroy();
			}
		}
	}
}

void AAuraProjectile::Destroyed()
{
	/*
	if (!bHit && !HasAuthority())
	*/
	//ProjectileImpactEffects();
	
	Super::Destroyed();
}

void AAuraProjectile::ProjectileImpactEffects_Implementation()
{
	if (!bHit)
	{
		bHit = true;
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}
}