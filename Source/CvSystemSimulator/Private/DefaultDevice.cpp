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
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->bCaptureOnMovement = false;

	// Создание экземпляра контроллера виджета
	WidgetController = CreateDefaultSubobject<UCameraBroadcastWidgetController>(TEXT("WidgetController"));
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

	// Инициализация виджета через контроллер с передачей размера
	if (WidgetController)
	{
		WidgetController->InitializeWidget(GetWorld(), CameraBroadcastWidget, 
			FVector2D(CameraMatrixSize.X / DownscaleFactor, CameraMatrixSize.Y / DownscaleFactor));
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
		if (CreateBitmapFromRenderTarget(Bitmap))
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
}

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