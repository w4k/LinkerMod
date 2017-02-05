#include "gui_d3d.h"
#include "gui_wnd.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
		//case WM_SIZE:
		//	//...
		//	return 0;
	case WM_SYSCOMMAND:
		// Disable ALT application menu
		if ((wParam & 0xFFF0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int GUI_InitWindow(wnd_instance_t* wnd)
{
	//
	// Register Window Class
	//
	WNDCLASSEX* wc = &wnd->wc;
	wc->cbSize = sizeof(WNDCLASSEX);
	wc->style = CS_CLASSDC;
	wc->lpfnWndProc = WndProc;
	wc->cbClsExtra = 0;
	wc->cbWndExtra = 0;
	wc->hInstance = GetModuleHandle(NULL);
	wc->hIcon = NULL; // TODO: Icon
	wc->hCursor = LoadCursor(NULL, IDC_ARROW);
	wc->hbrBackground = NULL;
	wc->lpszMenuName = NULL;
	wc->lpszClassName = WNDCLASS_NAME;
	wc->hIconSm = NULL; // TODO: Small Icon

	RegisterClassEx(wc);

	//
	// Create Window
	//
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;

	int size_x = CW_USEDEFAULT;
	int size_y = CW_USEDEFAULT;

	wnd->hWnd = CreateWindow(WNDCLASS_NAME, WND_TITLE, WS_OVERLAPPEDWINDOW, x, y, size_x, size_y, NULL, NULL, wc->hInstance, NULL);

	if (!wnd->hWnd)
	{
		return 1;
	}

	return 0;
}

void GUI_FreeWindow(wnd_instance_t* wnd)
{
	UnregisterClass(WNDCLASS_NAME, wnd->wc.hInstance);
}

void GUI_EnterMessageLoop(void)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		
		ImGui_ImplDX9_NewFrame();
	
		//
		// Draw the ImGui test window
		//
		bool show_test_window = true;
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);

		//
		// Render
		//
		g_d3d.device->SetRenderState(D3DRS_ZENABLE, false);
		g_d3d.device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_d3d.device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		
		g_d3d.device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3D_CLEARCOLOR, 1.0f, 0);
		
		if (g_d3d.device->BeginScene() >= 0)
		{
			ImGui::Render();
			g_d3d.device->EndScene();
		}

		g_d3d.device->Present(NULL, NULL, NULL, NULL);
	}
}
