// Copyright Druid Mechanics


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

#define DEFINE_NATIVE_GAMEPLAY_TAG(TagName, Tag, DevComment) GameplayTags.TagName = \
	UGameplayTagsManager::Get().AddNativeGameplayTag(\
		FName(#Tag),\
		FString(DevComment)\
	);

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/* Primary Attributes */
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Primary_Strength, Attributes.Primary.Strength, "Increases physical damage")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Primary_Intelligence, Attributes.Primary.Intelligence, "Increases magical damage")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Primary_Resilience, Attributes.Primary.Resilience, "Increases Armor and Armor Penetration")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Primary_Vigor, Attributes.Primary.Vigor, "Increases health")

	/* Secondary Attributes */
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_Armor, Attributes.Secondary.Armor, "Reduces damage taken, improves block chance")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_ArmorPenetration, Attributes.Secondary.ArmorPenetration, "Ignored percentage of enemy armor, increases critical hit chance");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_BlockChance, Attributes.Secondary.BlockChance, "Chance to cut incoming damage by half");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_CriticalHitChance, Attributes.Secondary.CriticalHitChance, "Chance to double damage lus critical hit bonus");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_CriticalHitDamage, Attributes.Secondary.CriticalHitDamage, "Bonus damage added when a critical hit is scored");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_CriticalHitResistance, Attributes.Secondary.CriticalHitResistance, "Reduces critical hit chance of attacking enemies");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_HealthRegeneration, Attributes.Secondary.HealthRegeneration, "Amount of health regenerated every second");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_ManaRegeneration, Attributes.Secondary.ManaRegeneration, "Amount of mana regenerated every second");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_MaxHealth, Attributes.Secondary.MaxHealth, "Maximum amount of health obtainable");
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Secondary_MaxMana, Attributes.Secondary.MaxMana, "Maximum amount of mana obtainable")

	/* Input Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_LMB, InputTag.LMB, "Input tag for Left Mouse Button")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_RMB, InputTag.RMB, "Input tag for Right Mouse Button")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_1, InputTag.1, "Input tag for 1 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_2, InputTag.2, "Input tag for 2 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_3, InputTag.3, "Input tag for 3 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_4, InputTag.4, "Input tag for 4 key")

	/* Tag used for damage */
	DEFINE_NATIVE_GAMEPLAY_TAG(Damage, Damage, "Damage")

	/* Tag to apply to targets */
	DEFINE_NATIVE_GAMEPLAY_TAG(Effects_HitReact, Effect.HitReact, "Tag granted when Hit Reacting")
}
