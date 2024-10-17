#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CameraBroadcastWidget/Controller/CameraBroadcastWidgetController.h"
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

	// UI Widget class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> CameraBroadcastWidget;

	// Контроллер виджета
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		UCameraBroadcastWidgetController* WidgetController;

	// Downscale factor for UI resolution, editable from Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int DownscaleFactor = 8;

	// Camera resolution and settings, editable from Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	FIntPoint CameraMatrixSize{ 1280, 720 };

	// FPS for image update, editable from Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	int FPS = 25;

	// Handles movement input
	void MoveForward(float Value);
	void MoveRight(float Value);

	// Updates the widget image
	void UpdateWidgetImage();

	// Function to create bitmap from render target
	bool CreateBitmapFromRenderTarget(TArray<FColor>& OutBitmap) const;
};
