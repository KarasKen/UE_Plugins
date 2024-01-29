// Fill out your copyright notice in the Description page of Project Settings.
#include "FileDropperActor.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#undef GetEnvironmentVariable 

typedef DWORD COLORREF;
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))



void OnFileDrop(const wchar_t* filePath);

//----------------------------------

class FMyRunnable : public FRunnable
{
public:
    FMyRunnable(int x, int y, int nWidth, int nHeight, FColor color1, int alpha) :X(x), Y(y),Width(nWidth),Height(nHeight),Color1(color1),Alpha(alpha){ StopTaskCounter.Reset(); }
    virtual bool Init() override { return true; }
    virtual uint32 Run() override
    {
        // 这里是你的子线程代码
 
        while (StopTaskCounter.GetValue() <2 ) {
           
            if (FCustomRenderWindowsModule::SetFileDropHandler != NULL)
            {
                FCustomRenderWindowsModule::SetFileDropHandler(OnFileDrop);
            }
        
            if (FCustomRenderWindowsModule::CreateAndShowWindow != nullptr && StopTaskCounter.GetValue()==0) {
                
                FCustomRenderWindowsModule::CreateAndShowWindow(X, Y, Width, Height, RGB(Color1.R, Color1.G, Color1.B), Alpha);
                
                StopTaskCounter.Increment();
            }
            else {
            
            }
        }
        
        return 0;
    }
    virtual void Stop() override { 
        FCustomRenderWindowsModule::mDestroyWindowInstance();
        StopTaskCounter.Increment(); 
        
    }
    virtual void Exit() override {}

private:
    FThreadSafeCounter StopTaskCounter;
    bool start = false;
    int X;
    int Y;
    int Width;
    int Height;
    FColor Color1;
    int Alpha;
};

FMyRunnable* MyRunnable = nullptr;
FRunnableThread* MyThread = nullptr;
//----------------------------------



// Sets default values
AFileDropperActor::AFileDropperActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    currentWorld = GetWorld();
    

}
void AFileDropperActor::CreateDropWindow(int x, int y, int nWidth, int nHeight, FColor color1, int alpha)
{
    if (MyRunnable == nullptr && MyThread == nullptr) {
        MyRunnable = new FMyRunnable(x, y, nWidth, nHeight, color1, alpha);
        MyThread = FRunnableThread::Create(MyRunnable, TEXT("MyThread"));
    }
    
    
}
void AFileDropperActor::HideWindow()
{
    if (FCustomRenderWindowsModule::mHideWindow != nullptr) {
        FCustomRenderWindowsModule::mHideWindow();
    }
}
void AFileDropperActor::ShowWindow()
{
    if (FCustomRenderWindowsModule::mShowWindow != nullptr) {
        FCustomRenderWindowsModule::mShowWindow();
    }
}
void AFileDropperActor::DestroyWindow()
{
    if (MyThread != nullptr)
    {
        MyRunnable->Stop();
        MyThread->WaitForCompletion();
        delete MyThread;
        MyThread = nullptr;
    }

    // 删除 runnable
    if (MyRunnable != nullptr)
    {
        delete MyRunnable;
        MyRunnable = nullptr;
    }

    if (FCustomRenderWindowsModule::mDestroyWindowInstance) {
        FCustomRenderWindowsModule::mDestroyWindowInstance();
    }
}
void AFileDropperActor::SetWinPosSize(int x, int y, int nWidth, int nHeight)
{
    if (FCustomRenderWindowsModule::SetWindowPositionAndSize != nullptr) {
        FCustomRenderWindowsModule::SetWindowPositionAndSize(x,y,nWidth,nHeight);
    }
}
bool AFileDropperActor::MoveFileToRecycleBin(FString filepath)
{
    return AFileDropperActor::MoveFile2RecycleBin(filepath);
}
AFileDropperActor* AFileDropperActor::SingletonInstance = nullptr;
AFileDropperActor* AFileDropperActor::GetInstance(UWorld* World)
{
	return SingletonInstance;
}

// Called when the game starts or when spawned
void AFileDropperActor::BeginPlay()
{
	Super::BeginPlay();
    currentWorld = GetWorld();
    SingletonInstance = this;

    /*if (IsValid(SingletonInstance) && SingletonInstance != this) {
        #if !WITH_EDITOR
            Destroy();
        #endif
    }
    if (SingletonInstance==NULL||!IsValid(SingletonInstance)) {
        SingletonInstance = this;
        
    }*/
}

// Called every frame
void AFileDropperActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFileDropperActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{   
    
   
    Super::EndPlay(EndPlayReason);
    // 停止线程
    if (MyThread != nullptr)
    {
        MyRunnable->Stop();
        MyThread->WaitForCompletion();
        delete MyThread;
        MyThread = nullptr;
    }

    // 删除 runnable
    if (MyRunnable != nullptr)
    {
        delete MyRunnable;
        MyRunnable = nullptr;
    }
    AFileDropperActor::currentWorld = nullptr;
    Destroy();
    
}



UWorld* AFileDropperActor::currentWorld = nullptr;
void OnFileDrop(const wchar_t* filePath)
{
    if (AFileDropperActor::currentWorld != nullptr && std::wcslen(filePath)>0) {
        UE_LOG(LogTemp, Log, TEXT("file path:%s"), *FString(filePath));
        if (AFileDropperActor::GetInstance(AFileDropperActor::currentWorld) != NULL) {
            AFileDropperActor::GetInstance(AFileDropperActor::currentWorld)->OnReceiveDropFileEvent.Broadcast(FString(filePath));
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("FileDropperActor Instance is NULL!"));
        }
        
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("current world or filePath is NULL!"));
    }
    
}

#if PLATFORM_WINDOWS
#include "Windows/MinWindows.h"
#include "Shellapi.h"

bool AFileDropperActor::MoveFile2RecycleBin(const FString& FilePath)
{
    if (!FPaths::FileExists(FilePath) && !FPaths::DirectoryExists(FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("File or directory does not exist: %s"), *FilePath);
        return false;
    }

    FString AbsolutePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*FilePath);
    
    TCHAR FilePathBuffer[MAX_PATH];
    FCString::Strcpy(FilePathBuffer, *AbsolutePath);

    FilePathBuffer[AbsolutePath.Len()] = 0;
    FilePathBuffer[AbsolutePath.Len() + 1] = 0;

    SHFILEOPSTRUCT FileOp;
    ZeroMemory(&FileOp, sizeof(FileOp));

    FileOp.wFunc = FO_DELETE;
    FileOp.pFrom = FilePathBuffer;
    FileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

    int Result = SHFileOperation(&FileOp);
    if (Result != 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to move file to recycle bin: %s"), *FilePath);
        return false;
    }
    return true;
}
#endif


