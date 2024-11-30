#include "DefaultDevice.h"

ADefaultDevice::ADefaultDevice()
{
	PrimaryActorTick.bCanEverTick = true;

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
	//SceneCaptureComponent->bCaptureEveryFrame = false;
	//SceneCaptureComponent->bCaptureOnMovement = false;

	WidgetController = CreateDefaultSubobject<UCameraBroadcastWidgetController>(TEXT("WidgetController"));
}

void ADefaultDevice::BeginPlay()
{
	Super::BeginPlay();

	if (SceneCaptureComponent)
	{
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
		RenderTarget->InitCustomFormat(CameraMatrixSize.X, CameraMatrixSize.Y, PF_B8G8R8A8, true);
		SceneCaptureComponent->TextureTarget = RenderTarget;
	}

	if (WidgetController)
	{
		FVector2D WidgetSize = FVector2D(CameraMatrixSize.X / DownscaleFactor, CameraMatrixSize.Y / DownscaleFactor);

		UE_LOG(LogTemp, Warning, TEXT("Initializing WidgetController. CameraBroadcastWidget: %s, WidgetSize: %s"),
			*CameraBroadcastWidget->GetName(),
			*WidgetSize.ToString());

		WidgetController->InitializeWidget(GetWorld(), CameraBroadcastWidget, WidgetSize);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetController is null!"));
	}

}

// Called every frame
void ADefaultDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float TimeSinceLastUpdate = 0.0f;
	TimeSinceLastUpdate += DeltaTime;

	static const float ImageUpdateInterval = 1.0f / FPS;

	if (TimeSinceLastUpdate >= ImageUpdateInterval)
	{
		TArray<FColor> Bitmap;
		if (WidgetController && CreateBitmapFromRenderTarget(Bitmap) && 
			Bitmap.Num() == CameraMatrixSize.X * CameraMatrixSize.Y)
		{
			WidgetController->UpdateWidgetImage(Bitmap, CameraMatrixSize);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ADefaultDevice: Failed to create bitmap from render target!"));
		}

		TimeSinceLastUpdate = 0.0f;
	}
}

void ADefaultDevice::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement functions
	PlayerInputComponent->BindAxis("MoveForward", this, &ADefaultDevice::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADefaultDevice::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ADefaultDevice::MoveUp);

	PlayerInputComponent->BindAxis("RotateRoll", this, &ADefaultDevice::RotateRoll);
	PlayerInputComponent->BindAxis("RotatePitch", this, &ADefaultDevice::RotatePitch);
	PlayerInputComponent->BindAxis("RotateYaw", this, &ADefaultDevice::RotateYaw);
}

void ADefaultDevice::MoveForward(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADefaultDevice::MoveRight(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADefaultDevice::MoveUp(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void ADefaultDevice::RotateRoll(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddActorLocalRotation(FRotator(0.0f, 0.0f, Value));
	}
}

void ADefaultDevice::RotatePitch(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddActorLocalRotation(FRotator(Value, 0.0f, 0.0f));
	}
}

void ADefaultDevice::RotateYaw(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddActorLocalRotation(FRotator(0.0f, Value, 0.0f));
	}
}

// Функция для создания битмапа из рендер-таргета
bool ADefaultDevice::CreateBitmapFromRenderTarget(TArray<FColor>& OutBitmap) const
{
	if (!SceneCaptureComponent || !SceneCaptureComponent->TextureTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("ADefaultDevice: SceneCaptureComponent or TextureTarget is null!"));
		return false;
	}

	SceneCaptureComponent->CaptureScene();
	FTextureRenderTargetResource* RenderTargetResource = SceneCaptureComponent->TextureTarget->GameThread_GetRenderTargetResource();

	// Чтение пикселей с рендер-таргета
	if (RenderTargetResource->ReadPixels(OutBitmap))
	{
		// Обработка альфа-канала для всех пикселей
		//for (FColor& Pixel : OutBitmap)
		//{
		//	Pixel.A = 255; // Устанавливаем полную непрозрачность
		//}
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("ADefaultDevice: ReadPixels failed!"));
	return false;
}