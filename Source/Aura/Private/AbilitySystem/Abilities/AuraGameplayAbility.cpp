// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"

FStringFormatNamedArguments UAuraGameplayAbility::GetAbilityDescriptionFormatter(UGameplayAbility* GameplayAbility, int32 Level)
{
	FStringFormatNamedArguments NamesToValues;
	FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
	
	/* Add connections from names to values here
	 * Using convention of {Name} for the names to be replaced in string to format
	 * Can use just Name as the String key but must be surrounded by {} in the AbilityInfo Description
	 */
	
	NamesToValues.Add(TEXT("Level"), FStringFormatArg(Level));

	// Cost and CD

	if (UAuraDamageGameplayAbility* AuraDGA = Cast<UAuraDamageGameplayAbility>(GameplayAbility))
	{
		NamesToValues.Add("ArcaneDmg",
			FStringFormatArg(FString::SanitizeFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Arcane))));
		NamesToValues.Add("FireDmg",
			FStringFormatArg(FString::SanitizeFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Fire))));
		NamesToValues.Add("LightningDmg",
			FStringFormatArg(FString::SanitizeFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Lightning))));
		NamesToValues.Add("PhysicalDmg",
			FStringFormatArg(FString::SanitizeFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Physical))));
	}
	
	
	return NamesToValues;
}

FString UAuraGameplayAbility::GetFormattedDescription(UGameplayAbility* GameplayAbility, int32 Level, FString LiteralDescription)
{
	const FStringFormatNamedArguments Formatter = GetAbilityDescriptionFormatter(GameplayAbility, Level);
	FString FormattedDescription = FString::Format(*LiteralDescription, Formatter);
	return FormattedDescription;
}

FString UAuraGameplayAbility::GetDescription_Implementation(UAbilityInfo* AbilityInfo, int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - LoremIpsum ", Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell locked until level %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel)
{
	return 0;
}

float UAuraGameplayAbility::GetCooldown(float InLevel)
{
	return 0;
}
