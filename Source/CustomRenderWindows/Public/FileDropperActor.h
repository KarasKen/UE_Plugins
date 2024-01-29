// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomRenderWindows.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "FileDropperActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDropFileEvent, FString, filepath);
UCLASS()
class CUSTOMRENDERWINDOWS_API AFileDropperActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFileDropperActor();
	UPROPERTY(BlueprintAssignable, Category = "Momo")
		FDropFileEvent OnReceiveDropFileEvent;

	UFUNCTION(BlueprintCallable, Category = "Momo")
		void CreateDropWindow(int x, int y, int nWidth, int nHeight, FColor color, int alpha);
	UFUNCTION(BlueprintCallable, Category = "Momo")
		void HideWindow();
	UFUNCTION(BlueprintCallable, Category = "Momo")
		void ShowWindow();
	UFUNCTION(BlueprintCallable, Category = "Momo")
		void DestroyWindow();

	UFUNCTION(BlueprintCallable, Category = "Momo")
		void SetWinPosSize(int x, int y, int nWidth, int nHeight);

	UFUNCTION(BlueprintCallable, Category="Momo")
		bool MoveFileToRecycleBin(FString filepath);

	static UWorld* currentWorld;
	static AFileDropperActor* GetInstance(UWorld* World);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	
	static AFileDropperActor* SingletonInstance;
	static bool MoveFile2RecycleBin(const FString& FilePath);

};
