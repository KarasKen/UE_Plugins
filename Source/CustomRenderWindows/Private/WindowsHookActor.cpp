// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowsHookActor.h"
#include "Windows/MinWindows.h"


HHOOK MouseHookHandle = NULL;
//HWINEVENTHOOK hEventHook = NULL;
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        if (wParam == WM_LBUTTONUP)
        {

            UE_LOG(LogTemp, Log, TEXT("left click up"));
            AWindowsHookActor::revCMD(FString("left_up"));
        }
        else if (wParam == WM_LBUTTONDOWN)
        {

            UE_LOG(LogTemp, Log, TEXT("left click down"));
            AWindowsHookActor::revCMD(FString("left_down"));
        }
        else if (wParam == WM_RBUTTONUP)
        {

            UE_LOG(LogTemp, Log, TEXT("right click up"));
            AWindowsHookActor::revCMD(FString("right_up"));
        }
        else if (wParam == WM_RBUTTONDOWN)
        {

            UE_LOG(LogTemp, Log, TEXT("right click down"));
            AWindowsHookActor::revCMD(FString("right_down"));
        }
        else if (wParam == WM_LBUTTONDBLCLK) 
        {
            UE_LOG(LogTemp, Log, TEXT("left click double"));
            AWindowsHookActor::revCMD(FString("double_click"));
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


//void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
//
//    wchar_t className[256];
//    HWND desktop = GetDesktopWindow();
//    GetClassName(hwnd, className, sizeof(className));
//    UE_LOG(LogTemp, Log, TEXT("%s"), *FString(className));
//    if ((std::wcscmp(className, L"WorkerW") == 0)) {
//        AWindowsHookActor::revCMD(FString("get_focus_desktop"));
//    }
//    else {
//        AWindowsHookActor::revCMD(FString("lose_focus_desktop"));
//    }
//    uintptr_t hwnd_as_int = reinterpret_cast<uintptr_t>(hwnd);
//    AWindowsHookActor::revCMD(FString("focus windows:")+FString(className)+FString::FromInt(hwnd_as_int));
//}

//static WNDPROC OldWndProc = nullptr;
//
//LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    if (msg == WM_DISPLAYCHANGE)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Display changed!"));
//        AWindowsHookActor::revCMD(FString("display_changed"));
//    }
//    if (OldWndProc != nullptr) {
//        // Call old window procedure
//        return CallWindowProc(OldWndProc, hwnd, msg, wParam, lParam);
//    }
//    else
//    {
//        return DefWindowProc(hwnd, msg, wParam, lParam);
//    }
//    
//}

// Sets default values
AWindowsHookActor::AWindowsHookActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    

}
UWorld* AWindowsHookActor::currentWorld = nullptr;
AWindowsHookActor* AWindowsHookActor::SingletonInstance = nullptr;
AWindowsHookActor* AWindowsHookActor::GetInstance(UWorld* World)
{
    //if (!SingletonInstance) {
    //    SingletonInstance = World ? World->SpawnActor<AWindowsHookActor>() : nullptr;
    //}
    return SingletonInstance;
}

void AWindowsHookActor::HookMouseMM() {
    if (!MouseHookHandle)
    {
        MouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    }
}

void AWindowsHookActor::UnHookMouseMM() {
    if (MouseHookHandle != NULL)
    {
        UnhookWindowsHookEx(MouseHookHandle);
        MouseHookHandle = NULL;
    }
}


// Called when the game starts or when spawned
void AWindowsHookActor::BeginPlay()
{
	Super::BeginPlay();
    currentWorld = GetWorld();
    SingletonInstance = this;
    /*if (SingletonInstance && SingletonInstance != this) {
        #if !WITH_EDITOR
            Destroy();
        #endif
    }
    else if (!SingletonInstance) {
        SingletonInstance = this;
    }*/
    
	

 /*   if (!hEventHook) {
        hEventHook= SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    }*/

    //GenericApplication::FOnDisplayMetricsChanged().AddUObject(this, &AWindowsHookActor::HandleDisplayMetricsChanged);
    // Get window handle
    //HWND hwnd = (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
    
//#if !WITH_EDITOR
//    // Store old window procedure
//    OldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)GlobalWndProc);
//#endif

}

// Called every frame
void AWindowsHookActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWindowsHookActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    
    //if (hEventHook != NULL) {
    //    UnhookWinEvent(hEventHook);
    //    hEventHook = NULL;
    //}
    //OldWndProc = nullptr;
   
}

void AWindowsHookActor::revCMD(FString cmd)
{
    if (currentWorld!=nullptr) {
        AWindowsHookActor::GetInstance(currentWorld)->OnReceiveWinEvent.Broadcast(cmd);
        //UE_LOG(LogTemp, Warning, TEXT("broadcast"));
    }

}
