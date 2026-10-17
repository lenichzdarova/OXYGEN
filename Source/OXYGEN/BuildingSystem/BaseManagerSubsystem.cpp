// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseManagerSubsystem.h"

FTransform UBaseManagerSubsystem::MakeIntegerTransform(const FTransform& InTransform)
{
    FVector Location = InTransform.GetLocation();
    Location.X = FMath::RoundToFloat(Location.X);
    Location.Y = FMath::RoundToFloat(Location.Y);
    Location.Z = FMath::RoundToFloat(Location.Z);

    FRotator Rotation = InTransform.GetRotation().Rotator();
    Rotation.Pitch = FMath::RoundToFloat(Rotation.Pitch);
    Rotation.Yaw   = FMath::RoundToFloat(Rotation.Yaw);
    Rotation.Roll  = FMath::RoundToFloat(Rotation.Roll);

    return FTransform(Rotation, Location, InTransform.GetScale3D());
}

bool UBaseManagerSubsystem::IsLocationInAnyBase(const FVector& Location, float Padding, FGuid& OutBaseId) const
{
    for (const FBaseData& Base : Bases)
    {
        if (Base.GetInfluenceBox(Padding).IsInside(Location))
        {
            OutBaseId = Base.Id;
            return true;
        }
    }
    OutBaseId = FGuid();
    return false;
}

FGuid UBaseManagerSubsystem::RegisterPiece(AActor* Piece, const FBox& PieceBounds)
{
    if (!Piece) return FGuid();

    int32 ExistingIndex = FindBaseIndexForBounds(PieceBounds, 500.f);

    if (ExistingIndex != INDEX_NONE)
    {
        FBaseData& Base = Bases[ExistingIndex];
        Base.Pieces.Add(Piece);
        ExpandBaseBounds(Base, PieceBounds);
        return Base.Id;
    }

    // Новая база — округляем рут
    FBaseData NewBase;
    NewBase.Id = FGuid::NewGuid();
    NewBase.RootTransform = MakeIntegerTransform(Piece->GetActorTransform());
    NewBase.Pieces.Add(Piece);
    ExpandBaseBounds(NewBase, PieceBounds);
    Bases.Add(NewBase);

    return NewBase.Id;
}

void UBaseManagerSubsystem::UnregisterPiece(AActor* Piece)
{
    int32 Index = FindBaseIndexByPiece(Piece);
    if (Index == INDEX_NONE) return;

    FBaseData& Base = Bases[Index];
    Base.Pieces.Remove(Piece);

    if (Base.Pieces.Num() == 0)
    {
        Bases.RemoveAt(Index);
        return;
    }

    // Пересчёт bounds
    Base.BoundsMin = FVector(FLT_MAX);
    Base.BoundsMax = FVector(-FLT_MAX);

    for (const TWeakObjectPtr<AActor>& WeakPiece : Base.Pieces)
    {
        if (AActor* ValidPiece = WeakPiece.Get())
        {
            FBox PieceBox = ValidPiece->GetComponentsBoundingBox(true);
            ExpandBaseBounds(Base, PieceBox);
        }
    }
}

FTransform UBaseManagerSubsystem::GetPreviewTransform(const FTransform& DesiredTransform) const
{
    const FVector DesiredLocation = DesiredTransform.GetLocation();
    const FRotator DesiredRotation = DesiredTransform.GetRotation().Rotator();

    for (const FBaseData& Base : Bases)
    {
        if (Base.GetInfluenceBox().IsInside(DesiredLocation))
        {
            const FTransform& Root = Base.RootTransform;

            FVector LocalPos = Root.InverseTransformPosition(DesiredLocation);

            const float GridSize = 100.f;
            LocalPos.X = FMath::GridSnap(LocalPos.X, GridSize);
            LocalPos.Y = FMath::GridSnap(LocalPos.Y, GridSize);
            LocalPos.Z = FMath::GridSnap(LocalPos.Z, GridSize);

            FVector SnappedLocation = Root.TransformPosition(LocalPos);

            FRotator SnappedRotation = DesiredRotation;
            SnappedRotation.Yaw = FMath::GridSnap(SnappedRotation.Yaw, 90.f);
            SnappedRotation.Pitch = 0.f;
            SnappedRotation.Roll = 0.f;

            return FTransform(SnappedRotation, SnappedLocation);
        }
    }

    // Нет базы — округляем до целых
    return MakeIntegerTransform(DesiredTransform);
}

FBaseData UBaseManagerSubsystem::GetBaseData(const FGuid& BaseId) const
{
    for (const FBaseData& Base : Bases)
    {
        if (Base.Id == BaseId)
            return Base;
    }
    return FBaseData();
}

void UBaseManagerSubsystem::ExpandBaseBounds(FBaseData& Base, const FBox& PieceBounds)
{
    Base.BoundsMin = Base.BoundsMin.ComponentMin(PieceBounds.Min);
    Base.BoundsMax = Base.BoundsMax.ComponentMax(PieceBounds.Max);
}

int32 UBaseManagerSubsystem::FindBaseIndexForBounds(const FBox& Bounds, float Padding) const
{
    for (int32 i = 0; i < Bases.Num(); ++i)
    {
        if (Bases[i].GetInfluenceBox(Padding).Intersect(Bounds))
            return i;
    }
    return INDEX_NONE;
}

int32 UBaseManagerSubsystem::FindBaseIndexByPiece(AActor* Piece) const
{
    for (int32 i = 0; i < Bases.Num(); ++i)
    {
        if (Bases[i].Pieces.Contains(Piece))
            return i;
    }
    return INDEX_NONE;
}