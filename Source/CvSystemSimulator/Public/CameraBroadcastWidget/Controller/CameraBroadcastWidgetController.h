// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CameraBroadcastWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class CVSYSTEMSIMULATOR_API UCameraBroadcastWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	void InitializeWidget(UWorld* World, TSubclassOf<UUserWidget> WidgetClass, FVector2D WidgetSize);
	void UpdateWidgetImage(const TArray<FColor>& Bitmap, FIntPoint CameraMatrixSize);

private:
	UUserWidget* CameraBroadcastWidgetInstance;
	UImage* CameraBroadcastImage;
};
