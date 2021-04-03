/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/OpenGLCamera2
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include <GLByteFlowRender.h>
#include <LogUtil.h>
#include "RenderContext.h"

jfieldID ByteFlowRenderContext::s_ContextHandle = 0L;

ByteFlowRenderContext::ByteFlowRenderContext(int renderType) :
		m_pByteFlowRender(NULL)
{
	switch (renderType)
	{
		case GL_RENDER_TYPE:
			m_pByteFlowRender = new GLByteFlowRender();
			break;
		case CL_RENDER_TYPE:
			break;
		default:
			m_pByteFlowRender = new GLByteFlowRender();
	}

}

ByteFlowRenderContext::~ByteFlowRenderContext()
{
	if (m_pByteFlowRender != NULL)
	{
		delete m_pByteFlowRender;
		m_pByteFlowRender = NULL;
	}

}

void ByteFlowRenderContext::CreateRenderContext(JNIEnv *env, jobject instance, jint renderType)
{
	LOGCATE("ByteFlowRenderContext::CreateRenderContext renderType = %d", renderType);
	ByteFlowRenderContext *pContext = new ByteFlowRenderContext(renderType);
	StoreRenderContext(env, instance, pContext);
}

void ByteFlowRenderContext::StoreRenderContext(JNIEnv *env, jobject instance, ByteFlowRenderContext *pContext)
{
	LOGCATE("ByteFlowRenderContext::StoreRenderContext");
	jclass cls = env->GetObjectClass(instance);
	if (cls == NULL)
	{
		LOGCATE("ByteFlowRenderContext::StoreRenderContext cls == NULL");
		return;
	}

	s_ContextHandle = env->GetFieldID(cls, "mNativeContextHandle", "J");
	if (s_ContextHandle == NULL)
	{
		LOGCATE("ByteFlowRenderContext::StoreRenderContext s_ContextHandle == NULL");
		return;
	}

	env->SetLongField(instance, s_ContextHandle, reinterpret_cast<jlong>(pContext));

}


void ByteFlowRenderContext::DeleteRenderContext(JNIEnv *env, jobject instance)
{
	LOGCATE("ByteFlowRenderContext::DeleteRenderContext");
	if (s_ContextHandle == NULL)
	{
		LOGCATE("ByteFlowRenderContext::DeleteRenderContext Could not find render context.");
		return;
	}

	ByteFlowRenderContext *pContext = reinterpret_cast<ByteFlowRenderContext *>(env->GetLongField(
			instance, s_ContextHandle));
	if (pContext)
	{
		delete pContext;
	}
	env->SetLongField(instance, s_ContextHandle, 0L);
}

ByteFlowRenderContext *ByteFlowRenderContext::GetRenderContext(JNIEnv *env, jobject instance)
{
	LOGCATE("ByteFlowRenderContext::GetRenderContext");

	if (s_ContextHandle == NULL)
	{
		LOGCATE("ByteFlowRenderContext::GetRenderContext Could not find render context.");
		return NULL;
	}

	ByteFlowRenderContext *pContext = reinterpret_cast<ByteFlowRenderContext *>(env->GetLongField(
			instance, s_ContextHandle));
	return pContext;
}

int ByteFlowRenderContext::Init(int initType)
{
	return m_pByteFlowRender->Init(initType);
}

int ByteFlowRenderContext::UnInit()
{
	return m_pByteFlowRender->UnInit();
}

void ByteFlowRenderContext::UpdateFrame(int format, uint8_t *pBuffer, int width, int height)
{
	LOGCATE("ByteFlowRenderContext::UpdateFrame format=%d, width=%d, height=%d, pData=%p",
			format, width, height, pBuffer);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pBuffer;

	switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
			break;
		default:
			break;
	}
	m_pByteFlowRender->UpdateFrame(&nativeImage);
}

void ByteFlowRenderContext::SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror)
{
	m_pByteFlowRender->SetTransformMatrix(translateX, translateY, scaleX, scaleY, degree, mirror);
}

void ByteFlowRenderContext::SetParamsInt(int paramType, int param)
{
	LOGCATE("ByteFlowRenderContext::SetParamsInt paramType = %d, param = %d", paramType, param);
	switch (paramType)
	{
		case PARAM_TYPE_SET_SHADER_INDEX:
			m_pByteFlowRender->SetShaderIndex(param);
			break;
		default:
			break;
	}

}

int ByteFlowRenderContext::GetParamsInt(int paramType)
{
	LOGCATE("ByteFlowRenderContext::GetParamsInt paramType = %d", paramType);
	switch (paramType)
	{
		case PARAM_TYPE_SET_SHADER_INDEX:
			return m_pByteFlowRender->GetShaderIndex();
		default:
			break;
	}
	return -1;
}

void ByteFlowRenderContext::OnSurfaceCreated()
{
	m_pByteFlowRender->OnSurfaceCreated();
}

void ByteFlowRenderContext::OnSurfaceChanged(int width, int height)
{
	m_pByteFlowRender->OnSurfaceChanged(width, height);
}

void ByteFlowRenderContext::OnDrawFrame()
{
	m_pByteFlowRender->OnDrawFrame();
}

void ByteFlowRenderContext::LoadLutImageData(int index, int format, int width, int height, uint8_t *pData)
{
	LOGCATE("ByteFlowRenderContext::LoadFilterImageData index=%d, format=%d, width=%d, height=%d, pData=%p",
			index, format, width, height, pData);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pData;

	switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
			break;
		default:
			break;
	}

	m_pByteFlowRender->LoadFilterImageData(index, &nativeImage);
}

void ByteFlowRenderContext::LoadFragShaderScript(int shaderIndex, char *pShaderStr, int strLen)
{
	LOGCATE("ByteFlowRenderContext::LoadFragShaderScript shaderIndex = %d, pShaderStr = %s, strLen = %d", shaderIndex, pShaderStr, strLen);
	m_pByteFlowRender->LoadFragShaderScript(shaderIndex, pShaderStr, strLen);

}



