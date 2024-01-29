// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomRenderWindows.h"

#define LOCTEXT_NAMESPACE "FCustomRenderWindowsModule"

void (*FCustomRenderWindowsModule::SetFileDropHandler)(FileDropHandler handler)=nullptr;
void (*FCustomRenderWindowsModule::CreateAndShowWindow)(int x, int y, int nWidth, int nHeight, COLORREF col, int alpha) = nullptr;
void (*FCustomRenderWindowsModule::SetWindowPositionAndSize)(int x, int y, int nWidth, int nHeight) = nullptr;
void (*FCustomRenderWindowsModule::mHideWindow)() = nullptr;
void (*FCustomRenderWindowsModule::mShowWindow)() = nullptr;
void (*FCustomRenderWindowsModule::mDestroyWindowInstance)() = nullptr;
void *FCustomRenderWindowsModule::DLLHandle = nullptr;

void FCustomRenderWindowsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    loadDLL();
}

void FCustomRenderWindowsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    unloadDLL();
}

void FCustomRenderWindowsModule::loadDLL()
{
    FCustomRenderWindowsModule::DLLHandle = FPlatformProcess::GetDllHandle(*FPaths::Combine(*FPaths::ProjectPluginsDir(), TEXT("CustomRenderWindows/Source/CustomRenderWindows/ThirdParty/sstools.dll"))); // Fill in the relative path to your DLL file here

    if (FCustomRenderWindowsModule::DLLHandle != nullptr)
    {
        //create windows function

        FString procName1 = "CreateAndShowWindow";
        FCustomRenderWindowsModule::CreateAndShowWindow = (_CreateAndShowWindow)FPlatformProcess::GetDllExport(DLLHandle, *procName1);
        if (FCustomRenderWindowsModule::CreateAndShowWindow == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Import CreateAndShowWindow failed!"));
        }

        //bind drop file callback function
        FString procName2 = "SetFileDropHandler";
        FCustomRenderWindowsModule::SetFileDropHandler = (_SetFileDropHandler)FPlatformProcess::GetDllExport(DLLHandle, *procName2);

        if (FCustomRenderWindowsModule::SetFileDropHandler != NULL)
        {
            //SetFileDropHandler(OnFileDrop);   
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Import SetFileDropHandler failed!"));
        }

        //hide window function
        FString procName3 = "mHideWindow";
        FCustomRenderWindowsModule::mHideWindow = (_mHideWindow)FPlatformProcess::GetDllExport(DLLHandle, *procName3);
        if (FCustomRenderWindowsModule::mHideWindow == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Import mHideWindow failed!"));
        }

        //show window function
        FString procName4 = "mShowWindowEx";
        FCustomRenderWindowsModule::mShowWindow = (_mShowWindow)FPlatformProcess::GetDllExport(DLLHandle, *procName4);
        if (FCustomRenderWindowsModule::mShowWindow == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Import mShowWindow failed!"));
        }

        //destroy window
        FString procName5 = "DestroyWindowInstance";
        FCustomRenderWindowsModule::mDestroyWindowInstance = (_mDestroyWindowInstance)FPlatformProcess::GetDllExport(DLLHandle, *procName5);
        if (FCustomRenderWindowsModule::mDestroyWindowInstance == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Import DestroyWindowInstance failed!"));
        }

        FString procName6 = "SetWindowPositionAndSize";
        FCustomRenderWindowsModule::SetWindowPositionAndSize = (_SetWindowPositionAndSize)FPlatformProcess::GetDllExport(DLLHandle, *procName6);
        if (FCustomRenderWindowsModule::SetWindowPositionAndSize == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Import SetWindowPositionAndSize failed!"));
        }
    }
    else
    {
        FString dll_path = FPaths::Combine(*FPaths::ProjectPluginsDir(), TEXT("CustomRenderWindows/ThirdParty/sstools.dll"));
        UE_LOG(LogTemp, Error, TEXT("load dll failed! dll path:%s"), *dll_path);
    }

}

void FCustomRenderWindowsModule::unloadDLL() {
    if (FCustomRenderWindowsModule::DLLHandle != nullptr)
    {
        FPlatformProcess::FreeDllHandle(FCustomRenderWindowsModule::DLLHandle);
        FCustomRenderWindowsModule::DLLHandle = nullptr;
    }
}
//
//void FCustomRenderWindowsModule::create_window(int x, int y, int nWidth, int nHeight, COLORREF col, int alpha)
//{
//    if (FCustomRenderWindowsModule::CreateAndShowWindow != nullptr) {
//        FCustomRenderWindowsModule::CreateAndShowWindow(x, y, nWidth, nHeight, col, alpha);
//    }
//}
//
//void FCustomRenderWindowsModule::set_window_position_size(int x, int y, int nWidth, int nHeight) {
//    if (FCustomRenderWindowsModule::SetWindowPositionAndSize != nullptr) {
//        FCustomRenderWindowsModule::SetWindowPositionAndSize(x, y, nWidth, nHeight);
//    }
//}
//
//void FCustomRenderWindowsModule::destory_window()
//{
//    if (FCustomRenderWindowsModule::mDestroyWindowInstance != nullptr) {
//        FCustomRenderWindowsModule::mDestroyWindowInstance();
//    }
//}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomRenderWindowsModule, CustomRenderWindows)