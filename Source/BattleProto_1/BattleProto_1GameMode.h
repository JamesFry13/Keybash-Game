// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleProto_1GameMode.generated.h"


UCLASS(minimalapi)
class ABattleProto_1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleProto_1GameMode();

    /** Remove the current menu widget and create a new one from the specified class, if provided. */
    UFUNCTION(BlueprintCallable, Category = "UMG Game")
        void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

    /** The widget class we will use as our menu when the game starts. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
        TSubclassOf<UUserWidget> StartingWidgetClass;

    /** The widget instance that we are using as our menu. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
        UUserWidget* CurrentWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Words")
        FString WordsString;


};
