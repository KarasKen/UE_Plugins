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




//���Windows.h���ͻ����1
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

//�����洢ԭ�ȵĴ��ھ���ĸ������ھ��
static HWND DefaultUEParent = NULL;

//�ж��Ƿ���������Ⱦ
static bool IsIndesktop = false;

//�Ƿ�����������Ⱦ
static bool EnableDesktopRender = false;

//���������¼�
static HWINEVENTHOOK hEventHook = NULL;

//����һ�����ھ�������洢����������Ĵ��ھ��
static HWND CurrentDesktopWindowHandle = NULL;
// ����һ�����ھ�������洢������Ĵ��ھ��
static HWND CurrentWindowHandle = NULL;


//����������ô������������洰������ƫ�Ƶ�
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




//�õ�UE�Ĵ��ھ��
static HWND GetUEWindowHandle()
{
    HWND unrealWindowHandle = nullptr;
    TSharedPtr<SWindow> Window;
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
    {
        // ��ȡ���ӿ�
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


//��ȡ����������
static HWND GetBackground()
{

    //return GetDesktopWindow();
    //��������û�д�����������֪������������workerW�����и�����Program Maneger������ֻҪ
   //��������workW���͵Ĵ��壬��һ�Ƚ����ĸ������ǲ���Program Manager�Ϳ����ҵ���������
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
            return worker;//���ؽ��
        }
    } while (worker != NULL);
    /*û���ҵ�������Ϣ����һ��WorkerW����*/
    SendMessage(hwnd, 0x052C, 0, 0);

    //�ظ����沽��
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
            return worker;//���ؽ��
        }
    } while (worker != NULL);
    return NULL;

}






// ����Ļ�����ת��Ϊ���������
static  POINT screentodesktop(HWND hNormalWnd,bool ScreenToDesktop)
{
    //��ȡ��ͨ����
    RECT rcNormal;
    GetWindowRect(hNormalWnd, &rcNormal);

    //��ӡֵ
   // GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MyMonitorPos = %dx%d"), rcNormal.left, rcNormal.top));

    //�������Ҫӳ�䵽�����ֽ����ֱ�ӷ�����Ļ����
    if (!ScreenToDesktop)
    {
       return { rcNormal.left, rcNormal.top };
    }


    //����ӳ�䵽��ֽ������

    else
    {

        // ��ȡ�����ֽ�㴰�ڵľ��
        HWND hWallpaperWnd = GetBackground();

        //�鿴һ��hWallpaperWnd��ֵ
     /*   char buf[256];
        sprintf(buf, "hWallpaperWnd: %p", hWallpaperWnd);
        GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Yellow, buf);*/

     
        // ��ȡ�����ֽ�㴰�ڵ���Ļ����
        RECT  rcWallpaper;
        GetWindowRect(hWallpaperWnd, &rcWallpaper);

        //GetClientRect(hWallpaperWnd, &rcWallpaper);

        // ����ͨ������Ļ����ת��Ϊ�ͻ�������
        POINT ptNormal = { rcNormal.left, rcNormal.top };
        ScreenToClient(hNormalWnd, &ptNormal);

        //��ӡת�����ֵ
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MyWorkerPos = %dx%d"), ptNormal.x, ptNormal.y));

        //�������ֽ�㴰�ڵ���Ļ����ת��Ϊ�ͻ�������
        POINT ptWallpaper = { rcWallpaper.left, rcWallpaper.top };

        //��ӡ�������Ļ����ֵ
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("WallMonitorPos = %dx%d"), rcWallpaper.left, rcWallpaper.top));
        ScreenToClient(hWallpaperWnd, &ptWallpaper);

        //��ӡ���洰�ڵĿͻ�������
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("WallWorkerPos = %dx%d"), ptWallpaper.x, ptWallpaper.y));

           // ����ͨ���ڵĿͻ�������ת��Ϊ�����ֽ�㴰�ڵĿͻ�������
        MapWindowPoints(hNormalWnd, hWallpaperWnd, &ptNormal, 1);

        //��ӡת������ҵĴ����������ֽ����ϵ�е�����
        //GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("MyWallpaperPos_After = %dx%d"), ptNormal.x, ptNormal.y));

       
        return ptNormal;
    }
}

//���������� C++ ������ʹ�� PathAppendW ��������ֺ��ͻ����ʹʹ���������������ռ��ȫ�������ռ䡣����������£����Գ���ʹ����һ�ַ����������ͻ����ʹ�ú���ָ�������� PathAppendW ������
//��Ϊ����ͷ�ļ�������������һ��PathAppendW����
//typedef BOOL(WINAPI* PathAppendWFunc)(LPWSTR pszPath, LPCWSTR pszMore);







void UCustomRenderWindowsBPLibrary::ActiveDesktopRender(bool Enable)
{
    EnableDesktopRender = Enable;
}

//#pragma optimize( "", on) 
void UCustomRenderWindowsBPLibrary::RenderToDesktop(bool FistTime, bool Enable, FIntPoint ScreenRes)
{
    //�ж��Ƿ�����������Ⱦ�������ڱ༭��ģʽ��Debug�������ܣ���Ϊ�ڱ༭��ģʽ�½���������Ⱦ����鷳
    if (EnableDesktopRender)
    {
        // ��ȡ����������Ĵ��ھ��
    //HWND unrealWindowHandle = FindWindowW(L"UnrealWindow", nullptr);
        HWND unrealWindowHandle = GetUEWindowHandle();
     


        if (DefaultUEParent == NULL)
        {
            DefaultUEParent = ::GetParent(unrealWindowHandle);
        };

        //�õ������ֽ����
        HWND desktopHandle = GetBackground();

        //��ʼ��Ļ����
        float dpiScaleX = 0.0f, dpiScaleY = 0.0f;
        //��ʼ��������ʼλ��
        FIntPoint ScreenPos = { 0,0 };


        //���Ѳ�Ҫ��Ĭ�����ű��������ҵõ���ǰ��Ļ�����ű���
        SetProcessDPIAware();

        //�����Ҫ�õ���������ֵ��ʱ���뼤���������
        HDC screenDC = GetDC(NULL);
        if (screenDC)
        {
            dpiScaleX = static_cast<float>(GetDeviceCaps(screenDC, LOGPIXELSX)) / 96.0f;
            dpiScaleY = static_cast<float>(GetDeviceCaps(screenDC, LOGPIXELSY)) / 96.0f;
            ReleaseDC(NULL, screenDC);
            // GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MonitorDpiScale = %fx%f"), dpiScaleX, dpiScaleY));
        }

        //��õ�ǰ���ڵ���ʾ��

        HMONITOR hMonitor = MonitorFromWindow(unrealWindowHandle, MONITOR_DEFAULTTONEAREST);

        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);



        if (!(ScreenRes.X && ScreenRes.Y))
        {
            ScreenRes.X = static_cast<int32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);          /* ��Ļ��� ���� */
            ScreenRes.Y = static_cast<int32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);         /* ��Ļ�߶� ���� */
        }


        if (FistTime)
        {
            ScreenRes.X = ScreenRes.X * dpiScaleX;          /* ��Ļ��� ���� */
            ScreenRes.Y = ScreenRes.Y * dpiScaleX;
        }

        if (Enable == true)
        {
       
                    





                    ////��С����ԭ
                    //ShowWindow(unrealWindowHandle, SW_MINIMIZE);
                    //ShowWindow(unrealWindowHandle, SW_RESTORE);
                    ScreenPos = { screentodesktop(unrealWindowHandle,true).x , screentodesktop(unrealWindowHandle,true).y };

                    UEViewportOffset = ScreenPos;

                    SetParent(unrealWindowHandle, desktopHandle);


                    //���ô���λ��
                    SetWindowPos(unrealWindowHandle, HWND_TOP, ScreenPos.X, ScreenPos.Y, ScreenRes.X, ScreenRes.Y, SWP_SHOWWINDOW);

                    // ����������Ϊ���洰��
                    SetForegroundWindow(GetBackground());


                    IsIndesktop = true;

              

            

        }




        else if ((Enable == false))
        {
            
                    
                    ScreenPos = { screentodesktop(unrealWindowHandle,false).x , screentodesktop(unrealWindowHandle,false).y };

                    // ����ʾ�����������Ƴ�
                    SetParent(unrealWindowHandle, NULL);


                    //�Ѵ��ڻ�ԭ������ʾ����λ��
                    SetWindowPos(unrealWindowHandle, HWND_TOP, ScreenPos.X, ScreenPos.Y, ScreenRes.X, ScreenRes.Y, SWP_SHOWWINDOW | SWP_FRAMECHANGED| SWP_NOOWNERZORDER);


                    //�ػ洰��
                   // RedrawWindow(unrealWindowHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);


                    //// ���������
                    //ShowWindow(unrealWindowHandle, SW_MAXIMIZE);

                    // ����������Ϊ���洰��
                   // SetForegroundWindow(GetBackground());

                //     ��������widget�������ϵͳ
             /*       if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
                    {
                        
                        GEngine->GameViewport->GetWindow()->GetWidgetFocusedOnDeactivate()
                       
                    };*/


                    GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("MyRes = %dx%d"), ScreenRes.X, ScreenRes.Y));
                    GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("MyScreenPos = %dx%d"), ScreenPos.X, ScreenPos.Y));

                    IsIndesktop = false;
               






                //    �����Ҫ�˳�����ģʽ��Ҫˢ�������ֽ������£��������������
                //    UMyBlueprintFunctionLibrary::BP_RefreshWalolpaper();

           
        };
    }
    
}

void UCustomRenderWindowsBPLibrary::ResetFocus()
{
   
    //// ��ȡ��ǰ�����û������widget
    //TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);

 // �������ü��̽���
                FSlateApplication::Get().SetKeyboardFocus(FWidgetPath(), EFocusCause::SetDirectly);


                // ���������û�����
                FSlateApplication::Get().SetUserFocus(0, FWidgetPath(), EFocusCause::SetDirectly);

                // �������������û�����
                FSlateApplication::Get().SetAllUserFocus(FWidgetPath(), EFocusCause::SetDirectly);

                // ����Ҫ������������ʱ����ResetToDefaultInputSettings����

               /* ��Ҫע����ǣ�����FSlateApplication::Get().ResetToDefaultInputSettings�����Ὣ����������������ΪĬ��ֵ��
               ����ܻ�Ӱ�쵽����Ӧ�ó�����û����顣��ˣ��ڵ��øú���֮ǰ������Ҫ��ϸ�����Ƿ�������Ҫ�����������ã����ڱ�Ҫʱ�����û���*/
                
                //FSlateApplication::Get().ResetToDefaultInputSettings();
             
                
                
             
    
        
    
}

bool UCustomRenderWindowsBPLibrary::IsInDesktopMode()
{
    return IsIndesktop;
}



void UCustomRenderWindowsBPLibrary::BP_SetWallpaper(const FString& Filename)
{

    // �� FString ת��Ϊ TCHAR*
    TCHAR* FilePath = new TCHAR[Filename.Len() + 1];
    FCString::Strcpy(FilePath, Filename.Len() + 1, *Filename);

    // ���±�ֽ
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, FilePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

    // �ͷŷ�����ڴ�
    delete[] FilePath;
    
}
FIntPoint UCustomRenderWindowsBPLibrary::BP_MainScreenInDesktopCS()
{

 
    // ��ȡ���洰�ھ��
    HWND hDesktopWnd = GetDesktopWindow();
    
    // ��ȡ�������洰�ڵ���ʾ�����
    HMONITOR hDesktopMonitor = MonitorFromWindow(hDesktopWnd, MONITOR_DEFAULTTONEAREST);

    // ��ȡ����ʾ���ľ��
    POINT pt = { 0, 0 };
    HMONITOR hPrimaryMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

    // ����������洰�ڵ���ʾ��������ʾ����ͬ���򷵻����洰�ھ��
    if (hDesktopMonitor == hPrimaryMonitor)
    {
        return { screentodesktop(hDesktopWnd,true).x, screentodesktop(hDesktopWnd,true).y };
    }

    // ����ö�����д��ڣ��ҵ�������ʾ���ϵĶ��㴰��
    // ����ö�����д��ڣ��ҵ�������ʾ���ϵĶ��㴰��
    HWND hTopWindow = NULL;
    auto EnumWindowsProc = [](HWND hWnd, LPARAM lParam) -> BOOL
    {
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        if (hMonitor == (HMONITOR)lParam && IsWindowVisible(hWnd))
        {
            // �ҵ�������ʾ���ϵĿɼ����㴰��
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

    //��õ�ǰ���ڵ���ʾ��

    HMONITOR hMonitor = MonitorFromWindow(GetUEWindowHandle(), MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfo);



        Res.X = static_cast<int32>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);          /* ��Ļ��� ���� */
        Res.Y = static_cast<int32>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);         /* ��Ļ�߶� ���� */
    
    return Res;
}



FIntPoint UCustomRenderWindowsBPLibrary::BP_CurrentViewportInScreenCS()
{
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
    {
        HWND UEWindowHandle = nullptr;
        TSharedPtr<SWindow> Window;
        // ��ȡ���ӿ�
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
    //��õ�ǰ���ڵ���ʾ��

    HMONITOR hMonitor = MonitorFromWindow(GetUEWindowHandle(), MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfo);

    //ʹ�� SystemParametersInfo ������ȡ�����ֽ��ľ�������
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
    // ��鴫�ݵ� Mesh �����Ƿ���һ����Ч�� UStaticMeshComponent �� USkeletalMeshComponent ʵ��
    UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Mesh);
    USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh);

    if (!StaticMesh && !SkeletalMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("BP_CurrentMeshOnScreenCS: Invalid mesh component"));
        return;
    }

    // ��ȡ��ǰ��ҿ�����������
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Mesh->GetOuter(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("BP_CurrentMeshOnScreenCS: Invalid player controller"));
        return;
    }



    // ��ȡ����������Ļ�ռ��е����귶Χ
    FBoxSphereBounds Bounds;
    if (StaticMesh)
    {
        Bounds = StaticMesh->CalcBounds(StaticMesh->GetComponentTransform());
    }
    else
    {
        Bounds = SkeletalMesh->CalcBounds(SkeletalMesh->GetComponentTransform());
    }
    //��ʱ����λ��
    FVector Postion;

    //����8��ӳ��㣬�洢bound����Ļ�ϵ������
    FVector2D Point1;
    FVector2D Point2;
    FVector2D Point3;
    FVector2D Point4;
    FVector2D Point5;
    FVector2D Point6;
    FVector2D Point7;
    FVector2D Point8;


    //��ӡֵ
    // GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("Bound.Origin = %f,%f,%f"), Bounds.Origin.X, Bounds.Origin.Y, Bounds.Origin.Z));

    //�õ������漫�޵�
    UGameplayStatics::ProjectWorldToScreen(PlayerController, (Bounds.Origin- Bounds.BoxExtent), Point1);
    Point1 = Point1 + BP_CurrentViewportInScreenCS();
    //�õ����ϼ��޵�
    UGameplayStatics::ProjectWorldToScreen(PlayerController, (Bounds.Origin + Bounds.BoxExtent), Point2);
    Point2 = Point2 + BP_CurrentViewportInScreenCS();
    //�ֱ�õ��������ӳ��
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

    // �ҵ�X��Сֵ
    int32 MinValueX = NumbersX[0];
    for (int32 i = 1; i < NumbersX.Num(); i++)
    {
        if (NumbersX[i] < MinValueX)
        {
            MinValueX = NumbersX[i];
        }
    }
    // �ҵ�X���ֵ
    int32 MaxValueX = NumbersX[0];
    for (int32 i = 1; i < NumbersX.Num(); i++)
    {
        if (NumbersX[i] > MaxValueX)
        {
            MaxValueX = NumbersX[i];
        }
    };

    // �ҵ�Y��Сֵ
    int32 MinValueY = NumbersY[0];
    for (int32 i = 1; i < NumbersY.Num(); i++)
    {
        if (NumbersY[i] < MinValueY)
        {
            MinValueY = NumbersY[i];
        }
    }
    // �ҵ�Y���ֵ
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


        //���ô���λ��
        SetWindowPos(unrealWindowHandle, HWND_TOP, ScreenPos.X, ScreenPos.Y, ScreenRes.X, ScreenRes.Y, SWP_FRAMECHANGED);

        UEViewportOffset = { ScreenPos.X, ScreenPos.Y };

        
    
    }
   


}
bool UCustomRenderWindowsBPLibrary::SaveTextureAsPNG(UTexture2D* Texture, const FString& Filename)
{
    //�����ʽ���Ծ��˳�
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


        // ʹ�� ImageWrapper ��ѹ��������ݱ���Ϊ PNG ��ʽ
        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);


        ImageWrapper->SetRaw(RawFileData.GetData(), RawFileData.Num(), Texture->GetSizeX(), Texture->GetSizeY(), ColorFormat, 8);
        ImageWrapper->GetCompressed();

        bool bSuccess = FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *Filename);


        return  bSuccess;
    }
   
    else if (Extension == "jpg")
    {
        // ��ȡ�����ԭʼ����
        TArray<FColor> RawData;
        FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
        uint8* Data = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
        int32 Width = Mip.SizeX;
        int32 Height = Mip.SizeY;
        FColor* ColorData = reinterpret_cast<FColor*>(Data);
        RawData.Append(ColorData, Width * Height);
        Mip.BulkData.Unlock();

        // ������ת��Ϊ JPEG ��ʽ
        TArray<uint8> CompressedData;
        FImageUtils::ThumbnailCompressImageArray(Width, Height, RawData, CompressedData);
        //FImageUtils::CompressImageArray(Width, Height, RawData, CompressedData);

        // ��ѹ��������ݱ��浽�ļ�
        return FFileHelper::SaveArrayToFile(CompressedData, *Filename);

    }
    return false;
}


void UCustomRenderWindowsBPLibrary::WindowOnTop()
{
#if PLATFORM_WINDOWS

 
            HWND WindowHandle = GetUEWindowHandle();
            //�Ѵ˴��ڷŵ�ϵͳ���㽹��
            SetForegroundWindow(WindowHandle);
            //�Ѵ˽��̷��ڽ�����ǰ��
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
        // ����������Ϊ���洰��
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
        //���������Ƿ���������������������������Ҽ�����
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

    //�ػ洰��
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
     //��ȡ��ǰ���̵ľ��
    DWORD pid= FPlatformProcess::GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS); // ��ָ�����̵����ȼ���ߵ����
    CloseHandle(hProcess);
#endif

#if PLATFORM_MAC
 

#endif
}

void UCustomRenderWindowsBPLibrary::SetNormalPriorityUEThreat()
{
#if PLATFORM_WINDOWS
    //��ȡ��ǰ���̵ľ��
    DWORD pid = FPlatformProcess::GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS); // ��ָ�����̵����ȼ���ߵ����
    CloseHandle(hProcess);
#endif

#if PLATFORM_MAC


#endif
}






#pragma optimize( "", on) 