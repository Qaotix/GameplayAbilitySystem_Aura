// Copywrite Shanathan Hanson


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/AuraCharacter.h"
#include "Compression/lz4.h"
#include "Interaction/EnemyInterface.h"
#include "Math/RotationMatrix.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace(DeltaTime);
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;

	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	SetupInputComponent();
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered, this,	&AAuraPlayerController::Move );
	
}


// ReSharper disable once CppMemberFunctionMayBeConst
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotator = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotator.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = Cast<APawn>(GetPawn()))
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y, true);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X, true);
	}
}

void AAuraPlayerController::CursorTrace(float DeltaTime)
{
	float MyTime = DeltaTime;
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;
	
	LastActor = CurrentActor;
	CurrentActor = CursorHit.GetActor();

	/**
	 * Line Trace From Cursor. There are several scenarios.
	 * A.  LastActor is null && CurrentActor is null.
	 *		- Do Nothing
	 * B.  LastActor is null && CurrentActor is Valid.
	 *		- Highlight CurrentActor
	 * C.  LastActor is Valid && CurrentActor is null.
	 *		- UnHighLight LastActor
	 * D.  Both Actors are valid but LastActor != CurrentActor
	 *		- UnHighlight LastActor
	 *		- Hightlight CurrentActor
	 * E.  Both Actors are valid but LastActor == CurrentActor
	 *		- Do Nothing
	 */
	if (LastActor == nullptr)
	{
		if (CurrentActor != nullptr)
		{
			// Case B
			CurrentActor->HighlightActor();
		}
		else
		{
			// Case A - Do Nothing
		}
	}
	else // LastActor is Valid
	{
		if (CurrentActor == nullptr)
		{
			// Case C
			LastActor->UnHighlightActor();
		}
		else // both actors are valid
		{
			if (LastActor != CurrentActor) 
			{
				// Case D
				LastActor->UnHighlightActor();
				CurrentActor->HighlightActor();
			}
			else
			{
				// Case E - Do Nothing
			}
		}
	}
}
