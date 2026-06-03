// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource_M.h"


// Sets default values
AResource_M::AResource_M()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup for the mesh object
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh; // The root of the BP object

	/* Removing name text
	// Setup the text for the resource name
	ResourceNameTxt = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));

	// Link the text to the mesh
	ResourceNameTxt->SetupAttachment(Mesh);
	*/

}

// Called when the game starts or when spawned
void AResource_M::BeginPlay()
{
	Super::BeginPlay();

	/* Removed for project
	// Get the name entered for the resource
	tempText = tempText.FromString(resourceName);

	ResourceNameTxt->SetText(tempText);
	*/
}

// Called every frame
void AResource_M::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

