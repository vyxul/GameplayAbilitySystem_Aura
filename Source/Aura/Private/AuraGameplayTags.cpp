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
	/* DEFINE_NATIVE_GAMEPLAY_TAG(CppTagName, BpTagName, Description) */
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

	/* Resistances */
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Resistance_Arcane, Attributes.Resistance.Arcane, "Arcane Damage Type Resistance")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Resistance_Fire, Attributes.Resistance.Fire, "Fire Damage Type Resistance")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Resistance_Lightning, Attributes.Resistance.Lightning, "Lightning Damage Type Resistance")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Resistance_Physical, Attributes.Resistance.Physical, "Physical Damage Type Resistance")

	/* Vital Attributes */
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Vital_Health, Attributes.Vital.Health, "Current HP")
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Vital_Mana, Attributes.Vital.Mana, "Current MP")
	
	/* Meta Attributes */
	DEFINE_NATIVE_GAMEPLAY_TAG(Attributes_Meta_IncomingXP, Attributes.Meta.IncomingXP, "Current XP")
	
	/* Input Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_LMB, InputTag.LMB, "Input tag for Left Mouse Button")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_RMB, InputTag.RMB, "Input tag for Right Mouse Button")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_1, InputTag.1, "Input tag for 1 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_2, InputTag.2, "Input tag for 2 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_3, InputTag.3, "Input tag for 3 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_4, InputTag.4, "Input tag for 4 key")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_Passive_1, InputTag.Passive.1, "Input tag for Passive 1")
	DEFINE_NATIVE_GAMEPLAY_TAG(InputTag_Passive_2, InputTag.Passive.2, "Input tag for Passive 2")

	/* Damage Types */
	DEFINE_NATIVE_GAMEPLAY_TAG(Damage, Damage, "Damage")
	DEFINE_NATIVE_GAMEPLAY_TAG(Damage_Arcane, Damage.Arcane, "Arcane Damage Type")
	DEFINE_NATIVE_GAMEPLAY_TAG(Damage_Fire, Damage.Fire, "Fire Damage Type")
	DEFINE_NATIVE_GAMEPLAY_TAG(Damage_Lightning, Damage.Lightning, "Lightning Damage Type")
	DEFINE_NATIVE_GAMEPLAY_TAG(Damage_Physical, Damage.Physical, "Physical Damage Type")

	/* Map of Damage Types to Resistances */
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/* Debuffs */
	// Meta
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Meta_Chance, StatusEffect.Debuff.Meta.Chance, "Chance to apply debuff")
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Meta_Damage, StatusEffect.Debuff.Meta.Damage, "Damage of the applied debuff")
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Meta_Frequency, StatusEffect.Debuff.Meta.Frequency, "Frequency that the debuff applies effect")
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Meta_Duration, StatusEffect.Debuff.Meta.Duration, "Duration that the debuff is applied to the target")
	// Effects
	// Fire
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Fire, StatusEffect.Debuff.Fire, "General tag for Fire type debuffs")
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Fire_Burn, StatusEffect.Debuff.Fire.Burn, "Burn debuff tag")
	// Lightning
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Lightning, StatusEffect.Debuff.Lightning, "General tag for Lightning type debuffs")
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Lightning_Stun, StatusEffect.Debuff.Lightning.Stun, "Stun debuff tag")
	// Arcane
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Arcane, StatusEffect.Debuff.Arcane, "General tag for Arcane type debuffs")
	// Physical
	DEFINE_NATIVE_GAMEPLAY_TAG(StatusEffect_Debuff_Physical, StatusEffect.Debuff.Physical, "General tag for Physical type debuffs")
	
	
	/* Tag to apply to targets */
	DEFINE_NATIVE_GAMEPLAY_TAG(Effects_HitReact, Effect.HitReact, "Tag granted when Hit Reacting")
	
	/* Ability Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_None, Abilities.None, "Default/No Ability Tag - used similar as nullptr")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Attack, Abilities.Attack, "Attack Ability Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Summon, Abilities.Summon, "Summon Ability Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_HitReact, Abilities.HitReact, "HitReact Ability Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Fire, Abilities.Fire, "Fire Abilities Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Fire_FireBolt, Abilities.Fire.FireBolt, "FireBolt Ability Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Fire_FireBlast, Abilities.Fire.FireBlast, "FireBlast Ability Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Lightning, Abilities.Lightning, "Lightning Abilities Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Lightning_Electrocute, Abilities.Lightning.Electrocute, "Electrocute Ability Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Arcane, Abilities.Arcane, "Arcane Abilities Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Arcane_ArcaneShards, Abilities.Arcane.ArcaneShards, "Arcane Shards Ability Tag")

	/* Passive Ability Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Passive_HaloOfProtection, Abilities.Passive.HaloOfProtection, "Halo of Protection")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Passive_LifeSiphon, Abilities.Passive.LifeSiphon, "Life Siphon")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Passive_ManaSiphon, Abilities.Passive.ManaSiphon, "Mana Siphon")

	/* Ability Status */
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Status_Locked, Abilities.Status.Locked, "Locked Abilities")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Status_Eligible, Abilities.Status.Eligible, "Eligible Abilities")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Status_Unlocked, Abilities.Status.Unlocked, "Unlocked Abilities")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Status_Equipped, Abilities.Status.Equipped, "Equipped Abilities")
	
	/* Ability Type */
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Type_Offensive, Abilities.Type.Offensive, "Offensive Abilities")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Type_Passive, Abilities.Type.Passive, "Passive Abilities")
	DEFINE_NATIVE_GAMEPLAY_TAG(Abilities_Type_None, Abilities.Type.None, "Non-Typed Abilities")

	/* Cooldown Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(Cooldown_Fire_FireBolt, Cooldown.Fire.FireBolt, "FireBolt Cooldown Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Cooldown_Fire_FireBlast, Cooldown.Fire.FireBlast, "FireBlast Cooldown Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Cooldown_Lightning_Electrocute, Cooldown.Lightning.Electrocute, "Electrocute Cooldown Tag")
	DEFINE_NATIVE_GAMEPLAY_TAG(Cooldown_Arcane_ArcaneShards, Cooldown.Arcane.ArcaneShards, "Arcane Shards Cooldown Tag")
	
	/* Combat Socket Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(Combat_Socket_Weapon,    Combat.Socket.Weapon,    "Weapon")
	DEFINE_NATIVE_GAMEPLAY_TAG(Combat_Socket_RightHand, Combat.Socket.RightHand, "Right Hand")
	DEFINE_NATIVE_GAMEPLAY_TAG(Combat_Socket_LeftHand,  Combat.Socket.LeftHand,  "Left Hand")
	DEFINE_NATIVE_GAMEPLAY_TAG(Combat_Socket_Tail,	    Combat.Socket.Tail,      "Tail")

	/* Montage Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(Montage_Attack_1, Montage.Attack.1, "Attack 1")
	DEFINE_NATIVE_GAMEPLAY_TAG(Montage_Attack_2, Montage.Attack.2, "Attack 2")
	DEFINE_NATIVE_GAMEPLAY_TAG(Montage_Attack_3, Montage.Attack.3, "Attack 3")
	DEFINE_NATIVE_GAMEPLAY_TAG(Montage_Attack_4, Montage.Attack.4, "Attack 4")

	/* Player Tags */
	DEFINE_NATIVE_GAMEPLAY_TAG(Player_Block_InputPressed,  Player.Block.InputPressed,  "Block Input Pressed callback for input")
	DEFINE_NATIVE_GAMEPLAY_TAG(Player_Block_InputHeld,     Player.Block.InputHeld,     "Block Input Held callback for input")
	DEFINE_NATIVE_GAMEPLAY_TAG(Player_Block_InputReleased, Player.Block.InputReleased, "Block Input Released callback for input")
	DEFINE_NATIVE_GAMEPLAY_TAG(Player_Block_CursorTrace,   Player.Block.CursorTrace,   "Block tracing under the cursor")

	/* Gameplay Cues */
	DEFINE_NATIVE_GAMEPLAY_TAG(GameplayCue_FireBlast_Explosion, GameplayCue.FireBlast.Explosion, "FireBlast Explosion GameplayCue")
}
