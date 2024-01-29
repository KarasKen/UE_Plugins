// Copyright Epic Games, Inc. All Rights Reserved.


#include "CustomRenderWindowsBPLibrary.h"
#include "CustomRenderWindows.h"
#include "Logging/LogCategory.h"
#include "Engine.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"




//解决Windows.h宏冲突方法1
#include "Windows/MinWindows.h"
#include "Shobjidl.h"
#include <Shlobj.h>
#include "TextureResource.h"







//#pragma comment(lib, "shlwapi.lib")
UCustomRenderWindowsBPLibrary::UCustomRenderWindowsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UCustomRenderWindowsBPLibrary::CustomRenderWindowsSampleFunction(float Param)
{
	return -1;
}

 #pragma optimize("", off)

//用来存储原先的窗口句柄的父级窗口句柄
static HWND DefaultUEParent = NULL;

//判断是否是桌面渲染
static bool IsIndesktop = false;

//是否启用桌面渲染
static bool EnableDesktopRender = false;

//创建钩子事件
static HWINEVENTHOOK hEventHook = NULL;

//创建一个窗口句柄用来存储鼠标点击桌面后的窗口句柄
static HWND CurrentDesktopWindowHandle = NULL;
// 创建一个窗口句柄用来存储鼠标点击的窗口句柄
static HWND CurrentWindowHandle = NULL;


//用来储存虚幻窗口坐标与桌面窗口坐标偏移的
static FIntPoint UEViewportOffset = { 0,0 };

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {

    wchar_t className[256];
    GetClassName(hwnd, className, sizeof(className));
    UE_LOG(LogTemp, Log, TEXT("%s"), *FString(className));
    CurrentWindowHandle = hwnd;
    
    if ((std::wcscmp(className, L"WorkerW") == 0)) {
        CurrentDesktopWindowHandle = hwnd;
    }
    else {
        ;
    }
  ;
}




//得到UE的窗口句柄
static HWND GetUEWindowHandle()
{
    HWND unrealWindowHandle = nullptr;
    TSharedPtr<SWindow> Window;
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
    {
        // 获取主视口
        Window = GEngine->GameViewport->GetWindow();
        const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow();

        if (NativeWindow.IsValid())
        {
            unrealWindowHandle = (HWND)NativeWindow->GetOSWindowHandle();

            return unrealWindowHandle;
        }

    }

    return NULL;
};

static void CopyTextureData(UTexture2D* Texture, TArray<FColor>& OutData)
{

 
}


//获取背景窗体句柄
static HWND GetBackground()
{

    //return GetDesktopWindow();
    //背景窗体没有窗体名，但是知道它的类名是workerW，且有父窗体Program Maneger，所以只要
   //遍历所有workW类型的窗体，逐一比较它的父窗体是不是Program Manager就可以找到背景窗体
    HWND hwnd = FindWindowA("progman", "Program Manager");
    HWND worker = NULL;
    do {
        worker = FindWindowExA(NULL, worker, "workerW", NULL);
        if (worker != NULL) {
            char buff[200] = { 0 };
            int ret = GetClassNameA(worker, (PCHAR)buff, sizeof(buff) * 2);
            if (ret == 0) {
                return NULL;
            }
        }
        if (GetParent(worker) == hwnd) {
            return worker;//返回结果
        }
    } while (worker != NULL);
    /*没有找到发送消息生成一个WorkerW窗体*/
    SendMessage(hwnd, 0x052C, 0, 0);

    //重复上面步骤
    do {
        worker = FindWindowExA(NULL, worker, "workerW", NULL);
        if (worker != NULL) {
            char buff[200] = { 0 };
            int ret = GetClassNameA(worker, (PCHAR)buff, sizeof(buff) * 2);
            if (ret == 0) {
                return NULL;
            }
        }
        if (GetParent(worker) == hwnd) {
            return worker;//返回结果
        }
    } while (worker != NULL);
    return NULL;

}






// 将屏幕坐标点转换为桌面坐标点
static  POINT screentodesktop(HWND hNormalWnd,bool ScreenToDesktop)
{
    //获取普通窗口
    RECT rcNormal;
    GetWindowRect(hNormalWnd, &rcNormal);

    //打印值
   // GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MyMonitorPos = %dx%d"), rcNormal.left, rcNormal.top));

    //如果不需要映射到桌面壁纸，怎直接返回屏幕坐标
    if (!ScreenToDesktop)
    {
       return { rcNormal.left, rcNormal.top };
    }


    //否则映射到壁纸层坐标

    else
    {

        // 获取桌面壁纸层窗口的句柄
        HWND hWallpaperWnd = GetBackground();

        //查看一下hWallpaperWnd的值
     /*   char buf[256];
        sprintf(buf, "hWallpaperWnd: %p", hWallpaperWnd);
        GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Yellow, buf);*/

     
        // 获取桌面壁纸层窗口的屏幕坐标
        RECT  rcWallpaper;
        GetWindowRect(hWallpaperWnd, &rcWallpaper);

        //GetClientRect(hWallpaperWnd, &rcWallpaper);

        // 将普通窗口屏幕坐标转换为客户区坐标
        POINT ptNormal = { rcNormal.left, rcNormal.top };
        ScreenToClient(hNormalWnd, &ptNormal);

        //打印转换后的值
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MyWorkerPos = %dx%d"), ptNormal.x, ptNormal.y));

        //把桌面壁纸层窗口的屏幕坐标转换为客户区坐标
        POINT ptWallpaper = { rcWallpaper.left, rcWallpaper.top };

        //打印桌面的屏幕坐标值
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("WallMonitorPos = %dx%d"), rcWallpaper.left, rcWallpaper.top));
        ScreenToClient(hWallpaperWnd, &ptWallpaper);

        //打印桌面窗口的客户区坐标
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("WallWorkerPos = %dx%d"), ptWallpaper.x, ptWallpaper.y));

           // 将普通窗口的客户区坐标转换为桌面壁纸层窗口的客户区坐标
        MapWindowPoints(hNormalWnd, hWallpaperWnd, &ptNormal, 1);

        //打印转换后的我的窗口在桌面壁纸坐标系中的坐标
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("MyWallpaperPos_After = %dx%d"), ptNormal.x, ptNormal.y));

       
        return ptNormal;
    }
}

//在虚幻引擎的 C++ 代码中使用 PathAppendW 函数会出现宏冲突，即使使用了完整的命名空间或全局命名空间。在这种情况下，可以尝试使用另一种方法来解决冲突，即使用函数指针来调用 PathAppendW 函数。
//因为不用头文件，所以再声明一个PathAppendW函数
//typedef BOOL(WINAPI* PathAppendWFunc)(LPWSTR pszPath, LPCWSTR pszMore);







void UCustomRenderWindowsBPLibrary::ActiveDesktopRender(bool Enable)
{
    EnableDesktopRender = Enable;
}

//#pragma optimize( "", on) 
void UCustomRenderWindowsBPLibrary::RenderToDesktop(bool FistTime, bool Enable, FIntPoint ScreenRes)
{
    //判断是否启用桌面渲染，方便在编辑器模式下Debug其他功能，因为在编辑器模式下进入桌面渲染会很麻烦
    if (EnableDesktopRender)
    {
        // 获取虚幻引擎程序的窗口句柄
    //HWND unrealWindowHandle = FindWindowW(L"UnrealWindow", nullptr);
        HWND unrealWindowHandle = GetUEWindowHandle();
     


        if (DefaultUEParent == NULL)
        {
            DefaultUEParent = ::GetParent(unrealWindowHandle);
        };

        //得到桌面壁纸层句柄
        HWND desktopHandle = GetBackground();

        //初始屏幕缩放
        float dpiScaleX = 0.0f, dpiScaleY = 0.0f;
        //初始化窗口起始位置
        FIntPoint ScreenPos = { 0,0 };


        //提醒不要给默认缩放比例，并且得到当前屏幕的缩放比例
        SetProcessDPIAware();

        //如果需要用到窗口缩放值的时候，请激活下面代码
        HDC screenDC = GetDC(NULL);
        if (screenDC)
        {
            dpiScaleX = static_cast<float>(GetDeviceCaps(screenDC, LOGPIXELSX)) / 96.0f;
            dpiScaleY = static_cast<float>(GetDeviceCaps(screenDC, LOGPIXELSY)) / 96.0f;
            ReleaseDC(NULL, screenDC);
            // GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MonitorDpiScale = %fx%f"), dpiScaleX, dpiScaleY));
        }

        //获得当前窗口的显示器

        HMONITOR hMonitor = MonitorFromWindow(unrealWindowHandle, MONITOR_DEFAULTTONEAREST);

        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);



        if (!(ScreenRes.X && ScreenRes.Y))
        {
            ScreenRes.X = static_cast<int32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);          /* 屏幕宽度 像素 */
            ScreenRes.Y = static_cast<int32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);         /* 屏幕高度 像素 */
        }


        if (FistTime)
        {
            ScreenRes.X = ScreenRes.X * dpiScaleX;          /* 屏幕宽度 像素 */
            ScreenRes.Y = ScreenRes.Y * dpiScaleX;
        }

        if (Enable == true)
        {
       
                    





                    ////最小化后复原
                    //ShowWindow(unrealWindowHandle, SW_MINIMIZE);
                    //ShowWindow(unrealWindowHandle, SW_RESTORE);
                    ScreenPos = { screentodesktop(unrealWindowHandle,true).x , screentodesktop(unrealWindowHandle,true).y };

                    UEViewportOffset = ScreenPos;

                    SetParent(unrealWindowHandle, desktopHandle);


                    //设置窗口位置
                    SetWindowPos(unrealWindowHandle, HWND_TOP, ScreenPos.X, ScreenPos.Y, ScreenRes.X, ScreenRes.Y, SWP_SHOWWINDOW);

                    // 将焦点设置为桌面窗口
                    SetForegroundWindow(GetBackground());


                    IsIndesktop = true;

              

            

        }




        else if ((Enable == false))
        {
            
                    
                    ScreenPos = { screentodesktop(unrealWindowHandle,false).x , screentodesktop(unrealWindowHandle,false).y };

                    // 从显示器的桌面中移除
                    SetParent(unrealWindowHandle, NULL);


                    //把窗口还原到主显示器的位置
                    SetWindowPos(unrealWindowHandle, HWND_TOP, ScreenPos.X, ScreenPos.Y, ScreenRes.X, ScreenRes.Y, SWP_SHOWWINDOW | SWP_FRAMECHANGED| SWP_NOOWNERZORDER);


                    //重绘窗口
                   // RedrawWindow(unrealWindowHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);


                    //// 将窗口最大化
                    //ShowWindow(unrealWindowHandle, SW_MAXIMIZE);

                    // 将焦点设置为桌面窗口
                   // SetForegroundWindow(GetBackground());

                //     重新修正widget焦点管理系统
             /*       if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
                    {
                        
                        GEngine->GameViewport->GetWindow()->GetWidgetFocusedOnDeactivate()
                       
                    };*/


                    GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MyRes = %dx%d"), ScreenRes.X, ScreenRes.Y));
                    GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("MyScreenPos = %dx%d"), ScreenPos.X, ScreenPos.Y));

                    IsIndesktop = false;
               






                //    如果需要退出桌面模式需要刷新桌面壁纸的情况下，可启用下面代码
                //    UMyBlueprintFunctionLibrary::BP_RefreshWalolpaper();

           
        };
    }
    
}

void UCustomRenderWindowsBPLibrary::ResetFocus()
{
   
    //// 获取当前具有用户焦点的widget
    //TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);

 // 重新设置键盘焦点
                FSlateApplication::Get().SetKeyboardFocus(FWidgetPath(), EFocusCause::SetDirectly);


                // 重新设置用户焦点
                FSlateApplication::Get().SetUserFocus(0, FWidgetPath(), EFocusCause::SetDirectly);

                // 重新设置所有用户焦点
                FSlateApplication::Get().SetAllUserFocus(FWidgetPath(), EFocusCause::SetDirectly);

                // 在需要重置输入设置时调用ResetToDefaultInputSettings函数

               /* 需要注意的是，调用FSlateApplication::Get().ResetToDefaultInputSettings函数会将所有输入设置重置为默认值，
               这可能会影响到您的应用程序的用户体验。因此，在调用该函数之前，您需要仔细考虑是否真正需要重置输入设置，并在必要时提醒用户。*/
                
                //FSlateApplication::Get().ResetToDefaultInputSettings();
             
                
                
             
    
        
    
}

bool UCustomRenderWindowsBPLibrary::IsInDesktopMode()
{
    return IsIndesktop;
}



void UCustomRenderWindowsBPLibrary::BP_SetWallpaper(const FString& Filename)
{

    // 将 FString 转换为 TCHAR*
    TCHAR* FilePath = new TCHAR[Filename.Len() + 1];
    FCString::Strcpy(FilePath, Filename.Len() + 1, *Filename);

    // 更新壁纸
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, FilePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

    // 释放分配的内存
    delete[] FilePath;
    
}
FIntPoint UCustomRenderWindowsBPLibrary::BP_MainScreenInDesktopCS()
{

 
    // 获取桌面窗口句柄
    HWND hDesktopWnd = GetDesktopWindow();
    
    // 获取包含桌面窗口的显示器句柄
    HMONITOR hDesktopMonitor = MonitorFromWindow(hDesktopWnd, MONITOR_DEFAULTTONEAREST);

    // 获取主显示器的句柄
    POINT pt = { 0, 0 };
    HMONITOR hPrimaryMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

    // 如果包含桌面窗口的显示器和主显示器相同，则返回桌面窗口句柄
    if (hDesktopMonitor == hPrimaryMonitor)
    {
        return { screentodesktop(hDesktopWnd,true).x, screentodesktop(hDesktopWnd,true).y };
    }

    // 否则，枚举所有窗口，找到在主显示器上的顶层窗口
    // 否则，枚举所有窗口，找到在主显示器上的顶层窗口
    HWND hTopWindow = NULL;
    auto EnumWindowsProc = [](HWND hWnd, LPARAM lParam) -> BOOL
    {
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        if (hMonitor == (HMONITOR)lParam && IsWindowVisible(hWnd))
        {
            // 找到在主显示器上的可见顶层窗口
            HWND hTopWnd = GetAncestor(hWnd, GA_ROOTOWNER);
            if (hTopWnd != NULL)
            {
                *(HWND*)lParam = hTopWnd;
                return FALSE;
            }
        }
        return TRUE;
    };
    EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&hTopWindow);

    return { screentodesktop(hTopWindow,true).x, screentodesktop(hTopWindow,true).y };
}
FIntPoint UCustomRenderWindowsBPLibrary::BP_CurrentViewportInDesktopCS()
{
    return UEViewportOffset;
}

FIntPoint UCustomRenderWindowsBPLibrary::GetCurrentUEViewportMonitorRes()
{
    FIntPoint Res;

    //获得当前窗口的显示器

    HMONITOR hMonitor = MonitorFromWindow(GetUEWindowHandle(), MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfo);



        Res.X = static_cast<int32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);          /* 屏幕宽度 像素 */
        Res.Y = static_cast<int32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);         /* 屏幕高度 像素 */
    
    return Res;
}



FIntPoint UCustomRenderWindowsBPLibrary::BP_CurrentViewportInScreenCS()
{
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
    {
        HWND UEWindowHandle = nullptr;
        TSharedPtr<SWindow> Window;
        // 获取主视口
        Window = GEngine->GameViewport->GetWindow();
        const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow();

        if (NativeWindow.IsValid())
        {
            UEWindowHandle = (HWND)NativeWindow->GetOSWindowHandle();
            return{ screentodesktop(UEWindowHandle,false).x , screentodesktop(UEWindowHandle,false).y };
        }
    }
    return FIntPoint();
}

FIntPoint UCustomRenderWindowsBPLibrary::GetCurrentUEViewportMonitorInMonitorCS()
{

    FIntPoint CS;
    //获得当前窗口的显示器

    HMONITOR hMonitor = MonitorFromWindow(GetUEWindowHandle(), MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfo);

    //使用 SystemParametersInfo 函数获取桌面壁纸层的矩形区域。
    RECT desktopRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);



    CS.X = monitorInfo.rcMonitor.left - desktopRect.left;
    CS.Y = monitorInfo.rcMonitor.top - desktopRect.top;
    return CS;
}

FIntPoint UCustomRenderWindowsBPLibrary::GetCurrentUEViewportMonitorInDesktopCS()
{
    FIntPoint DesktopCS;
    FIntPoint MonitorCS= GetCurrentUEViewportMonitorInMonitorCS();
    DesktopCS = GetCurrentUEViewportMonitorInMonitorCS() + BP_MainScreenInDesktopCS();
 
    return DesktopCS;
}


void UCustomRenderWindowsBPLibrary::BP_CurrentMeshOnScreenCS(UObject* Mesh, FVector2D& MinPoint, FVector2D& MaxPoint)
{
    // 检查传递的 Mesh 参数是否是一个有效的 UStaticMeshComponent 或 USkeletalMeshComponent 实例
    UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Mesh);
    USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh);

    if (!StaticMesh && !SkeletalMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("BP_CurrentMeshOnScreenCS: Invalid mesh component"));
        return;
    }

    // 获取当前玩家控制器的引用
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Mesh->GetOuter(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("BP_CurrentMeshOnScreenCS: Invalid player controller"));
        return;
    }



    // 获取网格体在屏幕空间中的坐标范围
    FBoxSphereBounds Bounds;
    if (StaticMesh)
    {
        Bounds = StaticMesh->CalcBounds(StaticMesh->GetComponentTransform());
    }
    else
    {
        Bounds = SkeletalMesh->CalcBounds(SkeletalMesh->GetComponentTransform());
    }
    //临时储存位置
    FVector Postion;

    //创建8个映射点，存储bound在屏幕上的坐标点
    FVector2D Point1;
    FVector2D Point2;
    FVector2D Point3;
    FVector2D Point4;
    FVector2D Point5;
    FVector2D Point6;
    FVector2D Point7;
    FVector2D Point8;


    //打印值
    // GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("Bound.Origin = %f,%f,%f"), Bounds.Origin.X, Bounds.Origin.Y, Bounds.Origin.Z));

    //得到左下面极限点
    UGameplayStatics::ProjectWorldToScreen(PlayerController, (Bounds.Origin- Bounds.BoxExtent), Point1);
    Point1 = Point1 + BP_CurrentViewportInScreenCS();
    //得到右上极限点
    UGameplayStatics::ProjectWorldToScreen(PlayerController, (Bounds.Origin + Bounds.BoxExtent), Point2);
    Point2 = Point2 + BP_CurrentViewportInScreenCS();
    //分别得到其他点的映射
    Postion.Set((Bounds.Origin.X + Bounds.BoxExtent.X), (Bounds.Origin.Y + Bounds.BoxExtent.Y), (Bounds.Origin.Z - Bounds.BoxExtent.Z));
    UGameplayStatics::ProjectWorldToScreen(PlayerController, Postion, Point3);
    Point3 = Point3 + BP_CurrentViewportInScreenCS();

    Postion.Set((Bounds.Origin.X + Bounds.BoxExtent.X), (Bounds.Origin.Y - Bounds.BoxExtent.Y), (Bounds.Origin.Z + Bounds.BoxExtent.Z));
    UGameplayStatics::ProjectWorldToScreen(PlayerController, Postion, Point4);
    Point4 = Point4 + BP_CurrentViewportInScreenCS();

    Postion.Set((Bounds.Origin.X + Bounds.BoxExtent.X), (Bounds.Origin.Y - Bounds.BoxExtent.Y), (Bounds.Origin.Z - Bounds.BoxExtent.Z));
    UGameplayStatics::ProjectWorldToScreen(PlayerController, Postion, Point5);
    Point5 = Point5 + BP_CurrentViewportInScreenCS();

    Postion.Set((Bounds.Origin.X - Bounds.BoxExtent.X), (Bounds.Origin.Y + Bounds.BoxExtent.Y), (Bounds.Origin.Z + Bounds.BoxExtent.Z));
    UGameplayStatics::ProjectWorldToScreen(PlayerController, Postion, Point6);
    Point6 = Point6 + BP_CurrentViewportInScreenCS();

    Postion.Set((Bounds.Origin.X - Bounds.BoxExtent.X), (Bounds.Origin.Y + Bounds.BoxExtent.Y), (Bounds.Origin.Z - Bounds.BoxExtent.Z));
    UGameplayStatics::ProjectWorldToScreen(PlayerController, Postion, Point7);
    Point7 = Point7 + BP_CurrentViewportInScreenCS();

    Postion.Set((Bounds.Origin.X - Bounds.BoxExtent.X), (Bounds.Origin.Y - Bounds.BoxExtent.Y), (Bounds.Origin.Z + Bounds.BoxExtent.Z));
    UGameplayStatics::ProjectWorldToScreen(PlayerController, Postion, Point8);
    Point8 = Point8 + BP_CurrentViewportInScreenCS();

    TArray<int32> NumbersX = { static_cast<int32>(Point1.X), static_cast<int32>(Point2.X), static_cast<int32>(Point3.X), static_cast<int32>(Point4.X), static_cast<int32>(Point5.X), static_cast<int32>(Point6.X), static_cast<int32>(Point7.X), static_cast<int32>(Point8.X) };
    TArray<int32> NumbersY = { static_cast<int32>(Point1.Y), static_cast<int32>(Point2.Y), static_cast<int32>(Point3.Y), static_cast<int32>(Point4.Y), static_cast<int32>(Point5.Y), static_cast<int32>(Point6.Y), static_cast<int32>(Point7.Y), static_cast<int32>(Point8.Y) };

    // 找到X最小值
    int32 MinValueX = NumbersX[0];
    for (int32 i = 1; i < NumbersX.Num(); i++)
    {
        if (NumbersX[i] < MinValueX)
        {
            MinValueX = NumbersX[i];
        }
    }
    // 找到X最大值
    int32 MaxValueX = NumbersX[0];
    for (int32 i = 1; i < NumbersX.Num(); i++)
    {
        if (NumbersX[i] > MaxValueX)
        {
            MaxValueX = NumbersX[i];
        }
    };

    // 找到Y最小值
    int32 MinValueY = NumbersY[0];
    for (int32 i = 1; i < NumbersY.Num(); i++)
    {
        if (NumbersY[i] < MinValueY)
        {
            MinValueY = NumbersY[i];
        }
    }
    // 找到Y最大值
    int32 MaxValueY = NumbersY[0];
    for (int32 i = 1; i < NumbersY.Num(); i++)
    {
        if (NumbersY[i] > MaxValueY)
        {
            MaxValueY = NumbersY[i];
        }
    };
    MinPoint.Set(MinValueX, MinValueY);
    MaxPoint.Set(MaxValueX, MaxValueY);
 
}
void UCustomRenderWindowsBPLibrary::BP_RefreshUEWindow()
{
    if (IsIndesktop)
    {
        HWND unrealWindowHandle = GetUEWindowHandle();
        FIntPoint ScreenPos = GetCurrentUEViewportMonitorInDesktopCS();
        FIntPoint ScreenRes = GetCurrentUEViewportMonitorRes();


        //设置窗口位置
        SetWindowPos(unrealWindowHandle, HWND_TOP, ScreenPos.X, ScreenPos.Y, ScreenRes.X, ScreenRes.Y, SWP_FRAMECHANGED);

        UEViewportOffset = { ScreenPos.X, ScreenPos.Y };

        
    
    }
   


}
bool UCustomRenderWindowsBPLibrary::SaveTextureAsPNG(UTexture2D* Texture, const FString& Filename)
{
    //如果格式不对就退出
    if (Texture->CompressionSettings != TC_EditorIcon)
    {
        return false;
    }

    FString Extension = FPaths::GetExtension(Filename);

    if (Extension == "png")
    {
        ERGBFormat ColorFormat = ERGBFormat::BGRA;
        TArray<uint8> RawFileData;
        FIntPoint Size(Texture->GetSizeX(), Texture->GetSizeY());
        FTexture2DMipMap& MipMap = Texture->GetPlatformData()->Mips[0];
        uint8* Data = (uint8*)MipMap.BulkData.Lock(LOCK_READ_ONLY);
        RawFileData.AddUninitialized(Size.X * Size.Y * sizeof(FColor));
        FMemory::Memcpy(RawFileData.GetData(), Data, RawFileData.Num());
        MipMap.BulkData.Unlock();


        // 使用 ImageWrapper 将压缩后的数据编码为 PNG 格式
        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);


        ImageWrapper->SetRaw(RawFileData.GetData(), RawFileData.Num(), Texture->GetSizeX(), Texture->GetSizeY(), ColorFormat, 8);
        ImageWrapper->GetCompressed();

        bool bSuccess = FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *Filename);


        return  bSuccess;
    }
   
    else if (Extension == "jpg")
    {
        // 获取纹理的原始数据
        TArray<FColor> RawData;
        FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
        uint8* Data = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
        int32 Width = Mip.SizeX;
        int32 Height = Mip.SizeY;
        FColor* ColorData = reinterpret_cast<FColor*>(Data);
        RawData.Append(ColorData, Width * Height);
        Mip.BulkData.Unlock();

        // 将纹理转换为 JPEG 格式
        TArray<uint8> CompressedData;
        FImageUtils::ThumbnailCompressImageArray(Width, Height, RawData, CompressedData);
        //FImageUtils::CompressImageArray(Width, Height, RawData, CompressedData);

        // 将压缩后的数据保存到文件
        return FFileHelper::SaveArrayToFile(CompressedData, *Filename);

    }
    return false;
}


void UCustomRenderWindowsBPLibrary::WindowOnTop()
{
#if PLATFORM_WINDOWS

 
            HWND WindowHandle = GetUEWindowHandle();
            //把此窗口放到系统顶层焦点
            SetForegroundWindow(WindowHandle);
            //把此进程放在进程最前端
            SetWindowPos(WindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
      



#endif


#if PLATFORM_MAC
    NSWindow* WindowHandle = (NSWindow*)FSlateApplication::Get().GetPlatformApplication()->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
    [WindowHandle makeKeyAndOrderFront : nil] ;
    [WindowHandle setLevel : NSFloatingWindowLevel] ;
#endif

}

void UCustomRenderWindowsBPLibrary::FocusOnDesktop()
{
    if (CurrentDesktopWindowHandle)
    {
        SetForegroundWindow(CurrentDesktopWindowHandle);
    }
    else {
        // 将焦点设置为桌面窗口
        SetForegroundWindow(GetBackground());
    }
 
    

}

void UCustomRenderWindowsBPLibrary::FocusOnOutDesktop()
{
    SetForegroundWindow(GetDesktopWindow());
}

bool UCustomRenderWindowsBPLibrary::IsFocusOnDesktop(FString& CurrentFocuseHWNDClassNane)
{
#if PLATFORM_WINDOWS
    HWND Foregroundhwnd = GetForegroundWindow();
    wchar_t className[256];

    if (Foregroundhwnd)
    {
        GetClassName(Foregroundhwnd, className, sizeof(className));
        CurrentFocuseHWNDClassNane = FString(className);
        //看看窗口是否是桌面或者任务栏或者是桌面右键窗口
        if ((std::wcscmp(className, L"WorkerW") == 0)
            ||(std::wcscmp(className, L"Sample Window Class") == 0)
            || (std::wcscmp(className, L"Shell_TrayWnd") == 0)
            || (std::wcscmp(className, L"#32768") == 0) 
            || (std::wcscmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0)
            || (std::wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0) 
            || (std::wcsncmp(className, L"ZJJ",3) == 0)
            || (std::wcscmp(className, L"Progman") == 0))
        {
            return true;
        }

    }
#endif
    return false;
}

void UCustomRenderWindowsBPLibrary::FocusOnUE(bool Enable)
{
     if (Enable)
     {          
           SetForegroundWindow(GetUEWindowHandle());     
      }
}

bool UCustomRenderWindowsBPLibrary::IsFocusOnUE()
{
    if (GetForegroundWindow() == GetUEWindowHandle())
    {
        return true;
    };
    return false;
}

void UCustomRenderWindowsBPLibrary::RedrawViewport()
{

    //重绘窗口
    RedrawWindow(GetUEWindowHandle(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);


}

void UCustomRenderWindowsBPLibrary::StopRendering(bool Stop)
{
    if (GEngine)
    {
        if (Stop)
        {
            GEngine->DeferredCommands.AddUnique(TEXT("rhi.StopRendering();"));

        }
        else
        {
            GEngine->DeferredCommands.AddUnique(TEXT("rhi.StartRendering();"));
        }

    }

}

void UCustomRenderWindowsBPLibrary::StopGPUOccupancy(bool Stop)
{
    if (GEngine)
        if (Stop)
        {
            GEngine->DeferredCommands.AddUnique(TEXT("r.SetStopRenderingWhenBackgrounded(true);"));
        }
        else
        {
            GEngine->DeferredCommands.AddUnique(TEXT("r.SetStopRenderingWhenBackgrounded(false);"));
        }

}

void UCustomRenderWindowsBPLibrary::OpenHook()
{
    if (!hEventHook) 
    {
        hEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    }
}

void UCustomRenderWindowsBPLibrary::CloseHook()
{
    if (hEventHook != NULL) {
        UnhookWinEvent(hEventHook);
        hEventHook = NULL;
    }
}

void UCustomRenderWindowsBPLibrary::SetHighestPriorityUEThreat()
{
#if PLATFORM_WINDOWS
     //获取当前进程的句柄
    DWORD pid= FPlatformProcess::GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS); // 将指定进程的优先级提高到最高
    CloseHandle(hProcess);
#endif

#if PLATFORM_MAC
 

#endif
}

void UCustomRenderWindowsBPLibrary::SetNormalPriorityUEThreat()
{
#if PLATFORM_WINDOWS
    //获取当前进程的句柄
    DWORD pid = FPlatformProcess::GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS); // 将指定进程的优先级提高到最高
    CloseHandle(hProcess);
#endif

#if PLATFORM_MAC


#endif
}






#pragma optimize( "", on) 