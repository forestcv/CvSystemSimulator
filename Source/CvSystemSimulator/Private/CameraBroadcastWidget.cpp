#include "CameraBroadcastWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

void UCameraBroadcastWidget::SetRenderTargetTexture(UTextureRenderTarget2D* RenderTarget)
{
    if (CaptureImage && CaptureMaterialParent)
    {
        if (!CaptureMaterialInstance)
        {
            CaptureMaterialInstance = UMaterialInstanceDynamic::Create(CaptureMaterialParent, this);
        }

        if (CaptureMaterialInstance)
        {
            CaptureMaterialInstance->SetTextureParameterValue(TEXT("RenderTargetTexture"), RenderTarget);
            CaptureImage->SetBrushFromMaterial(CaptureMaterialInstance);
        }
    }
}