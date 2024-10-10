#include "DefaultDevice.h"

// Sets default values
ADefaultDevice::ADefaultDevice()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(Camera);
}

// Called when the game starts or when spawned
void ADefaultDevice::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the render target
	if (SceneCaptureComponent)
	{
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
		RenderTarget->InitCustomFormat(CameraMatrixSize.X, CameraMatrixSize.Y, PF_B8G8R8A8, false);
		SceneCaptureComponent->TextureTarget = RenderTarget;
	}

	// Initialize UI widget
	if (CameraBroadcastWidget)
	{
		CameraBroadcastWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CameraBroadcastWidget);
		if (CameraBroadcastWidgetInstance)
		{
			CameraBroadcastWidgetInstance->AddToViewport();
			CameraBroadcastWidgetInstance->SetDesiredSizeInViewport(
				FVector2D(CameraMatrixSize.X / DownscaleFactor, 
					CameraMatrixSize.Y / DownscaleFactor));
			CameraBroadcastImage = Cast<UImage>(CameraBroadcastWidgetInstance->
				GetWidgetFromName(TEXT("CameraBroadcastImage")));
		}
	}
}

// Called every frame
void ADefaultDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float TimeSinceLastUpdate = 0.0f;
	TimeSinceLastUpdate += DeltaTime;

	const float ImageUpdateInterval = 1.0f / FPS;  // Convert FPS to seconds

	if (TimeSinceLastUpdate >= ImageUpdateInterval)
	{
		UpdateWidgetImage();
		TimeSinceLastUpdate = 0.0f;
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

// Handles forward movement
void ADefaultDevice::MoveForward(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

// Handles right movement
void ADefaultDevice::MoveRight(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

// Updates the widget image
void ADefaultDevice::UpdateWidgetImage()
{
	if (!SceneCaptureComponent ||
		!SceneCaptureComponent->TextureTarget ||
		!CameraBroadcastWidgetInstance ||
		!CameraBroadcastImage)
	{
		UE_LOG(LogTemp, Error, TEXT("Widget or SceneCapture components are null!"));
		return;
	}

	FTextureRenderTargetResource* RenderTargetResource =
		SceneCaptureComponent->TextureTarget->GameThread_GetRenderTargetResource();

	TArray<FColor> Bitmap;
	if (RenderTargetResource->ReadPixels(Bitmap))
	{
		// Ensure the bitmap has the correct size
		for (FColor& Pixel : Bitmap)
		{
			Pixel.A = 255;  // Set alpha to fully opaque
		}

		// Create a new texture and copy the bitmap to it
		UTexture2D* Texture = UTexture2D::CreateTransient(
			CameraMatrixSize.X,
			CameraMatrixSize.Y,
			PF_B8G8R8A8);

		void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
		Texture->PlatformData->Mips[0].BulkData.Unlock();
		Texture->UpdateResource();

		// Update the broadcast image
		CameraBroadcastImage->SetBrushFromTexture(Texture, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ReadPixels failed!"));
	}
}