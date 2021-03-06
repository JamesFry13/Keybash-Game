// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleProto_1Character.h"
#include "BattleEnemy.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"



//////////////////////////////////////////////////////////////////////////
// ABattleProto_1Character

ABattleProto_1Character::ABattleProto_1Character()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set to determin wether player is in overworld or battle scene

	bInBattle = false;

	bStartTemp = true;



	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	MaxHealth = 20000;
	Health = MaxHealth;

	PlayerScore = 0;

	GoodWins = 0;
	BadWins = 0;
	TotalWins = 0;
	PlayerLevel = 0;

	DiscardPile.Empty();

	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABattleProto_1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Startbattle", IE_Pressed, this, &ABattleProto_1Character::ToggleBattle);
	

	PlayerInputComponent->BindAxis("MoveForward", this, &ABattleProto_1Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABattleProto_1Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABattleProto_1Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABattleProto_1Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABattleProto_1Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABattleProto_1Character::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABattleProto_1Character::OnResetVR);
}


void ABattleProto_1Character::OnResetVR()
{
	// If BattleProto_1 is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in BattleProto_1.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABattleProto_1Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!bInBattle)
	{
		Jump();
	}
}

void ABattleProto_1Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!bInBattle)
	{
		StopJumping();
	}
}

void ABattleProto_1Character::TurnAtRate(float Rate)
{
	if (!bInBattle)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ABattleProto_1Character::LookUpAtRate(float Rate)
{
	if (!bInBattle)
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ABattleProto_1Character::MoveForward(float Value)
{
	if (!bInBattle)
	{
		if ((Controller != nullptr) && (Value != 0.0f))
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void ABattleProto_1Character::MoveRight(float Value)
{
	if (!bInBattle)
	{

		if ((Controller != nullptr) && (Value != 0.0f))
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

void ABattleProto_1Character::ToggleBattle() 
{

	if (!bInBattle)
	{
		bInBattle = true;

		UE_LOG(LogTemp, Warning, TEXT("In Battle"));

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), MyBattleCam, FoundCams);


		

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABattleProto_1Character::ChangeCamera, 1, false);
		//ChangeCamera();

	}

	else {

		bInBattle = false;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), MyBattleCam, FoundCams);

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		TotalWins = BadWins + GoodWins;

		PlayerLevel = 1 + (sqrt(TotalWins));

		float tempMaxHealth = 20000 * PlayerLevel;

		Health = Health + (tempMaxHealth - MaxHealth);

		MaxHealth = tempMaxHealth;

		//PlayerController->SetViewTarget(this);

		FTimerHandle TimerHandle;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABattleProto_1Character::ChangeCameraBack, 1, false);

		

		//UE_LOG(LogTemp, Warning, TEXT("Out of Battle"));
	}


}

void ABattleProto_1Character::ChangeCamera()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetViewTarget(FoundCams[0]);

}

void ABattleProto_1Character::ChangeCameraBack()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetViewTarget(this);
}

void ABattleProto_1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	if (Health < 0) {
		ToggleBattle();
		UE_LOG(LogTemp, Warning, TEXT("Game Lost"));
		Health = 1;
	}
	else if (Health > MaxHealth) {
		Health = MaxHealth;
	}


	if (bInBattle) {

		if (CardHand.Num() < 4) {
			CardHand.Add(DrawPile.Last());
			DrawPile.RemoveAt(DrawPile.Num()-1);
			if (DrawPile.Num() < 1) {
				DrawPile = DiscardPile;
				DiscardPile.Empty();
			}
		}
	}
}
