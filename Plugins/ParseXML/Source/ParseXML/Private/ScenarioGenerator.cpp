// Fill out your copyright notice in the Description page of Project Settings.


#include "ScenarioGenerator.h"

// Sets default values
AScenarioGenerator::AScenarioGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PrintLog("Inside the SceneGen constructor");
	//PrintLog(VehicleRef);
	//vehicle = LoadVehicleFromPluginAsset("Blueprint'/BT_Plugin/VehicleModel/test1.test1'");
	//static ConstructorHelpers::FObjectFinder<UClass> vehicleAsset(TEXT("Blueprint'/Game/MyWheeledVehicleObject.MyWheeledVehicleObject'"));
	vehicleBPAsset = LoadObject<UClass>(nullptr, TEXT("Class'/Game/MyWheeledVehicleObject.MyWheeledVehicleObject_C'"));
	//vehicleAsset.Object;
}

// Called when the game starts or when spawned
void AScenarioGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AScenarioGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AWheeledVehicleObject* AScenarioGenerator::LoadVehicleFromPluginAsset(FString Path)
{
	AWheeledVehicleObject* VehicleAsset = LoadObjFromPath<AWheeledVehicleObject>(*Path);
	if (VehicleAsset != nullptr)
	{
		return VehicleAsset;
	}
	return nullptr;
}

AWheeledVehicleObject* AScenarioGenerator::SpawnVehicle(FVehicleSpecification VehicleSpec)
{
	UWorld* World = GetWorld();
	FVector LocationVector = VehicleSpec.WayPoint->GetActorLocation();
	PrintLog(LocationVector.ToString());
	FVector SpawnPoint = FVector(LocationVector.X, LocationVector.Y, 10.0f);
	//FActorSpawnParameters SpawnParam;
	//SpawnParam.Owner = this;
	//FRotator Rotator = FRotator(0, 0, 0);

	FQuat vehicleRotation(0.0f, 0.0f, 0.0f, 0.0f);
	FTransform SpawnTransform(vehicleRotation, SpawnPoint);
	AWheeledVehicleObject* newVehicle = Cast<AWheeledVehicleObject>(UGameplayStatics::BeginDeferredActorSpawnFromClass(World, AWheeledVehicleObject::StaticClass(), SpawnTransform));

	UGameplayStatics::FinishSpawningActor(newVehicle, SpawnTransform);
	//AWheeledVehicleObject* Temp = World->SpawnActor<AWheeledVehicleObject>(vehicle, SpawnPoint, Rotator, SpawnParam);
	//Temp->InitializeWheeledVehicle(VehicleSpec.BT_Path, VehicleSpec.WayPoint);
	//Temp->InitializeBlackBoardValues();
	return newVehicle;
}

/*
APedestrianCharacter* AScenarioGenerator::SpawnPedestrian(FPedestrianSpecification PedestrianSpec)
{
	UWorld* World = GetWorld();

	const FVector LocationVector = PedestrianSpec.RoadMesh->GetActorLocation();
	const FVector SpawnPoint = FVector(LocationVector.X, LocationVector.Y, 10.0f);

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	const FRotator Rotator = FRotator(0, 0, 0);

	// spawn
	APedestrianCharacter* Temp = World->SpawnActor<APedestrianCharacter>(
		PedestrianSpec.PedestrianAsset, SpawnPoint, Rotator, SpawnParam);

	return Temp;
}
*/