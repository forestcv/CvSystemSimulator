// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultDevice.h"

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
}

// Called when the game starts or when spawned
void ADefaultDevice::BeginPlay()
{
	Super::BeginPlay();
	
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

