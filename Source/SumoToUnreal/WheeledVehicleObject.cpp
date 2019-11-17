// Fill out your copyright notice in the Description page of Project Settings.


#include "WheeledVehicleObject.h"
#include <Runtime\AIModule\Classes\BehaviorTree\BlackboardComponent.h>
#include "WheeledVehicleMovementComponent4W.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"

//runs first when simulate
AWheeledVehicleObject::AWheeledVehicleObject()
{
	PrimaryActorTick.bCanEverTick = true;
	PrintLog("Inside vehicle object constructor ");
}

//runs fourth time
void AWheeledVehicleObject::BeginPlay()
{
	Super::BeginPlay();

	PrintLog("Inside vehicle object beginplay");
	
	VehicleAIController = GetController<AVehicleAIController>();
	BlackBoardData = VehicleAIController->BlackboardComp;
	if (BlackBoardData == NULL && VehicleAIController == NULL) return;

	//setting up variable in the blackboard
	BlackBoardData->SetValueAsObject("WayPoint", WayPoint);
	BlackBoardData->SetValueAsObject("VehicleObject", this);

	float distance_along_spline = GetDistanceAlongSpline(GetActorLocation(), WayPoint->SplineComponent);
	BlackBoardData->SetValueAsFloat("DistanceAlongWayPoint", distance_along_spline);
	PrintLog("Get dsitance along spline " + FString::SanitizeFloat(distance_along_spline));
	
	
}

void AWheeledVehicleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PrintLog("Inside vehicle object tick");
	
	float brake_value = BlackBoardData->GetValueAsFloat("BrakeValue");
	float steer_value = BlackBoardData->GetValueAsFloat("SteerValue");
	float throttle_value = BlackBoardData->GetValueAsFloat("ThrottleValue");
	
	this->GetVehicleMovement()->SetBrakeInput(brake_value);
	this->GetVehicleMovement()->SetSteeringInput(steer_value);
	this->GetVehicleMovement()->SetThrottleInput(throttle_value);
	
	
}



float AWheeledVehicleObject::GetDistanceAlongSpline(FVector InWorldLocation, USplineComponent* InSpline)
{
	if (!InSpline)
		return 0.f;

	auto InputKeyFloat = InSpline->FindInputKeyClosestToWorldLocation(InWorldLocation);
	auto InputKey = FMath::TruncToInt(InputKeyFloat);
	auto A = InSpline->GetDistanceAlongSplineAtSplinePoint(InputKey);
	auto B = InSpline->GetDistanceAlongSplineAtSplinePoint(InputKey + 1);

	return A + ((B - A) * (InputKeyFloat - InputKey));
}