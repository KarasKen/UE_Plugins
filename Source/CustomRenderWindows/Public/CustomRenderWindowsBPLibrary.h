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
	
	//是否启用桌面渲染
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void ActiveDesktopRender( bool Enable);

	//渲染到桌面壁纸层
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void RenderToDesktop(bool FistTime,bool Enable, FIntPoint ScreenRes);

	//重新设置Widget的焦点管理系统，由于显示器的分辨率的改变，容易让虚幻的widget的click事件失效
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void ResetFocus();

	//得到是否在桌面层的状态
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	bool IsInDesktopMode();

	//刷新桌面壁纸
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void BP_SetWallpaper(const FString& Filename);


	//主屏幕在壁纸层的坐标
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint BP_MainScreenInDesktopCS();


	//当前窗口在壁纸层的坐标
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint BP_CurrentViewportInDesktopCS();

	//当前UE窗口所在显示器的分辨率
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint GetCurrentUEViewportMonitorRes();

	//当前窗口在屏幕的坐标
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint BP_CurrentViewportInScreenCS();

	//当前窗口所在显示器在屏幕的坐标
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint GetCurrentUEViewportMonitorInMonitorCS();

	//当前窗口所在显示器在壁纸层的坐标
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	FIntPoint GetCurrentUEViewportMonitorInDesktopCS();

	//Mesh当前屏幕的坐标
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static void BP_CurrentMeshOnScreenCS(UObject* Mesh, FVector2D& MinPoint, FVector2D& MaxPoint);

	//刷新桌面位置
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void BP_RefreshUEWindow();

	//把纹理转成图片保存到指定路径
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	bool SaveTextureAsPNG(UTexture2D* Texture, const FString& Filename);

	//把窗口显示在最上层

	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void WindowOnTop();


	//使焦点在桌面里面

	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void FocusOnDesktop();

	//使焦点在桌面外面

	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void FocusOnOutDesktop();

	//焦点是否在桌面上

	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	bool IsFocusOnDesktop(FString& CurrentFocuseHWNDClassNane);



	//把焦点放到UE上全力GPU运算
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void FocusOnUE( bool Enable);

	//焦点是否在UE上
	UFUNCTION(BlueprintPure, Category = "MyWindowsTool")
		static	bool IsFocusOnUE();
	
	//重新绘制以下窗口
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void RedrawViewport();

	//渲染进程停止与启用
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void StopRendering(bool Stop);

	//GPU占用的停止与启用
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void StopGPUOccupancy(bool Stop);

	//开启钩子
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void OpenHook();

	//结束钩子
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void CloseHook();


	//把UE线程级别提高
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void SetHighestPriorityUEThreat();


	//把UE线程级别恢复普通
	UFUNCTION(BlueprintCallable, Category = "MyWindowsTool")
		static	void SetNormalPriorityUEThreat();
	

};
