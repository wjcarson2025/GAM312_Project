// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Resource_M.generated.h"

UCLASS()
class GAM312_CARSON_API AResource_M : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResource_M();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	FString resourceName = "Wood"; // Name of the resource

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int resourceAmount = 5; // The amount of the resource picked up by click

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int totalResource = 100; // The total amount of resources available

	UPROPERTY()
	FText tempText; // Temp variable for simply storing the name of the resource

	UPROPERTY(EditAnywhere)
	UTextRenderComponent* ResourceNameTxt; // Setup for the name of the resource to be displayed

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh; // Mesh object for the pickup

};
