#pragma once
// ���� Windows.h ͷ�ļ�
//#include "Windows/MinWindows.h"



// ������Ҫʹ�õ� Windows API ����

namespace CustomRenderWindowAPI
{
#include <Windows.h>
	void MySendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		CustomRenderWindowAPI::SendMessage(hWnd, Msg, wParam, lParam);
	}
}
