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

	UTexture2D* Texture = UTexture2D::CreateTransient(
		CameraMatrixSize.X,
		CameraMatrixSize.Y,
		PF_B8G8R8A8);

	void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
	Texture->PlatformData->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	if (CameraBroadcastImage && Texture)
	{
		CameraBroadcastImage->SetBrushFromTexture(Texture, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraBroadcastWidgetController: CameraBroadcastImage or Texture is null!"))
	}

}

