#pragma once
// 包含 Windows.h 头文件
//#include "Windows/MinWindows.h"



// 声明需要使用的 Windows API 函数

namespace CustomRenderWindowAPI
{
#include <Windows.h>
	void MySendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		CustomRenderWindowAPI::SendMessage(hWnd, Msg, wParam, lParam);
	}
}
