
#pragma once

#include "GameplayTagContainer.h"

#define HDTAG_CHARACTER_STATE_STRATAGEMINPUTMODE    FGameplayTag::RequestGameplayTag(FName("Character.State.IsStratagemInputMode"))

#define HDTAG_EVENT_STRATAGEMHUD_APPEAR             FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.Active"))
#define HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR          FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.Deactive"))

#define HDTAG_DATA_DAMAGE_PROJECTILE                FGameplayTag::RequestGameplayTag(FName("Data.Damage.Projectile"))
#define HDTAG_DATA_DOTDAMAGE_TICKDAMAGE             FGameplayTag::RequestGameplayTag(FName("Data.DotDamage.TickDamage"))
#define HDTAG_DATA_DOTDAMAGE_DURATION               FGameplayTag::RequestGameplayTag(FName("Data.Projectile.StatusEffectDuration"))

#define HDTAG_DATA_STATUS_MAXHEALTH                 FGameplayTag::RequestGameplayTag(FName("Data.Status.MaxHealth"))
#define HDTAG_DATA_STATUS_WALKSPEED                 FGameplayTag::RequestGameplayTag(FName("Data.Status.WalkSpeed"))
#define HDTAG_DATA_STATUS_CRAWLINGSPEED             FGameplayTag::RequestGameplayTag(FName("Data.Status.CrawlingSpeed"))
#define HDTAG_DATA_STATUS_CROUCHSPEED               FGameplayTag::RequestGameplayTag(FName("Data.Status.CrouchSpeed"))
#define HDTAG_DATA_STATUS_SPRINTSPEED               FGameplayTag::RequestGameplayTag(FName("Data.Status.SprintSpeed"))
#define HDTAG_DATA_STATUS_MAXSTAMINA                FGameplayTag::RequestGameplayTag(FName("Data.Status.MaxStamina"))

#define HDTAG_DATA_KNOCKBACK_HIT                    FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Hit"))
#define HDTAG_DATA_KNOCKBACK_RAGDOLL                FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Ragdoll"))