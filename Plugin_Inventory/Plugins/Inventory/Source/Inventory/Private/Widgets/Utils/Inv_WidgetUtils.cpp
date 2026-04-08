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

FVector2D UInv_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();

	return Geometry.GetLocalSize();
}

bool UInv_WidgetUtils::IsWithinBound(const FVector2D& BoundaryPos, const FVector2D& WidgetSize, const FVector2D& MousePos)
{
	return MousePos.X >= BoundaryPos.X && MousePos.X <= (BoundaryPos.X + WidgetSize.X)
		&& MousePos.Y >= BoundaryPos.Y && MousePos.Y <= (BoundaryPos.Y + WidgetSize.Y);

}

FVector2D UInv_WidgetUtils::GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize, const FVector2D MousePos)
{
	FVector2D ClampedPosition = MousePos;

	//위젯이 경계내부에 위치하도록 가로위치를 조정한다.
	if (MousePos.X + WidgetSize.X > Boundary.X) // 오른쪽가장자리를 초과하면,
	{
		ClampedPosition.X = Boundary.X - WidgetSize.X;
	}
	if (MousePos.X < 0.f)//왼쪽 초과
	{
		ClampedPosition.X = 0.f;
	}

	//수직 위치 조정
	if (MousePos.Y + WidgetSize.Y > Boundary.Y) //Botton초과
	{
		ClampedPosition.Y = Boundary.Y - WidgetSize.Y;
	}
	if (MousePos.Y < 0.f)//위쪽초과
	{
		ClampedPosition.Y = 0.f;
	}
	return ClampedPosition;
}

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}
