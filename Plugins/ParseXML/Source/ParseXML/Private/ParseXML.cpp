// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ParseXML.h"
#include "ParseXMLStyle.h"
#include "ParseXMLCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "fileParser.h"
#include "calcForkingPoints.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "LevelEditor.h"
#include "ScenarioGenerator.h"

using json = nlohmann::json;
static const FName ParseXMLTabName("Make Roads");

#define LOCTEXT_NAMESPACE "FParseXMLModule"

FParseXMLModule::FParseXMLModule() {
	
}
FParseXMLModule::~FParseXMLModule() {

}

void FParseXMLModule::StartupModule()
{
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FParseXMLStyle::Initialize();
	FParseXMLStyle::ReloadTextures();

	FParseXMLCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FParseXMLCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FParseXMLModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FParseXMLModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FParseXMLModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FParseXMLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FParseXMLStyle::Shutdown();

	FParseXMLCommands::Unregister();
}

/*
Plugin button click for multiple intersection generation based on the Python run scripts. 
Either this one or plugin button click for single intersection should be used.
*/
void FParseXMLModule::PluginButtonClicked()
{

	// Put your "OnButtonClicked" stuff here

	const FString& windowTitle = "Browse XML Files";
	FString defaultFilePath = "C:/Users/iparanja/Documents/Unreal Projects/NewS2U/Sumo2Unreal_4.22/Road_Network_Files/road_intersections";
	std::string jsonFilePath = "C:/Users/iparanja/Documents/Unreal Projects/NewS2U/Sumo2Unreal_4.22/Content/Python/Data/";
	const FString& defaultFileName = "SumoToUnreal.cpp";
	const FString& defaultFileType = "*.jpg";
	FVector multipleSpawningOffset(0.0f, 0.0f, 0.0f);
	UWorld* World = GEditor->GetEditorWorldContext().World();
	FVector Location = FVector(0.0f, 0.0f, 2000.0f);
	FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParameters;
	FLightingBuildOptions LightOptions;

	TArray <FString> originalOutFileNames = {};
	TArray <FString>& OutFilenames = originalOutFileNames;

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = nullptr;
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid())
	{
		ParentWindowWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
		World->SpawnActor<AAtmosphericFog>(Location, Rotation, SpawnParameters);
		Location.Z = 100000.0f;
		ASkyLight* Skylight = World->SpawnActor<ASkyLight>(Location, Rotation, SpawnParameters);
		if (Skylight != nullptr) {
			Skylight->GetLightComponent()->SetIntensity(5.0f);
		}
		json output_json;
		std::ofstream output(jsonFilePath + "intersectionScores.json");

        /*
		* Create initial population
		*/
		GEngine->Exec(nullptr, TEXT("py \"run.py\""));
		/*
		* Accept and run for 3 generations
		* NOTE ------- Nested for loops. Try to avoid -------------
		*/
		for (int k = 0; k < 3; k++) {
			multipleSpawningOffset.X += float(k) * 60000.0f;
			for (int i = 0; i < 3; i++)
			{
				multipleSpawningOffset.X += float(i)*20000.0f;
				FString finalPath = FString(TEXT("/four_one_"));
				finalPath.AppendInt(i);
				FString selectedFile = defaultFilePath + finalPath + FString(TEXT(".xml")); //object destroyed when scope has been left so no need to remove filename added.

				/*
				Uncomment these lines if you do not want any debug UE_LOG statements
				GEngine->Exec(nullptr, TEXT("Log LogTemp off")); //comment (1/2) to see log messages
				GEngine->Exec(nullptr, TEXT("Log LogEngine off")); //comment (2/2) to see log messages
				*/


				UE_LOG(LogTemp, Warning, TEXT("==================== Parsing file %s ==========================="), *(selectedFile));
				UfileParser fileParser(*selectedFile, multipleSpawningOffset, (k+1));
				UfileParser* intersectionPointer = &fileParser;
				fileParser.loadxml();
				calcForkingPoints forkingPoints(intersectionPointer);
				int32 numberOfForkingPoints = forkingPoints.calculateForkingPointsCount(&(intersectionPointer->SplineContainer));
				UE_LOG(LogTemp, Warning, TEXT("====================Forking points are %d"), numberOfForkingPoints);

				/*
				Log the output scores of the road intersection to a json file.
				*/
				output_json["intersection" + std::to_string(i)]["forking_points"] = numberOfForkingPoints;
				/*
				UE_LOG(LogTemp, Warning, TEXT("========================On Screen Debug message display is %d"), GEngine->bEnableOnScreenDebugMessagesDisplay);
				UE_LOG(LogTemp, Warning, TEXT("========================On Screen Debug message display is %d"), GEngine->bEnableOnScreenDebugMessages);
				GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::Red, FString::Printf(TEXT("Intersection number %d"), i));
				GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::Red, FString::Printf(TEXT("Number of Forking points %d"), numberOfForkingPoints));
				*/
			}
			output << output_json << std::endl;
			GEngine->Exec(nullptr, TEXT("py \"run2.py\""));
		}
		/*Once the roads are placed, pick a road and spawn a car
		*/
		
		UGameplayStatics::GetAllActorsOfClass(World, AWayPoint::StaticClass(), FoundWaypoints);
		AScenarioGenerator* scenario = World->SpawnActor<AScenarioGenerator>(Location, Rotation, SpawnParameters);
		FVehicleSpecification vSpec;
		vSpec.BT_Path = "BehaviorTree'/Game/BT/Car_BT.Car_BT'";
		int32 spawnedCars = 0;
		for (int i = 0; i < FoundWaypoints.Num(); i++) {
			UE_LOG(LogTemp, Warning, TEXT("==================== Connected splines are %d"), ((AWayPoint*)FoundWaypoints[i])->ConnectedSpline.Num());
			if (((AWayPoint*)FoundWaypoints[i])->TotalDistance >= 5000.0f && spawnedCars <= 5 && ((AWayPoint*)FoundWaypoints[i])->ConnectedSpline.Num() >= 2) {
				vSpec.WayPoint = (AWayPoint*)FoundWaypoints[i];
				scenario->SpawnVehicle(vSpec);
				spawnedCars += 1;
				UE_LOG(LogTemp, Warning, TEXT("==================== Spawn vehicle number %d"), i);
			}
		}
		GEditor->BuildLighting(LightOptions);
	}
}

///*
//Plugin button click for single intersection generation based on the Python run scripts.
//Either this one or plugin button click for single intersection should be used.
//*/
//void FParseXMLModule::PluginButtonClicked()
//{
//
//	// Put your "OnButtonClicked" stuff here
//
//	const FString& windowTitle = "Browse XML Files";
//	FString defaultFilePath = "C:/Users/iparanja/Documents/Unreal Projects/NewS2U/Sumo2Unreal_4.22/Road_Network_Files/road_intersections";
//	std::string jsonFilePath = "C:/Users/iparanja/Documents/Unreal Projects/NewS2U/Sumo2Unreal_4.22/Content/Python/Data/";
//	const FString& defaultFileName = "SumoToUnreal.cpp";
//	const FString& defaultFileType = "*.jpg";
//	FVector multipleSpawningOffset(0.0f, 0.0f, 0.0f);
//	UWorld* World = GEditor->GetEditorWorldContext().World();
//	FVector Location = FVector(0.0f, 0.0f, 2000.0f);
//	FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);
//	FActorSpawnParameters SpawnParameters;
//	FLightingBuildOptions LightOptions;
//
//	TArray <FString> originalOutFileNames = {};
//	TArray <FString>& OutFilenames = originalOutFileNames;
//
//	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
//
//	const void* ParentWindowWindowHandle = nullptr;
//	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
//	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
//	if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid())
//	{
//		ParentWindowWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
//		World->SpawnActor<AAtmosphericFog>(Location, Rotation, SpawnParameters);
//		Location.Z = 100000.0f;
//		ASkyLight* Skylight = World->SpawnActor<ASkyLight>(Location, Rotation, SpawnParameters);
//		if (Skylight != nullptr) {
//			Skylight->GetLightComponent()->SetIntensity(5.0f);
//			GEditor->BuildLighting(LightOptions);
//		}
//
//	
//		if (DesktopPlatform->OpenFileDialog(ParentWindowWindowHandle, windowTitle, defaultFilePath, defaultFileName,
//					defaultFileType, 0x00, OutFilenames))
//		{
//			FString selectedFile = OutFilenames[0];
//			UfileParser fileParser(*selectedFile);
//			fileParser.loadxml();
//
//			/*
//			Uncomment these lines if you do not want any debug UE_LOG statements
//			GEngine->Exec(nullptr, TEXT("Log LogTemp off")); //comment (1/2) to see log messages
//			GEngine->Exec(nullptr, TEXT("Log LogEngine off")); //comment (2/2) to see log messages
//			*/
//		}
//
//		UGameplayStatics::GetAllActorsOfClass(World, AWayPoint::StaticClass(), FoundWaypoints);
//		FVehicleSpecification vSpec;
//		vSpec.BT_Path = "BehaviorTree'/BT_Plugin/BT/BehaviorTree.BehaviorTree'";
//		vSpec.WayPoint = (AWayPoint*)FoundWaypoints[30];
//		AScenarioGenerator* scenario = World->SpawnActor<AScenarioGenerator>(Location, Rotation, SpawnParameters);
//		scenario->SpawnVehicle(vSpec);
//	}
//}


void FParseXMLModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FParseXMLCommands::Get().PluginAction);
}

void FParseXMLModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FParseXMLCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FParseXMLModule, ParseXML)