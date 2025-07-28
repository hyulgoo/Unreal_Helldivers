
#pragma once

#include "GameplayTagContainer.h"

#define HDTAG_CHARACTER_STATE_STRATAGEMINPUTMODE    FGameplayTag::RequestGameplayTag(FName("Character.State.IsStratagemInputMode"))
#define HDTAG_CHARACTER_STATE_KNOCKBACK				FGameplayTag::RequestGameplayTag(FName("Character.State.Knockback"))
#define HDTAG_CHARACTER_STATE_RAGDOLL				FGameplayTag::RequestGameplayTag(FName("Character.State.Ragdoll"))
#define HDTAG_CHARACTER_STATE_ISAIMING				FGameplayTag::RequestGameplayTag(FName("Character.State.IsAiming"))
#define HDTAG_CHARACTER_STATE_ISFIRING				FGameplayTag::RequestGameplayTag(FName("Character.State.IsFiring"))
#define HDTAG_CHARACTER_STATE_ISSHOULDER			FGameplayTag::RequestGameplayTag(FName("Character.State.IsShoulder"))
#define HDTAG_CHARACTER_STATE_ISSPRINT				FGameplayTag::RequestGameplayTag(FName("Character.State.IsSprint"))
#define HDTAG_CHARACTER_STATE_ISDEAD				FGameplayTag::RequestGameplayTag(FName("Character.State.IsDead"))

#define HDTAG_INPUT							        FGameplayTag::RequestGameplayTag(FName("Input"))

#define HDTAG_TRIGGER_AIMING						FGameplayTag::RequestGameplayTag(FName("Trigger.Aiming"))
#define HDTAG_TRIGGER_SHOULDER						FGameplayTag::RequestGameplayTag(FName("Trigger.Shoulder"))
#define HDTAG_TRIGGER_SPRINT						FGameplayTag::RequestGameplayTag(FName("Trigger.Sprint"))
#define HDTAG_TRIGGER_CROUCH						FGameplayTag::RequestGameplayTag(FName("Trigger.Crouch"))
#define HDTAG_TRIGGER_PRONE							FGameplayTag::RequestGameplayTag(FName("Trigger.Prone"))
#define HDTAG_TRIGGER_ATTACK						FGameplayTag::RequestGameplayTag(FName("Trigger.Attack"))
#define HDTAG_TRIGGER_RELOAD						FGameplayTag::RequestGameplayTag(FName("Trigger.Reload"))
#define HDTAG_TRIGGER_REGENSTAMINA                  FGameplayTag::RequestGameplayTag(FName("Trigger.RegenStamina"))
#define HDTAG_TRIGGER_STRATAGEMINPUTMODE			FGameplayTag::RequestGameplayTag(FName("Trigger.StratagemInputMode"))

#define HDTAG_EVENT_STRATAGEMHUD_APPEAR             FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.Active"))
#define HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR          FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.Deactive"))
#define HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND         FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.AddCommand"))
#define HDTAG_EVENT_PLAYERHUD_AMMOCHANGE			FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.AmmoChange"))
#define HDTAG_EVENT_PLAYERHUD_CAPACITYCHANGE		FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.CapacityChange"))
#define HDTAG_EVENT_PLAYERHUD_HEALTHCHANGE			FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.HealthChange"))

#define HDTAG_DATA_DAMAGE_PROJECTILE                FGameplayTag::RequestGameplayTag(FName("Data.Damage.Projectile"))
#define HDTAG_DATA_DOTDAMAGE_TICKDAMAGE             FGameplayTag::RequestGameplayTag(FName("Data.DotDamage.TickDamage"))
#define HDTAG_DATA_DOTDAMAGE_DURATION               FGameplayTag::RequestGameplayTag(FName("Data.Projectile.StatusEffectDuration"))

#define HDTAG_DATA_ATTRIBUTE                        FGameplayTag::RequestGameplayTag(FName("Data.Attribute"))
#define HDTAG_DATA_ATTRIBUTE_CURRENTHEALTH          FGameplayTag::RequestGameplayTag(FName("Data.Attribute.CurrentHealth"))
#define HDTAG_DATA_ATTRIBUTE_MAXHEALTH              FGameplayTag::RequestGameplayTag(FName("Data.Attribute.MaxHealth"))
#define HDTAG_DATA_ATTRIBUTE_WALKSPEED              FGameplayTag::RequestGameplayTag(FName("Data.Attribute.WalkSpeed"))
#define HDTAG_DATA_ATTRIBUTE_CRAWLINGSPEED          FGameplayTag::RequestGameplayTag(FName("Data.Attribute.CrawlingSpeed"))
#define HDTAG_DATA_ATTRIBUTE_CROUCHSPEED            FGameplayTag::RequestGameplayTag(FName("Data.Attribute.CrouchSpeed"))
#define HDTAG_DATA_ATTRIBUTE_SPRINTSPEED            FGameplayTag::RequestGameplayTag(FName("Data.Attribute.SprintSpeed"))
#define HDTAG_DATA_ATTRIBUTE_CURRENTSTAMINA         FGameplayTag::RequestGameplayTag(FName("Data.Attribute.CurrentStamina"))
#define HDTAG_DATA_ATTRIBUTE_MAXSTAMINA             FGameplayTag::RequestGameplayTag(FName("Data.Attribute.MaxStamina"))

#define HDTAG_DATA_KNOCKBACK_HIT                    FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Hit"))
#define HDTAG_DATA_KNOCKBACK_RAGDOLL                FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Ragdoll"))

#define HDTAG_COST_STAMINA                          FGameplayTag::RequestGameplayTag(FName("Cost.Stamina"))
#define HDTAG_REGEN_STAMINA                          FGameplayTag::RequestGameplayTag(FName("Regen.Stamina"))