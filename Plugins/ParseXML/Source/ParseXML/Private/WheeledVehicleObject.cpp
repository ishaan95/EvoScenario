// Fill out your copyright notice in the Description page of Project Settings.


#include "WheeledVehicleObject.h"
#include "GameFramework/Pawn.h"
#include "WheeledVehicleMovementComponent.h"



//runs first Tick when simulate
AWheeledVehicleObject::AWheeledVehicleObject()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrintLog("Inside vehicle object constructor ");
}

void AWheeledVehicleObject::BeginPlay()
{
	Super::BeginPlay();
	PrintLog("Inside vehicle object beginplay");
	VehicleController = GetController<AVehicleController>();
	InitializeWheeledVehicle(BehaviorTreePath, WayPoint);
	InitializeBlackBoardValues();
}

void AWheeledVehicleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//PrintLog("Inside vehicle object tick");

	UpdateBlackBoard(DeltaTime);
	UpdateControlValue();
}

void AWheeledVehicleObject::UpdateControlValue()
{
 	float Throttle = VehicleController->BlackboardComponent->GetValueAsFloat("ThrottleValue");
	float Brake = VehicleController->BlackboardComponent->GetValueAsFloat("BrakeValue");
	float Steer = VehicleController->BlackboardComponent->GetValueAsFloat("SteerValue");
	UE_LOG(LogEngine, Warning, TEXT("Given throttle value is %f"), Throttle);
	ApplyControlValue(Throttle, Steer, Brake);
}

void AWheeledVehicleObject::UpdateBlackBoard(float Delta)
{
	VehicleController->BlackboardComponent->SetValueAsVector("VehicleWorldLocation", this->GetActorLocation());
	VehicleController->BlackboardComponent->SetValueAsFloat("TimeDelta", Delta);
	VehicleController->BlackboardComponent->SetValueAsVector("VehicleVelocity", this->GetVelocity());
	UE_LOG(LogEngine, Warning, TEXT("Vehicle velocity during blackboard update is %f"), VehicleController->BlackboardComponent->GetValueAsInt("VehicleVelocity"));
}

bool AWheeledVehicleObject::SelfDestroy()
{
	this->Destroy();
	return true;
}

bool AWheeledVehicleObject::InitializeWheeledVehicle(FString Path, AWayPoint* WP)
{

	PrintLog("Initialize Wheeled Vehicle ");
	BehaviorTreePath = Path;
	WayPoint = WP;
	if (VehicleController != NULL)
	{
		PrintLog("Vehicle Controller not NULL");
		VehicleController->InitializeVehicleController(BehaviorTreePath, WayPoint);
		return true;
	}
	else
	{
		PrintLog("Vehicle Controller NULL");
		return false;
	}
}

void AWheeledVehicleObject::ApplyControlValue(float Throttle, float Steering, float Brake)
{
	this->GetVehicleMovement()->SetBrakeInput(Brake);
	this->GetVehicleMovement()->SetThrottleInput(Throttle);
	this->GetVehicleMovement()->SetSteeringInput(Steering);
}

void AWheeledVehicleObject::InitializeBlackBoardValues()
{
	if (VehicleController != NULL)
	{
		//PrintLog("Inside Initialize Black Board ");
		VehicleController->BlackboardComponent->SetValueAsObject("VehicleMovementComponent", this->GetVehicleMovement());
		VehicleController->BlackboardComponent->SetValueAsObject("WayPoint", this->WayPoint);
		VehicleController->BlackboardComponent->SetValueAsBool("IsStopSignAhead", this->WayPoint->isStopSignConnected);
		VehicleController->BlackboardComponent->SetValueAsVector("VehicleWorldLocation", this->GetActorLocation());
		VehicleController->BlackboardComponent->SetValueAsFloat("DesiredVelocity", this->WayPoint->SpeedLimit);
		VehicleController->BlackboardComponent->SetValueAsFloat("VelocityStatus01", 1.0);
		UE_LOG(LogEngine, Warning, TEXT("initializing velocityStatus as %f!===================="), VehicleController->BlackboardComponent->GetValueAsFloat("VelocityStatus01"))
	}
}

void AWheeledVehicleObject::EstimateDistancesForTTC() {
	FVector currentVelocity = VehicleController->BlackboardComponent->GetValueAsVector("VehicleVelocity");
	float currentVelocityMagnitude = currentVelocity.Size();
	float distanceAlreadyTravelled = VehicleController->WayPoint->GetDistanceAlongSpline(this->GetActorLocation());
	for (int i = 0; i < 150; i++) {
		FutureTimeArray.Push(int32(distanceAlreadyTravelled - (i * 0.3 * currentVelocityMagnitude)));
		UE_LOG(LogEngine, Warning, TEXT("The distance covered by current frame is "), int32(distanceAlreadyTravelled - (i * 0.3 * currentVelocityMagnitude)));
	}
}
