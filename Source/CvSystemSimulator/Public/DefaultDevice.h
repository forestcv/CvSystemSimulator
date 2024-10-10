#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "DefaultDevice.generated.h"

UCLASS()
class CVSYSTEMSIMULATOR_API ADefaultDevice : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADefaultDevice();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Spring arm component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* SpringArm;

	// Camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	// Scene Capture component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraCapture")
		USceneCaptureComponent2D* SceneCaptureComponent;

	// Movement components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		UFloatingPawnMovement* MovementComponent;

	// UI Widget class and instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> CameraBroadcastWidget;

	UUserWidget* CameraBroadcastWidgetInstance;
	UImage* CameraBroadcastImage;

	// FPS for image update
	FIntPoint CameraMatrixSize{1920, 1080};
	int FPS = 25;
	int DownscaleFactor = 8;

	// Handles updating the widget image
	void UpdateWidgetImage();

	// Handles movement input
	void MoveForward(float Value);
	void MoveRight(float Value);
};