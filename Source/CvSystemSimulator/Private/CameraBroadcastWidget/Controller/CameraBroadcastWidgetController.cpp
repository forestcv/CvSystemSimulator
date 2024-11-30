// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraBroadcastWidget/Controller/CameraBroadcastWidgetController.h"

void UCameraBroadcastWidgetController::InitializeWidget(UWorld* World, TSubclassOf<UUserWidget> WidgetClass, FVector2D WidgetSize)
{
	if (!World || !WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraBroadcastWidgetController: World or WidgetClass is null!"));
		return;
	}

	CameraBroadcastWidgetInstance = CreateWidget<UUserWidget>(World, WidgetClass);
	if (CameraBroadcastWidgetInstance)
	{
		CameraBroadcastWidgetInstance->AddToViewport();
		CameraBroadcastWidgetInstance->SetDesiredSizeInViewport(WidgetSize);

		FVector2D ScreenSize;
		GEngine->GameViewport->GetViewportSize(ScreenSize);
		FVector2D WidgetPosition = FVector2D(
			PositionOffset.X,
			PositionOffset.Y);

		CameraBroadcastWidgetInstance->SetPositionInViewport(WidgetPosition);

		CameraBroadcastImage = Cast<UImage>(CameraBroadcastWidgetInstance->GetWidgetFromName(TEXT("CameraBroadcastImage")));
	}
}

void UCameraBroadcastWidgetController::UpdateWidgetImage(const TArray<FColor>& Bitmap, FIntPoint CameraMatrixSize)
{
	if (!CameraBroadcastImage)
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraBroadcastWidgetController: CameraBroadcastImage is null!"));
		return;
	}

	if(UTexture2D* Texture = UTexture2D::CreateTransient(
		CameraMatrixSize.X,CameraMatrixSize.Y, PF_B8G8R8A8); Texture)
	{
		void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
		Texture->PlatformData->Mips[0].BulkData.Unlock();
		Texture->UpdateResource();

		if (CameraBroadcastImage && Texture)
		{
			CameraBroadcastImage->SetBrushFromTexture(Texture, true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UCameraBroadcastWidgetController: CameraBroadcastImage or Texture is null!"))
		}
	}

}

