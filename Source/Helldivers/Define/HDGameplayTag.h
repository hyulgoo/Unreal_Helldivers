
#pragma once

#include "GameplayTagContainer.h"

#define HDTAG_CHARACTER_ACTION_STRATAGEMINPUTMODE   FGameplayTag::RequestGameplayTag(FName("Character.Action.StratagemInputMode"))
#define HDTAG_CHARACTER_ACTION_HOLDSTRATAGEM        FGameplayTag::RequestGameplayTag(FName("Character.Action.HoldStratagem"))
#define HDTAG_CHARACTER_ACTION_DETACHSTRATAGEM      FGameplayTag::RequestGameplayTag(FName("Character.Action.DetachStratagem"))
#define HDTAG_CHARACTER_ACTION_THROWEND             FGameplayTag::RequestGameplayTag(FName("Character.Action.ThrowEnd"))

#define HDTAG_CHARACTER_STATE_DEAD				    FGameplayTag::RequestGameplayTag(FName("Character.State.Dead"))

#define HDTAG_BLOCK_ALL					            FGameplayTag::RequestGameplayTag(FName("Block.All"))
#define HDTAG_BLOCK_MOVE					        FGameplayTag::RequestGameplayTag(FName("Block.Move"))
#define HDTAG_BLOCK_FIRE					        FGameplayTag::RequestGameplayTag(FName("Block.Fire"))
#define HDTAG_BLOCK_RELOAD				            FGameplayTag::RequestGameplayTag(FName("Block.Reload"))
#define HDTAG_BLOCK_GRENADE				            FGameplayTag::RequestGameplayTag(FName("Block.Grenade"))
#define HDTAG_BLOCK_STRATAGEM				        FGameplayTag::RequestGameplayTag(FName("Block.Stratagem"))
#define HDTAG_BLOCK_MINIMAP				            FGameplayTag::RequestGameplayTag(FName("Block.Minimap"))

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
#define HDTAG_TRIGGER_THROWSTRATAGEM			    FGameplayTag::RequestGameplayTag(FName("Trigger.ThrowStratagem"))

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
#define HDTAG_EVENT_RELOAD                          FGameplayTag::RequestGameplayTag(FName("Event.Reload"))

#define HDTAG_DATA_DAMAGE_PROJECTILE                FGameplayTag::RequestGameplayTag(FName("Data.Damage.Projectile"))
#define HDTAG_DATA_DOTDAMAGE_TICKDAMAGE             FGameplayTag::RequestGameplayTag(FName("Data.DotDamage.TickDamage"))
#define HDTAG_DATA_DOTDAMAGE_DURATION               FGameplayTag::RequestGameplayTag(FName("Data.Projectile.StatusEffectDuration"))

#define HDTAG_DATA_ATTRIBUTE                        FGameplayTag::RequestGameplayTag(FName("Data.Attribute"))
#define HDTAG_DATA_ATTRIBUTE_AMMO                   FGameplayTag::RequestGameplayTag(FName("Data.Attribute.Ammo"))
#define HDTAG_DATA_ATTRIBUTE_MAXAMMO                FGameplayTag::RequestGameplayTag(FName("Data.Attribute.MaxAmmo"))
#define HDTAG_DATA_ATTRIBUTE_CAPACITY               FGameplayTag::RequestGameplayTag(FName("Data.Attribute.Capacity"))
#define HDTAG_DATA_ATTRIBUTE_MAXCAPACITY            FGameplayTag::RequestGameplayTag(FName("Data.Attribute.MaxCapacity"))

#define HDTAG_DATA_KNOCKBACK_HIT                    FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Hit"))
#define HDTAG_DATA_KNOCKBACK_RAGDOLL                FGameplayTag::RequestGameplayTag(FName("Data.Knockback.Ragdoll"))

#define HDTAG_COST_STAMINA                          FGameplayTag::RequestGameplayTag(FName("Cost.Stamina"))
#define HDTAG_REGEN_STAMINA                         FGameplayTag::RequestGameplayTag(FName("Regen.Stamina"))