
#pragma once

#include "GameplayTagContainer.h"

#define HDTAG_CHARACTER_STATE_STRATAGEMINPUTMODE    FGameplayTag::RequestGameplayTag(FName("Character.State.IsStratagemInputMode"))
#define HDTAG_CHARACTER_STATE_KNOCKBACK				FGameplayTag::RequestGameplayTag(FName("Character.State.Knockback"))
#define HDTAG_CHARACTER_STATE_RAGDOLL				FGameplayTag::RequestGameplayTag(FName("Character.State.Ragdoll"))
#define HDTAG_CHARACTER_STATE_ISAIMING				FGameplayTag::RequestGameplayTag(FName("Character.State.IsAiming"))
#define HDTAG_CHARACTER_STATE_ISFIRING				FGameplayTag::RequestGameplayTag(FName("Character.State.IsFiring"))
#define HDTAG_CHARACTER_STATE_ISSHOULDER			FGameplayTag::RequestGameplayTag(FName("Character.State.IsShoulder"))
#define HDTAG_CHARACTER_STATE_ISSPRINT				FGameplayTag::RequestGameplayTag(FName("Character.State.IsSprint"))

#define HDTAG_INPUT_AIMING							FGameplayTag::RequestGameplayTag(FName("Input.Aiming"))
#define HDTAG_INPUT_SHOULDER						FGameplayTag::RequestGameplayTag(FName("Input.Shoulder"))
#define HDTAG_INPUT_SPRINT							FGameplayTag::RequestGameplayTag(FName("Input.Sprint"))
#define HDTAG_INPUT_CROUCH							FGameplayTag::RequestGameplayTag(FName("Input.Crouch"))
#define HDTAG_INPUT_PRONE							FGameplayTag::RequestGameplayTag(FName("Input.Prone"))

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