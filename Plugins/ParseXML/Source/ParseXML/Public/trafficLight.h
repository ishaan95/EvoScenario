#pragma once
#include "Engine.h"

class trafficLight
{
	friend class UfileParser;
public:
	trafficLight();
	~trafficLight();

private:
	FString trafficLightID;
	FVector trafficLightLocation;
};