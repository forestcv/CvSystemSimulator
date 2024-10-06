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

	CurrentColor = FLinearColor::Red;
	ColorChangeTime = 0.040f;
	TimeSinceLastColorChange = 0.0f;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(Camera);
}

// Called when the game starts or when spawned
void ADefaultDevice::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBroadcastWidget)
	{
		CameraBroadcastWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CameraBroadcastWidget);
		if (CameraBroadcastWidgetInstance)
		{
			CameraBroadcastWidgetInstance->AddToViewport();

			CameraBroadcastWidgetInstance->SetDesiredSizeInViewport(FVector2D(320.0f, 240.0f));

			ColorChangingImage = Cast<UImage>(CameraBroadcastWidgetInstance->GetWidgetFromName(TEXT("CameraBroadcastImage")));

			//if (ColorChangingImage)
			//{
			//	ChangeColor();
			//}
		}
	}

	if (SceneCaptureComponent)
	{
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
		RenderTarget->InitCustomFormat(320, 240, PF_B8G8R8A8, false);

		SceneCaptureComponent->TextureTarget = RenderTarget;
	}
}

// Called every frame
void ADefaultDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastColorChange += DeltaTime;
	if (TimeSinceLastColorChange >= ColorChangeTime)
	{
		UpdateWidgetImage();
		//ChangeColor();
		TimeSinceLastColorChange = 0.0f;
	}
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

void ADefaultDevice::ChangeColor()
{
	CurrentColor = FLinearColor::MakeRandomColor();
	if (ColorChangingImage)
	{
		ColorChangingImage->SetColorAndOpacity(CurrentColor);
	}
}

void ADefaultDevice::UpdateWidgetImage()
{
	static int cnt = 0;

	if (SceneCaptureComponent &&
		SceneCaptureComponent->TextureTarget &&
		CameraBroadcastWidgetInstance &&
		ColorChangingImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Components are valid, starting to capture image."));

		// Получаем ресурс рендера таргета
		FTextureRenderTargetResource* RenderTargetResource =
			SceneCaptureComponent->TextureTarget->GameThread_GetRenderTargetResource();

		// Создаем массив для пикселей
		TArray<FColor> Bitmap;
		if (RenderTargetResource->ReadPixels(Bitmap))
		{
			UE_LOG(LogTemp, Warning, TEXT("ReadPixels succeeded, Bitmap size: %d"), Bitmap.Num());

			for (int32 i = 0; i < 320 * 240; ++i)
			{
				Bitmap[i].A = 255;
				//UE_LOG(LogTemp, Error, TEXT("Bitmap: %d, %d, %d, %d"), Bitmap[i].R, Bitmap[i].G, Bitmap[i].B, Bitmap[i].A);
			}

			if (Bitmap.Num() == 320 * 240)
			{
				UTexture2D* Texture = UTexture2D::CreateTransient(320, 240, PF_B8G8R8A8);

				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
				Texture->PlatformData->Mips[0].BulkData.Unlock();

				Texture->UpdateResource();

				if (ColorChangingImage)
				{
					UE_LOG(LogTemp, Warning, TEXT("Applying texture to widget: %d"), ++cnt);
					ColorChangingImage->SetBrushFromTexture(Texture, true);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ColorChangingImage is null!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Bitmap size mismatch! Expected: %d, Got: %d"), 320 * 240, Bitmap.Num());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ReadPixels failed!"));
		}
	}
	else
	{
		if (!SceneCaptureComponent)
			UE_LOG(LogTemp, Error, TEXT("SceneCaptureComponent is null!"));
		if (!SceneCaptureComponent->TextureTarget)
			UE_LOG(LogTemp, Error, TEXT("TextureTarget is null!"));
		if (!CameraBroadcastWidgetInstance)
			UE_LOG(LogTemp, Error, TEXT("CameraBroadcastWidgetInstance is null!"));
		if (!ColorChangingImage)
			UE_LOG(LogTemp, Error, TEXT("ColorChangingImage is null!"));
	}
}

