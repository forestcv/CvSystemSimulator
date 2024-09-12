// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultDevice.h"
#include "CameraBroadcastWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
ADefaultDevice::ADefaultDevice()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a Spring Arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent); // Attach the spring arm to the root component
	SpringArm->TargetArmLength = 0.0f; // Set the length of the spring arm
	SpringArm->bUsePawnControlRotation = true; // Allow the arm to rotate based on the controller

	// Create a Camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the spring arm
	Camera->bUsePawnControlRotation = false; // We don't want the camera to rotate relative to the arm

	// Create a movement component
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	// Create a scene capture component
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(RootComponent);
	SceneCaptureComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); // Adjust as needed
}

// Called when the game starts or when spawned
void ADefaultDevice::BeginPlay()
{
	Super::BeginPlay();

	// Создаем рендер-таргет и назначаем его камере
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitAutoFormat(1024, 1024);
	SceneCaptureComponent->TextureTarget = RenderTarget;

	SceneCaptureComponent->TextureTarget = RenderTarget;

	// Создаем виджет и добавляем его на экран
	if (UCameraBroadcastWidget* CameraBroadcastWidget = CreateWidget<UCameraBroadcastWidget>(GetWorld(),
		UCameraBroadcastWidget::StaticClass()))
	{
		CameraBroadcastWidget->AddToViewport();
		CameraBroadcastWidget->SetRenderTargetTexture(RenderTarget);
	}
	
}

// Called every frame
void ADefaultDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefaultDevice::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement functions
	PlayerInputComponent->BindAxis("MoveForward", this, &ADefaultDevice::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADefaultDevice::MoveRight);
}

void ADefaultDevice::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADefaultDevice::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

