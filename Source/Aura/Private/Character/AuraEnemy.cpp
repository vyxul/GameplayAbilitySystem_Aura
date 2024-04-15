// Copyright Druid Mechanics


#include "Character/AuraEnemy.h"
#include <Aura/Aura.h>

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AAuraEnemy::HighlightActor()
{
	// highlight enemy mesh
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	// highlight enemy weapon mesh
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	// unhighlight enemy mesh
	GetMesh()->SetRenderCustomDepth(false);

	// unhighlight enemy weapon mesh
	Weapon->SetRenderCustomDepth(false);
}
