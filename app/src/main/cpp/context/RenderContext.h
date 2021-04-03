/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/OpenGLCamera2
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#ifndef OPENGLCAMERA2_BYTEFLOWRENDERCONTEXT_H
#define OPENGLCAMERA2_BYTEFLOWRENDERCONTEXT_H


#include <cstdint>
#include <jni.h>
#include <ByteFlowRender.h>

#define GL_RENDER_TYPE   0
#define CL_RENDER_TYPE   1

#define PARAM_TYPE_SET_SHADER_INDEX   201

class ByteFlowRenderContext
{
public:
	ByteFlowRenderContext(int renderType);

	~ByteFlowRenderContext();

	static void CreateRenderContext(JNIEnv *env, jobject instance, jint renderType);

	static void StoreRenderContext(JNIEnv *env, jobject instance, ByteFlowRenderContext *pContext);

	static void DeleteRenderContext(JNIEnv *env, jobject instance);

	static ByteFlowRenderContext* GetRenderContext(JNIEnv *env, jobject instance);

	int Init(int initType);

	int UnInit();

	void UpdateFrame(int format, uint8_t *pBuffer, int width, int height);

	void LoadLutImageData(int index, int format, int width, int height, uint8_t *pData);

	void LoadFragShaderScript(int shaderIndex, char *pShaderStr, int strLen);

	void SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror);

	void SetParamsInt(int paramType, int param);

	int GetParamsInt(int paramType);

	void OnSurfaceCreated();

	void OnSurfaceChanged(int width, int height);

	void OnDrawFrame();

private:
	static jfieldID s_ContextHandle;

	ByteFlowRender *m_pByteFlowRender;
};


#endif //OPENGLCAMERA2_BYTEFLOWRENDERCONTEXT_H
