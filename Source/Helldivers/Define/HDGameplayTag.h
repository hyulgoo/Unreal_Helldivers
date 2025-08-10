
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

#define HDTAG_EVENT_STRATAGEMHUD                    FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD"))
#define HDTAG_EVENT_STRATAGEMHUD_APPEAR             FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.Active"))
#define HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR          FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.Deactive"))
#define HDTAG_EVENT_STRATAGEMHUD_ADDCOMMAND         FGameplayTag::RequestGameplayTag(FName("Event.StratagemHUD.AddCommand"))
#define HDTAG_EVENT_PLAYERHUD		                FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD"))
#define HDTAG_EVENT_PLAYERHUD_CURRENTAMMOCHANGE		FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.CurrentAmmoChange"))
#define HDTAG_EVENT_PLAYERHUD_MAXAMMOCHANGE			FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.MaxAmmoChange"))
#define HDTAG_EVENT_PLAYERHUD_CURRENTCAPACITYCHANGE	FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.CurrentCapacityChange"))
#define HDTAG_EVENT_PLAYERHUD_MAXCAPACITYCHANGE		FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.MaxCapacityChange"))
#define HDTAG_EVENT_PLAYERHUD_INITIALIZE		    FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.Initialize"))
#define HDTAG_EVENT_PLAYERHUD_EQUIPWEAPON			FGameplayTag::RequestGameplayTag(FName("Event.PlayerHUD.EquipWeapon"))
#define HDTAG_EVENT_SPAWN_PROJECTILE                FGameplayTag::RequestGameplayTag(FName("Event.Spawn.Projectile"))

#define HDTAG_DATA_DAMAGE_PROJECTILE                FGameplayTag::RequestGameplayTag(FName("Data.Damage.Projectile"))
#define HDTAG_DATA_DOTDAMAGE_TICKDAMAGE             FGameplayTag::RequestGameplayTag(FName("Data.DotDamage.TickDamage"))
#define HDTAG_DATA_DOTDAMAGE_DURATION               FGameplayTag::RequestGameplayTag(FName("Data.Projectile.StatusEffectDuration"))

#define HDTAG_DATA_ATTRIBUTE                        FGameplayTag::RequestGameplayTag(FName("Data.Attribute"))

#define HDTAG_DATA_KNOCKBACK_HIT                    FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Hit"))
#define HDTAG_DATA_KNOCKBACK_RAGDOLL                FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Ragdoll"))

#define HDTAG_COST_STAMINA                          FGameplayTag::RequestGameplayTag(FName("Cost.Stamina"))
#define HDTAG_REGEN_STAMINA                         FGameplayTag::RequestGameplayTag(FName("Regen.Stamina"))