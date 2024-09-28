#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CameraBroadcastWidget.generated.h"

UCLASS()
class CVSYSTEMSIMULATOR_API UCameraBroadcastWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Capture")
        void SetRenderTargetTexture(UTextureRenderTarget2D* RenderTarget);

protected:
    UPROPERTY(meta = (BindWidget))
        class UImage* CaptureImage;

    UPROPERTY(EditDefaultsOnly, Category = "Capture")
        class UMaterialInterface* CaptureMaterialParent;

    UPROPERTY()
        class UMaterialInstanceDynamic* CaptureMaterialInstance;
};