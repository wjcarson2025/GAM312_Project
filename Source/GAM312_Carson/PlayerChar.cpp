// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"
#include <Math/Color.h>
#include <Microsoft/COMPointer.h>

// Sets default values
APlayerChar::APlayerChar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Player Camera Setup
	PlayerCamComp = CreateDefaultSubobject <UCameraComponent>(TEXT("First Person Cam")); // Creates Camera
	PlayerCamComp->SetupAttachment(GetMesh(), "Mesh"); // Attaches camera
	PlayerCamComp->bUsePawnControlRotation = true; // Assigns camera to rotate based on the pitch and yaw

	// Initialize the arrays so it stops crashing
	ResourcesArray.SetNum(3);
	ResourcesNameArray.Add(TEXT("Wood"));
	ResourcesNameArray.Add(TEXT("Stone"));
	ResourcesNameArray.Add(TEXT("Berry"));

	// Setup the building array
	BuildingArray.SetNum(3);
}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();

	// Setup a timer to call the DecreaseStats() function to 
	// adjust the player's stats every 2 seconds
	FTimerHandle StatsTimerHandler;
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandler, this, &APlayerChar::DecreaseStats, 2.0f, true);

	// Set the objects collected and built to 0 for init
	if (objWidget) {
		objWidget->UpdatebuildOBJ(0.0f);
		objWidget->UpdatematOBJ(0.0f);
	}

	objectsBuilt = 0.0f;
	matsCollected = 0.0f;
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player UI is loaded. Had to do this so it would stop crashing
	if (playerUI) {
		playerUI->UpdateBars(Health, Hunger, Stamina);
	}

	// If the user is building something
	if (isBuilding) {

		// If the spawnned part is valid
		if (spawnedPart) {
			// Set the location for the spawned part
			FVector StartLocation = PlayerCamComp->GetComponentLocation(); // Get the location of the camera
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f; // Get the vector we're looking multiply it by 400 units
			FVector EndLocation = StartLocation + Direction; // Calculate the end point
			spawnedPart->SetActorLocation(EndLocation); // Spawn the part at the end location
		}
	}
}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Names in quotes were setup in the project setup - inputs area of UE5
	// Setup the player axis inputs	to call the appropriate function
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerChar::AddControllerYawInput);

	// Setup the player action inputs to call the appropriate function
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerChar::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerChar::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::FindObject);
	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &APlayerChar::RotateBuilding);
}

void APlayerChar::MoveForward(float axisValue)
{
	// Forward and Backwards based on W and S inputs
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, axisValue);
}

void APlayerChar::MoveRight(float axisValue)
{
	// Strafe based on the A and D inputs
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);
}

void APlayerChar::StartJump()
{
	bPressedJump = true;
}

void APlayerChar::StopJump()
{
	bPressedJump = false;
}

void APlayerChar::FindObject()
{
	// Setup the hit by start position, direction and the distance to search
	FHitResult HitResult;
	FVector StartLocation = PlayerCamComp->GetComponentLocation();
	FVector Direction = PlayerCamComp->GetForwardVector() * 800.0f;
	FVector EndLocation = StartLocation + Direction;

	// Setup the raytrace search
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the player in the results
	QueryParams.bTraceComplex = true; // Enable the collisions to be complex
	QueryParams.bReturnFaceIndex = true; // Returns face information such as normals

	// If we're building, don't collect resources
	if (!isBuilding) {

		// Check for a hit
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
		{
			// Check to see if they have enough stamina to collect the resource first
			if (Stamina >= 0.0f)
			{
				// Cast the hit to our resources
				AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor()); // Get the actor in the scene that was hit

				// Check that the HitResource object is valid so we don't crash
				if (HitResource)
				{
					FString hitName = HitResource->resourceName; // Get the name of the resource
					int resourceValue = HitResource->resourceAmount; // Get the amount to pick up

					HitResource->totalResource = HitResource->totalResource - resourceValue; // Remove the resource amount from the total amount of the resource

					// If there is enough resources left to pick up, pick them up
					if (HitResource->totalResource > resourceValue)
					{
						GiveResources(resourceValue, hitName);

						check(GEngine != nullptr);
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Resource Collected"));

						// Update the number of resources collected
						matsCollected += resourceValue;
						if (objWidget) {
							objWidget->UpdatematOBJ(matsCollected);
						}

						// Show the decal for the hit
						UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);

						// Substract Stamina
						SetStamina(-2.0f);

						SetHunger(-1.0f);
					}
					else
					{
						HitResource->Destroy();

						check(GEngine != nullptr);
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Depleted"));
					}
				}
			}
		}
	}
	else
	{
		isBuilding = false;

		// Update the number of objects built
		objectsBuilt += 1.0f;
		if (objWidget) {
			objWidget->UpdatebuildOBJ(objectsBuilt);
		}
	}
}

//  Pick up aquired resource and add it to our inventory
void APlayerChar::GiveResources(float amount, FString resourceType)
{

	if (resourceType == "Wood")
	{
		ResourcesArray[0] += amount;
	}

	if (resourceType == "Stone")
	{
		ResourcesArray[1] += amount;
	}

	if (resourceType == "Berry")
	{
		ResourcesArray[2] += amount;
	}

	
}

void APlayerChar::SetHealth(float amount)
{
	// If the player's health is less than 100, add the amount given
	// Makes sure it won't go over 100
	if (Health + amount < 100)
	{
		Health = Health + amount;
	}
	else
	{
		Health = 100;
	}
}

void APlayerChar::SetHunger(float amount)
{
	// If the player's hunger is less than 100, add the amount given
	// Makes sure it won't go over 100
	if (Hunger + amount < 100)
	{
		Hunger = Hunger + amount;
	}
	else
	{
		Hunger = 100;
	}
}

void APlayerChar::SetStamina(float amount)
{
	// If the player's Stamina is less than 100, add the amount given
	// Makes sure it won't go over 100
	if (Stamina + amount < 100)
	{
		Stamina = Stamina + amount;
	}
	else
	{
		Stamina = 100;
	}
}

void APlayerChar::DecreaseStats()
{
	// If the player's hunger isn't empty, then subtract some hunger
	if (Hunger > 0)
	{
		SetHunger(-0.5f);
	}

	// reduce the player's stamina
	SetStamina(-1.0f);

	// If the player's hunger is empty, start decreasing the health
	if (Hunger <= 0)
	{
		SetHealth(-2.0f);
	}
}

void APlayerChar::UpdateResources(float woodAmount, float stoneAmount, FString buildingObject)
{
	// If we have enough wood
	if (woodAmount <= ResourcesArray[0])
	{
		// If we have enough stone
		if (stoneAmount <= ResourcesArray[1])
		{
			// Subtract the wood and stone from our collected resources
			ResourcesArray[0] -= woodAmount;
			ResourcesArray[1] -= stoneAmount;

			// Add the approproate structure to the approprate array
			if (buildingObject == "Wall") {
				BuildingArray[0] += 1;
			}

			if (buildingObject == "Floor") {
				BuildingArray[1] += 1;
			}

			if (buildingObject == "Ceiling") {
				BuildingArray[2] += 1;
			}
		}
	}
}

void APlayerChar::SpawnBuilding(int buildingID, bool& isSuccess)
{
	// If we're not currently building
	if (!isBuilding)
	{
		// Check to see if we have the item we need to build with
		if (BuildingArray[buildingID] >= 1)
		{
			// Place the building in the world
			isBuilding = true;
			FActorSpawnParameters SpawnParams; // Parameters for the spawned object

			// Set the location for the spawned part
			FVector StartLocation = PlayerCamComp->GetComponentLocation(); // Get the location of the camera
			FVector Direction = PlayerCamComp->GetForwardVector() * 600.0f; // Get the vector we're looking multiply it by 600 units
			FVector EndLocation = StartLocation + Direction; // Calculate the end point
			FRotator myRot(0, 0, 0);

			// Remove the object from our inventory
			BuildingArray[buildingID] -= 1;

			// Spawn the object
			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);

			// Return success
			isSuccess = true;
		}

		// Return success
		isSuccess = false;
	}
}

void APlayerChar::RotateBuilding()
{
	// If we're building, rotate it 90 degrees
	if (isBuilding)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));
	}
}

