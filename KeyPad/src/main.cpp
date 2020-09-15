#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <Xinput.h>
#include "../resource.h"

#pragma comment(lib, "Xinput.lib")

#define USER_INDEX 0
#define MY_DEAD_ZONE 6500
#define INPUT_START -32767
#define INPUT_END 32767
#define OUTPUT_START -11
#define OUTPUT_END 11
#define WHEEL_START -10
#define WHEEL_END 10
#define DELAY 8

//#define DBG

#ifdef DBG
#define DEBUG(X) printf("[DEBUG] %s", X)
#define WINDOW_SHOWHIDE SW_SHOW
#else
#define DEBUG(X)
#define WINDOW_SHOWHIDE SW_HIDE
#endif


/*
Можно добавить

При первом запуске показывать управление
Сделать переназначение клавиш
GUI
Сворачивание приложеения в трей
Работа со стандартной osk

Баги
Не рабатает во время MessageBox
Не работает стандратная osk

*/

typedef struct _PressedKeys  //struct for detecting when user pressed key and when released
{
	bool key;
	bool leftMouseButton;
	bool rightMouseButton;
	bool osk;
	bool help;
	bool arrowUp;
	bool arrowDown;
	bool arrowLeft;
	bool arrowRight;
} PressedKeys;

bool IsConnected(XINPUT_STATE* s)
{
	ZeroMemory(s, sizeof(XINPUT_STATE));
	DWORD res = XInputGetState(USER_INDEX, s);
	if (res == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

PressedKeys* InitPressedKeys()
{
	PressedKeys* pk = (PressedKeys*)malloc(sizeof(PressedKeys));

	pk->key = false;
	pk->leftMouseButton = false;
	pk->rightMouseButton = false;
	pk->osk = false;
	pk->help = false;
	pk->arrowDown = false;
	pk->arrowUp = false;
	pk->arrowLeft = false;
	pk->arrowRight = false;

	return pk;
}

void sendPressRelease(DWORD Flags)
{
	INPUT input;

	input.type = INPUT_MOUSE;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dwExtraInfo = 0;

	input.mi.dwFlags = Flags;

	SendInput(1, &input, sizeof(INPUT));
}

void sendMove(SHORT thumbX, SHORT thumbY)
{
	INPUT in;
	
	/*Mapping*/
	int outputX, outputY;
	//calculate scale for X
	outputX = ((int)thumbX - INPUT_START)*(OUTPUT_END - OUTPUT_START) / (INPUT_END - INPUT_START) + OUTPUT_START;
	//for Y
	outputY = ((int)thumbY - INPUT_START)*(OUTPUT_END - OUTPUT_START) / (INPUT_END - INPUT_START) + OUTPUT_START;

	in.type = INPUT_MOUSE;
	in.mi.dwFlags = MOUSEEVENTF_MOVE;
	in.mi.dx = outputX;
	in.mi.dy = -outputY;  //inverse Y
	in.mi.dwExtraInfo = 0;
	in.mi.mouseData = 0;
	in.mi.time = 0;

	SendInput(1, &in, sizeof(INPUT));

}

void sendScroll(SHORT thumbY)
{
	INPUT in;

	int outputY;
	/*Mapping*/
	outputY = ((int)thumbY - INPUT_START)*(WHEEL_END - WHEEL_START) / (INPUT_END - INPUT_START) + WHEEL_START;

	in.type = INPUT_MOUSE;
	in.mi.dwFlags = MOUSEEVENTF_WHEEL;
	in.mi.dx = 0;
	in.mi.dy = 0;
	in.mi.dwExtraInfo = 0;
	in.mi.mouseData = outputY;
	in.mi.time = 0;

	SendInput(1, &in, sizeof(INPUT));
}

void sendArrow(WORD key, DWORD flag = NULL)
{
	INPUT in;

	in.type = INPUT_KEYBOARD;
	in.ki.wVk = key;
	in.ki.dwFlags = flag;
	in.ki.time = 0;
	in.ki.wScan = 0;
	in.ki.dwExtraInfo = 0;

	SendInput(1, &in, sizeof(INPUT));
}

int main()
{
	XINPUT_STATE* state = (XINPUT_STATE*)malloc(sizeof(XINPUT_STATE));
	PressedKeys* Keys = InitPressedKeys();
	HWND hwnd = GetConsoleWindow();
	long long counter = 0;

	ShowWindow(hwnd, WINDOW_SHOWHIDE);


	while (true)
	{
		if (IsConnected(state))
		{
			/*-----------------------------DIGITAL BUTTONS--------------------------------*/
			//check digital buttons
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_A) //if A pressed
			{
				//SendInput press left mouse
				if (!Keys->leftMouseButton)
				{
					sendPressRelease(MOUSEEVENTF_LEFTDOWN);
					DEBUG("Left mouse key pressed\n");
				}

				Keys->key = true;
				Keys->leftMouseButton = true;

				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_B) //if B pressed
			{
				//SendInput press right mouse
				if (!Keys->rightMouseButton)
				{
					sendPressRelease(MOUSEEVENTF_RIGHTDOWN);
					DEBUG("Right mouse key pressed\n");
				}
				Keys->key = true;
				Keys->rightMouseButton = true;

				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_X) //if X pressed
			{
				if (!Keys->osk)
				{
					ShellExecute(hwnd, "open", "FreeVK.exe", 0, 0, SW_SHOW);
					DEBUG("OSK Opened\n");
				}

				Keys->key = true;
				Keys->osk = true;

				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_START) //if start pressed
			{
				if (!Keys->help)
				{
					MessageBox(NULL, "Control\n"
						"A - Left mouse button click\n"
						"B - Right mouse button click\n"
						"Back - Exit program\n"
						"Left stick - Mouse moving\n"
						"Right stick - Mouse wheel"
						"Start - Help message\n",
						"How to use KeyPad | Created by Artem Puzik", MB_OK | MB_ICONINFORMATION);
				}

				Keys->key = true;
				Keys->help = true;

				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_BACK) //exit if BACK pressed
			{
				counter = 0;

				break;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)  //if directional pad up is pressed
			{
				if (!Keys->arrowUp)
				{
					sendArrow(VK_UP);
				}
				Keys->arrowUp = true;
				Keys->key = true;
				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) //if directional pad down is pressed
			{
				if (!Keys->arrowDown)
				{
					sendArrow(VK_DOWN);
				}
				Keys->arrowDown = true;
				Keys->key = true;
				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) //if directional pad left is pressed
			{
				if (!Keys->arrowLeft)
				{
					sendArrow(VK_LEFT);
				}
				Keys->arrowLeft = true;
				Keys->key = true;
				counter = 0;
			}
			if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) //if directional pad right is pressed
			{
				if (!Keys->arrowRight)
				{
					sendArrow(VK_RIGHT);
				}
				Keys->arrowRight = true;
				Keys->key = true;
				counter = 0;
			}

			/*------------------------ANALOG TRIGGERS-----------------------------*/
			//check analog triggers
			if ((state->Gamepad.sThumbLX > MY_DEAD_ZONE || state->Gamepad.sThumbLX < -MY_DEAD_ZONE) //if state is not in dead zone
				|| (state->Gamepad.sThumbLY > MY_DEAD_ZONE || state->Gamepad.sThumbLY < -MY_DEAD_ZONE)) //move mouse
			{
				//printf("X = %d\nY = %d\n\n\n", state->Gamepad.sThumbLX, state->Gamepad.sThumbLY);
				sendMove(state->Gamepad.sThumbLX, state->Gamepad.sThumbLY);

				counter = 0;
			}
			if (state->Gamepad.sThumbRY > MY_DEAD_ZONE || state->Gamepad.sThumbRY < -MY_DEAD_ZONE) //scroll
			{
				sendScroll(state->Gamepad.sThumbRY);

				counter = 0;
			}


			if (!Keys->key) //if not pressed any key
			{
				if (Keys->leftMouseButton)
				{
					//SendInput release
					sendPressRelease(MOUSEEVENTF_LEFTUP);
					DEBUG("Left moues key released\n");

					Keys->leftMouseButton = false;
				}
				if (Keys->rightMouseButton)
				{
					//SendInput release
					sendPressRelease(MOUSEEVENTF_RIGHTUP);
					DEBUG("Right mouse key released\n");

					Keys->rightMouseButton = false;
				}
				if (Keys->osk)
				{
					Keys->osk = false;
				}
				if (Keys->help)
				{
					Keys->help = false;
				}
				if (Keys->arrowUp)
				{
					sendArrow(VK_UP, KEYEVENTF_KEYUP);
					Keys->arrowUp = false;
				}
				if (Keys->arrowDown)
				{
					sendArrow(VK_DOWN, KEYEVENTF_KEYUP);
					Keys->arrowDown = false;
				}
				if (Keys->arrowLeft)
				{
					sendArrow(VK_LEFT, KEYEVENTF_KEYUP);
					Keys->arrowLeft = false;
				}
				if (Keys->arrowRight)
				{
					sendArrow(VK_RIGHT, KEYEVENTF_KEYUP);
					Keys->arrowRight = false;
				}
			}
			Keys->key = false; //set to false
		}
		else
		{
			while (!IsConnected(state)) //if not connected, then in waiting mode
			{
				DEBUG("Wait for connection\n");
				Sleep(500);
			}
		}

		if (counter > 5000 / DELAY) //if controller is not used for 5 seconds, then turn waiting mode
		{
			Sleep(250);
		}
		counter++;
		Sleep(DELAY); //decrease CPU load and speed of mouse
	}


exit:
	free(state);
	free(Keys);
	return 0;
}


