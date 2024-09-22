#pragma once

/*
* CREDIT TO https://www.unknowncheats.me/forum/minecraft/412743-2d-3d-esp-via-jni-opengl-overlay.html FOR THE OPENGL UTILITIES!
*/

#include <Windows.h>
#include <GL/glew.h>
#include <jni.h>
#include <iostream>

#include "VM.h"
#include "Debug.h"

class ActiveRenderInfo
{
public:
	static inline GLint VIEWPORT[16];
	static inline GLfloat MODELVIEW[16];
	static inline GLfloat PROJECTION[16];

	static inline void Get_VIEWPORT()
	{
		jclass CActiveRenderInfo = vm.renderEnv->FindClass("auz");
		jfieldID FIDVIEWPORT = vm.renderEnv->GetStaticFieldID(CActiveRenderInfo, "a", "Ljava/nio/IntBuffer;");
		jobject OVIEWPORT = vm.renderEnv->GetStaticObjectField(CActiveRenderInfo, FIDVIEWPORT);
		jclass CIntBuffer = vm.renderEnv->FindClass("java/nio/IntBuffer");
		jmethodID MIDget = vm.renderEnv->GetMethodID(CIntBuffer, "get", "(I)I");
		for (int i = 0; i < 16; i++)
		{
			VIEWPORT[i] = vm.renderEnv->CallIntMethod(OVIEWPORT, MIDget, i);
		}
	}

	static inline void Get_MODELVIEW()
	{
		jclass CActiveRenderInfo = vm.renderEnv->FindClass("auz");
		jfieldID FIDMODELVIEW = vm.renderEnv->GetStaticFieldID(CActiveRenderInfo, "b", "Ljava/nio/FloatBuffer;");
		jobject OMODELVIEW = vm.renderEnv->GetStaticObjectField(CActiveRenderInfo, FIDMODELVIEW);
		jclass CFloatBuffer = vm.renderEnv->FindClass("java/nio/FloatBuffer");
		jmethodID MIDget = vm.renderEnv->GetMethodID(CFloatBuffer, "get", "(I)F");
		for (int i = 0; i < 16; i++)
		{
			MODELVIEW[i] = vm.renderEnv->CallFloatMethod(OMODELVIEW, MIDget, i);
		}
	}

	static inline void Get_PROJECTION()
	{
		jclass CActiveRenderInfo = vm.renderEnv->FindClass("auz");
		jfieldID FIDPROJECTION = vm.renderEnv->GetStaticFieldID(CActiveRenderInfo, "c", "Ljava/nio/FloatBuffer;");
		jobject OPROJECTION = vm.renderEnv->GetStaticObjectField(CActiveRenderInfo, FIDPROJECTION);
		jclass CFloatBuffer = vm.renderEnv->FindClass("java/nio/FloatBuffer");
		jmethodID MIDget = vm.renderEnv->GetMethodID(CFloatBuffer, "get", "(I)F");
		for (int i = 0; i < 16; i++)
		{
			PROJECTION[i] = vm.renderEnv->CallFloatMethod(OPROJECTION, MIDget, i);
		}
	}
};