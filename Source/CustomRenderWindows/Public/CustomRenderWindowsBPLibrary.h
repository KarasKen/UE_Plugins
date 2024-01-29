// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomRenderWindowsBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UCustomRenderWindowsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()	

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "CustomRenderWindows sample test testing"), Category = "CustomRenderWindowsTesting")
	static float CustomRenderWindowsSampleFunction(float Param);

	

public:
	
	//�Ƿ�����������Ⱦ
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void ActiveDesktopRender( bool Enable);

	//��Ⱦ�������ֽ��
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void RenderToDesktop(bool FistTime,bool Enable, FIntPoint ScreenRes);

	//��������Widget�Ľ������ϵͳ��������ʾ���ķֱ��ʵĸı䣬��������õ�widget��click�¼�ʧЧ
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void ResetFocus();

	//�õ��Ƿ���������״̬
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	bool IsInDesktopMode();

	//ˢ�������ֽ
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void BP_SetWallpaper(const FString& Filename);


	//����Ļ�ڱ�ֽ�������
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint BP_MainScreenInDesktopCS();


	//��ǰ�����ڱ�ֽ�������
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint BP_CurrentViewportInDesktopCS();

	//��ǰUE����������ʾ���ķֱ���
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint GetCurrentUEViewportMonitorRes();

	//��ǰ��������Ļ������
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint BP_CurrentViewportInScreenCS();

	//��ǰ����������ʾ������Ļ������
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint GetCurrentUEViewportMonitorInMonitorCS();

	//��ǰ����������ʾ���ڱ�ֽ�������
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint GetCurrentUEViewportMonitorInDesktopCS();

	//Mesh��ǰ��Ļ������
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static void BP_CurrentMeshOnScreenCS(UObject* Mesh, FVector2D& MinPoint, FVector2D& MaxPoint);

	//ˢ������λ��
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void BP_RefreshUEWindow();

	//������ת��ͼƬ���浽ָ��·��
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	bool SaveTextureAsPNG(UTexture2D* Texture, const FString& Filename);

	//�Ѵ�����ʾ�����ϲ�

	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void WindowOnTop();


	//ʹ��������������

	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void FocusOnDesktop();

	//ʹ��������������

	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void FocusOnOutDesktop();

	//�����Ƿ���������

	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	bool IsFocusOnDesktop(FString& CurrentFocuseHWNDClassNane);



	//�ѽ���ŵ�UE��ȫ��GPU����
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void FocusOnUE( bool Enable);

	//�����Ƿ���UE��
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	bool IsFocusOnUE();
	
	//���»������´���
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void RedrawViewport();

	//��Ⱦ����ֹͣ������
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void StopRendering(bool Stop);

	//GPUռ�õ�ֹͣ������
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void StopGPUOccupancy(bool Stop);

	//��������
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void OpenHook();

	//��������
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void CloseHook();


	//��UE�̼߳������
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void SetHighestPriorityUEThreat();


	//��UE�̼߳���ָ���ͨ
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void SetNormalPriorityUEThreat();
	

};
