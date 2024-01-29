// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include <functional>
#include "Misc/Paths.h"

typedef DWORD COLORREF;
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

class FCustomRenderWindowsModule : public IModuleInterface
{
public:
	/*typedef std::function<void(const wchar_t*)> FileDropHandler;
	typedef void (*_SetFileDropHandler)(FileDropHandler handler);
	_SetFileDropHandler SetFileDropHandler = nullptr;
	typedef void(*_CreateAndShowWindow)(int x, int y, int nWidth, int nHeight, COLORREF col, int alpha);
	_CreateAndShowWindow CreateAndShowWindow=nullptr;
	typedef void(*_mHideWindow)();
	_mHideWindow mHideWindow = nullptr;
	typedef void(*_mShowWindow)();
	_mShowWindow mShowWindow = nullptr;
	typedef void(*_mDestroyWindowInstance)();
	_mDestroyWindowInstance mDestroyWindowInstance = nullptr;*/

	typedef std::function<void(const wchar_t*)> FileDropHandler;
	typedef void (*_SetFileDropHandler)(FileDropHandler handler);
	static _SetFileDropHandler SetFileDropHandler;
	typedef void(*_CreateAndShowWindow)(int x, int y, int nWidth, int nHeight, COLORREF col, int alpha);
	static _CreateAndShowWindow CreateAndShowWindow;
	typedef void(*_SetWindowPositionAndSize)(int x, int y, int nWidth, int nHeight);
	static _SetWindowPositionAndSize SetWindowPositionAndSize;

	typedef void(*_mHideWindow)();
	static _mHideWindow mHideWindow;
	typedef void(*_mShowWindow)();
	static _mShowWindow mShowWindow;
	typedef void(*_mDestroyWindowInstance)();
	static _mDestroyWindowInstance mDestroyWindowInstance;

	static void* DLLHandle;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void loadDLL();
	void unloadDLL();
	//static void create_window(int x, int y, int nWidth, int nHeight, COLORREF col, int alpha);
	//static void set_window_position_size(int x, int y, int nWidth, int nHeight);
	//static void destory_window();
};

