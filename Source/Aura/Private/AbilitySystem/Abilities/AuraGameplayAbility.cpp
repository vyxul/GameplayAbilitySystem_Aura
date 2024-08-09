// Copyright Druid Mechanics


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

FStringFormatNamedArguments UAuraGameplayAbility::GetAbilityDescriptionFormatter(UGameplayAbility* GameplayAbility, int32 Level)
{
	FStringFormatNamedArguments NamesToValues;
	FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
	
	/* Add connections from names to values here
	 * Using convention of {Name} for the names to be replaced in string to format
	 * Can use just Name as the String key but must be surrounded by {} in the AbilityInfo Description
	 */

	// Level
	NamesToValues.Add(TEXT("Level"), FStringFormatArg(Level));

	// Cost and CD
	if (UAuraGameplayAbility* AuraGA = Cast<UAuraGameplayAbility>(GameplayAbility))
	{
		NamesToValues.Add("ManaCost",
			FStringFormatArg(FMath::RoundToInt32(AuraGA->GetManaCost(Level))));
		NamesToValues.Add("Cooldown",
			FStringFormatArg(FString::SanitizeFloat(AuraGA->GetCooldown(Level))));
	}
	
	// Damage
	if (UAuraDamageGameplayAbility* AuraDGA = Cast<UAuraDamageGameplayAbility>(GameplayAbility))
	{
		NamesToValues.Add("ArcaneDmg",
			FStringFormatArg(FString::SanitizeFloat(FMath::RoundToFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Arcane)))));
		NamesToValues.Add("FireDmg",
			FStringFormatArg(FString::SanitizeFloat(FMath::RoundToFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Fire)))));
		NamesToValues.Add("LightningDmg",
			FStringFormatArg(FString::SanitizeFloat(FMath::RoundToFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Lightning)))));
		NamesToValues.Add("PhysicalDmg",
			FStringFormatArg(FString::SanitizeFloat(FMath::RoundToFloat(AuraDGA->GetDamageAtLevel(Level,AuraGameplayTags.Damage_Physical)))));
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

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo ModifierInfo : CostEffect->Modifiers)
		{
			if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}	
	}
	
	return FMath::Abs(ManaCost);
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	
	return Cooldown;
}
