// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Components/Widget.h"
#include "Blueprint/SlateBlueprintLibrary.h"

FVector2D UInv_WidgetUtils::GetWidgetPosision(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	FVector2D PixelPosition;
	FVector2D ViewportPosision;

	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPosition, ViewportPosision);

	return ViewportPosision;
}

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}
