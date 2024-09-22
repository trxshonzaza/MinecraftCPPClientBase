#include "pch.h"
#include "JPlayer.h"
#include "MinecraftInstance.h"
#include "Debug.h"
#include "VM.h"
#include "PlayerUtils.h"
#include "ActiveRenderInfo.h"
#include "Module.h"
#include "AxisAlignedBB.h"
#include "Vectors.h"
#include "JWorld.h"
#include "TextRenderer.h"
#include "ModList.h"

// Modules

#include "Aimbot.h"
#include "Tracers.h"
#include "ESP.h"
#include "ChestStealer.h"
#include "Strafe.h"
#include "TargetStrafe.h"
#include "AutoSprint.h"
#include "Scaffold.h"
#include "KillAura.h"
#include "NoFall.h"

// End Modules

#include <jni.h>
#include <stdio.h>

#include <cstdio>
#include <iostream>
#include <Windows.h>
#include <MinHook.h>
#include <GL/glew.h>
#include <vector>
#include "ModList.h"

/*
TrxshExternal is licensed under a
Creative Commons Attribution 4.0 International License.

You should have received a copy of the license along with this
work. If not, see <https://creativecommons.org/licenses/by/4.0/>.
*/

/*
* ----------------- !!!!!!!!!!!WARNING!!!!!!!!!!! -----------------
* YOU CANNOT CALL FUNCTIONS USING MAIN VM THREAD ON RENDER THREAD. 
* YOU WILL CAUSE THE GAME TO CRASH!
* 
* IF YOU WANT TO CALL 'onRender()' FUNCTIONS FOR MODULES BUT THEY ARE USING THE MAIN VM THREAD, 
* SAVE AS A VARIABLE IN YOUR MODULE HEADER AND CALL IT ON 'onTick()'!!!!!!!!!!!!!!
* 
* THIS APPLIES FOR ALL CLASSES CALLING RENDER THREAD FUNCTIONS ON MAIN THREAD!!!!!!!!!!!!
*/

bool canRender = false;

MinecraftInstance* MinecraftInstance::instance = nullptr;
MinecraftInstance* mc;

std::vector<std::unique_ptr<Module>> modules = {};

// OPENGL HOOK (CREDIT TO axxo1337 https://github.com/axxo1337/OpenGL-Hk)

static WNDPROC windowProc;

LRESULT CALLBACK h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_KEYDOWN) {
		//use these later
	}
	if (uMsg == WM_KEYUP) {
		std::cout << (int)wParam << std::endl;
		if ((int)wParam == 49)
			if (modules[0]->isEnabled)
				modules[0]->Disable();
			else
				modules[0]->Enable();

		if ((int)wParam == 50)
			if (modules[1]->isEnabled)
				modules[1]->Disable();
			else
				modules[1]->Enable();

		if ((int)wParam == 51)
			if (modules[2]->isEnabled)
				modules[2]->Disable();
			else
				modules[2]->Enable();

		if ((int)wParam == 52)
			if (modules[3]->isEnabled)
				modules[3]->Disable();
			else
				modules[3]->Enable();

		if ((int)wParam == 53)
			if (modules[4]->isEnabled)
				modules[4]->Disable();
			else
				modules[4]->Enable();

		if ((int)wParam == 54)
			if (modules[5]->isEnabled)
				modules[5]->Disable();
			else
				modules[5]->Enable();

		if ((int)wParam == 55)
			if (modules[6]->isEnabled)
				modules[6]->Disable();
			else
				modules[6]->Enable();

		if ((int)wParam == 56)
			if (modules[7]->isEnabled)
				modules[7]->Disable();
			else
				modules[7]->Enable();

		if ((int)wParam == 57)
			if (modules[8]->isEnabled)
				modules[8]->Disable();
			else
				modules[8]->Enable();
	}

	return CallWindowProc(windowProc, hWnd, uMsg, wParam, lParam);
}

void setupWndproc() {
	HWND window = FindWindow(NULL, L"Minecraft 1.8.9");

	if (!window) {
		std::cout << "failed to find window ):";
	}

	windowProc = (WNDPROC)SetWindowLongPtrA(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(h_WndProc));
}

void* swapBuffers;
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers owglSwapBuffers;
bool __stdcall wglSwapBuffers(HDC hDc) {
	//std::cout << "buffer swap called" << std::endl;
	static HGLRC oContext = wglGetCurrentContext();
	static HGLRC newContext = wglCreateContext(hDc);
	wglMakeCurrent(hDc, newContext);

	if (vm.renderEnv == nullptr) {
		jint res = vm.jvm->GetEnv((void**)&vm.renderEnv, JNI_VERSION_1_6);

		Debug::write("Attatching to render thread..");

		vm.jvm->AttachCurrentThread((void**)&vm.renderEnv, nullptr);

		Debug::write("Attatched to render thread. :)");

		TextRenderer::init(30);
	}

	PlayerUtils::getDimensions();

	glViewport(0, 0, PlayerUtils::width, PlayerUtils::height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-1 * PlayerUtils::width / 2.0, 1 * PlayerUtils::width / 2.0, 1 * PlayerUtils::height / 2.0, -1 * PlayerUtils::height / 2.0, PlayerUtils::width / 2.0, -PlayerUtils::width / 2.0);

	ActiveRenderInfo::Get_VIEWPORT();
	ActiveRenderInfo::Get_MODELVIEW();
	ActiveRenderInfo::Get_PROJECTION();
	PlayerUtils::getRenderManagerPositions();

	glPushMatrix();

	// Set up projection and modelview matrices once
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(ActiveRenderInfo::PROJECTION);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(ActiveRenderInfo::MODELVIEW);
	glDisable(GL_DEPTH_TEST);  // Optional based on your requirements

	for (auto& mod : modules) {
		if (mod->isEnabled) {
			glPushMatrix();  // Isolate matrix changes per module

			mod->onRender();

			glPopMatrix();   // Restore matrix state
		}
	}

	glPopMatrix();

	// 2d rendering

	glPushMatrix();

	glViewport(0, 0, PlayerUtils::width, PlayerUtils::height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-1 * PlayerUtils::width / 2.0, 1 * PlayerUtils::width / 2.0, 1 * PlayerUtils::height / 2.0, -1 * PlayerUtils::height / 2.0, PlayerUtils::width / 2.0, -PlayerUtils::width / 2.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	TextRenderer::drawWithShadow(-PlayerUtils::width / 2.0F + 300, -PlayerUtils::height / 2.0F + 30, "A Client Im Working On!", new float[4] {1, 1, 1, 1});
	TextRenderer::drawWithShadow(-PlayerUtils::width / 2.0F + 300, -PlayerUtils::height / 2.0F + 60, "https://github.com/trxshonzaza", new float[4] {1, 1, 1, 1});
	TextRenderer::drawWithShadow(-PlayerUtils::width / 2.0F + 300, -PlayerUtils::height / 2.0F + 90, "Version 1.1", new float[4] {1, 1, 1, 1});

	TextRenderer::drawQueuedEntityData();

	ModList::drawList(modules);

	glPopMatrix();

	wglMakeCurrent(hDc, oContext);
	return owglSwapBuffers(hDc);
}

void hookToSwapBuffers() {
	swapBuffers = (void*)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");

	MH_CreateHook(swapBuffers, (LPVOID*)&wglSwapBuffers, (LPVOID*)&owglSwapBuffers);
	MH_EnableHook(swapBuffers);
}

void unhookSwapBuffers() {
	MH_RemoveHook(swapBuffers);
}

// END 

void WINAPI Run(HMODULE module) {
	Debug::init();

	// init modules
	modules.push_back(std::make_unique<Aimbot>());
	modules.push_back(std::make_unique<KillAura>());
	modules.push_back(std::make_unique<Strafe>());
	modules.push_back(std::make_unique<ESP>());
	modules.push_back(std::make_unique<Tracers>());
	modules.push_back(std::make_unique<TargetStrafe>());
	modules.push_back(std::make_unique<Scaffold>());
	modules.push_back(std::make_unique<NoFall>());
	modules.push_back(std::make_unique<ChestStealer>());

	Debug::write("initalized console!");
	Debug::write("hooking to JVM");

	jsize count = 0;
	if (JNI_GetCreatedJavaVMs(&vm.jvm, 1, &count) != JNI_OK || count == 0) {
		// no JVM found, close dLL
		MessageBoxW(NULL, L"No JVM found. Please make sure Minecraft 1.8.9 is opened.", L"Error!", MB_OK);
		return;
	}

	Debug::write("getting env");

	jint res = vm.jvm->GetEnv((void**)&vm.env, JNI_VERSION_1_6);

	if (res == JNI_EDETACHED) {
		Debug::write("JNI is detatched. attatching current thread.");

		vm.jvm->AttachCurrentThread((void**)&vm.env, nullptr);
	}

	if (vm.env == nullptr) {
		Debug::write("PTR is null!!!!!");
	}

	Debug::write("injected.");

	MinecraftInstance::setInstance(mc); // ONLY CALL ONCE. ONLY CALL AFTER INITALIZATION!!!!!!

	MH_Initialize();
	hookToSwapBuffers();

	setupWndproc();

	while (!GetAsyncKeyState(VK_END)) {
		if (mc->isInWorld()) {
			for (auto& mod : modules) {
				if (mod->isEnabled) {
					mod->onTick();
				}
			}

			Sleep(10);
		}
		else {
			// TODO: force shut down code to prevent game crash if modules are used.
		}
	}

	Debug::write("closing and freeing handles. You can close this window freely.");
	Debug::close();
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	vm.jvm->DetachCurrentThread();
	FreeLibraryAndExitThread(module, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
	    case DLL_PROCESS_ATTACH:
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Run, NULL, NULL, NULL);
    }

    return TRUE;
}
