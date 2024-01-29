// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/GenericApplication.h"
#include "WindowsHookActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWinEvent, FString, Str);

UCLASS()
class CUSTOMRENDERWINDOWS_API AWindowsHookActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWindowsHookActor();
	UPROPERTY(BlueprintAssignable, Category = "Momo")
		FWinEvent OnReceiveWinEvent;

	UFUNCTION(BlueprintCallable, Category = "Momo")
		void HookMouseMM();

	UFUNCTION(BlueprintCallable, Category = "Momo")
		void UnHookMouseMM();

	static AWindowsHookActor* GetInstance(UWorld* World);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	static void revCMD(FString cmd);

private:
	static UWorld* currentWorld;
	static AWindowsHookActor* SingletonInstance;


};
