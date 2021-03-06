// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleProto_1Character.generated.h"

UCLASS(config=Game)
class ABattleProto_1Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ABattleProto_1Character();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> MyBattleCam;

	TArray<AActor*> FoundCams;



	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> MyBattleEnemy;

	TArray<AActor*> FoundEnemies;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	//wether player is in battle or not
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Battle)
		bool bInBattle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		float Health;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		float PlayerScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		int GoodWins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		int BadWins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		int TotalWins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		float PlayerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		bool bStartTemp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		TArray<int> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		TArray<int> CardHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		TArray<int> DrawPile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
		TArray<int> DiscardPile;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** called to start fight. Will be changed to happed when collide with enemy*/


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
		void ToggleBattle();

	UFUNCTION(BlueprintCallable)
		void ChangeCamera();

	UFUNCTION(BlueprintCallable)
		void ChangeCameraBack();


	virtual void Tick(float DeltaTime) override;
};

