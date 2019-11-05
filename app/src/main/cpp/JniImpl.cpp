//
// Created by ByteFlow on 2019/4/12.
//
#include <jni.h>
#include <string>
#include <RenderContext.h>
#include "util/LogUtil.h"

#define CLASS_NAME_BYTEFLOW_RENDER "com/byteflow/openglcamera2/render/ByteFlowRender"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL native_CreateContext(JNIEnv *env, jobject instance, jint renderType)
{
	ByteFlowRenderContext::CreateRenderContext(env, instance, renderType);
}

JNIEXPORT void JNICALL native_DestroyContext(JNIEnv *env, jobject instance)
{
	ByteFlowRenderContext::DeleteRenderContext(env, instance);
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_Init
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL native_Init(JNIEnv *env, jobject instance, jint iniType)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if(pContext) return pContext->Init(iniType);
	return BF_ERROR;
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_UnInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL native_UnInit(JNIEnv *env, jobject instance)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if(pContext) return pContext->UnInit();
	return BF_OK;
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_UpdateFrame
 * Signature: (I[BII)V
 */
JNIEXPORT void JNICALL native_UpdateFrame(JNIEnv *env, jobject instance, jint format, jbyteArray bytes, jint width, jint height)
{
	int len = env->GetArrayLength (bytes);
	unsigned char* buf = new unsigned char[len];
	env->GetByteArrayRegion(bytes, 0, len, reinterpret_cast<jbyte*>(buf));

	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if(pContext) pContext->UpdateFrame(format, buf, width, height);

	delete[] buf;
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_LoadFilterData
 * Signature: (IIII[B)V
 */
JNIEXPORT void JNICALL native_LoadFilterData
		(JNIEnv *env, jobject instance, jint index, jint format, jint width, jint height, jbyteArray imageData)
{
	int len = env->GetArrayLength (imageData);
	uint8_t* buf = new uint8_t[len];
	env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if(pContext) pContext->LoadLutImageData(index, format, width, height, buf);
	delete[] buf;
	env->DeleteLocalRef(imageData);
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_LoadShaderScript
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL native_LoadShaderScript
		(JNIEnv *env, jobject instance, jint shaderIndex, jstring script)
{
	int length = env->GetStringUTFLength(script);
	const char* cStr = env->GetStringUTFChars(script, JNI_FALSE);
	char *buf = static_cast<char *>(malloc(length + 1));
	memcpy(buf, cStr, length + 1);
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if(pContext) pContext->LoadFragShaderScript(shaderIndex, buf, length + 1);
	free(buf);
	env->ReleaseStringUTFChars(script, cStr);
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_SetTransformMatrix
 * Signature: (FFFFII)V
 */
JNIEXPORT void JNICALL native_SetTransformMatrix
(JNIEnv *env, jobject instance, jfloat translateX, jfloat translateY, jfloat scaleX, jfloat scaleY, jint degree, jint mirror)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if(pContext) pContext->SetTransformMatrix(translateX, translateY, scaleX, scaleY, degree, mirror);
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_SetParamsInt
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_SetParamsInt(JNIEnv *env, jobject instance, jint paramType, jint param)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if (pContext) pContext->SetParamsInt(paramType, param);
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_GetParamsInt
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL native_GetParamsInt(JNIEnv *env, jobject instance, jint paramType)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if (pContext) return pContext->GetParamsInt(paramType);
	return BF_ERROR;
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if (pContext) pContext->OnSurfaceCreated();
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance, jint width, jint height)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if (pContext) pContext->OnSurfaceChanged(width, height);
}

/*
 * Class:     com_byteflow_openglcamera2_render_ByteFlowRender
 * Method:    native_OnDrawFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance)
{
	ByteFlowRenderContext *pContext = ByteFlowRenderContext::GetRenderContext(env, instance);
	if (pContext) pContext->OnDrawFrame();
}

#ifdef __cplusplus
}
#endif

static JNINativeMethod g_RenderMethods[] = {
		{"native_CreateContext",      "(I)V",                   (void *)(native_CreateContext)},
		{"native_DestroyContext",     "()V",                    (void *)(native_DestroyContext)},
		{"native_Init",               "(I)I",                   (void *)(native_Init)},
		{"native_UnInit",             "()I",                    (void *)(native_UnInit)},
		{"native_UpdateFrame",        "(I[BII)V",               (void *)(native_UpdateFrame)},
		{"native_LoadFilterData",     "(IIII[B)V",              (void *)(native_LoadFilterData)},
		{"native_LoadShaderScript",   "(ILjava/lang/String;)V", (void *)(native_LoadShaderScript)},
		{"native_SetTransformMatrix", "(FFFFII)V",              (void *)(native_SetTransformMatrix)},
		{"native_SetParamsInt",       "(II)V",                  (void *)(native_SetParamsInt)},
		{"native_GetParamsInt",       "(I)I",                   (void *)(native_GetParamsInt)},
		{"native_OnSurfaceCreated",   "()V",                    (void *)(native_OnSurfaceCreated)},
		{"native_OnSurfaceChanged",   "(II)V",                  (void *)(native_OnSurfaceChanged)},
		{"native_OnDrawFrame",        "()V",                    (void *)(native_OnDrawFrame)},
};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
	LOGCATE("RegisterNativeMethods");
	jclass clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, methods, methodNum) < 0)
	{
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
	LOGCATE("RegisterNativeMethods");
	jclass clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		return;
	}
	if (env != NULL)
	{
		env->UnregisterNatives(clazz);
	}
}

extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
{
	jint jniRet = JNI_ERR;
	JNIEnv *env = NULL;
	if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
	{
		return jniRet;
	}

	jint regRet = RegisterNativeMethods(env, CLASS_NAME_BYTEFLOW_RENDER, g_RenderMethods,
										sizeof(g_RenderMethods) /
										sizeof(g_RenderMethods[0]));
	if (regRet != JNI_TRUE)
	{
		return JNI_ERR;
	}

	return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
	JNIEnv *env = NULL;
	if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
	{
		return;
	}

	UnregisterNativeMethods(env, CLASS_NAME_BYTEFLOW_RENDER);
}
