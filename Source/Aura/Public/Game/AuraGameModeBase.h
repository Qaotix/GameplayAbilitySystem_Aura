// Copywrite Shanathan Hanson

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAuraGameModeBase();

protected:
	virtual void BeginPlay() override;
	
	
};
