//
// Created by ByteFlow on 2019/4/15.
//

#ifndef OPENGLCAMERA2_GLBYTEFLOWRENDER_H
#define OPENGLCAMERA2_GLBYTEFLOWRENDER_H


#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <GLES3/gl3.h>
#include <vector>
#include "ByteFlowRender.h"

#define SPLIT_SCREEN_SHADER_INDEX     13
#define COLOR_SHIFT_SHADER_INDEX      14
#define ANTI_WIHITE_SHADER_INDEX      15
#define SCALE_CIRCLE_SHADER_INDEX     16
#define ROTATE_CIRCLE_SHADER_INDEX    17
#define DYNAMIC_GLITCH_SHADER_INDEX   18

#define MAX_FRAME_NUM  30
#define SKIP_FRAME_NUM 5


class GLByteFlowRender : public ByteFlowRender
{
public:
	GLByteFlowRender();
	virtual ~GLByteFlowRender();

	virtual int Init(int initType);

	virtual int UnInit();

	virtual void UpdateFrame(uint8_t *pBuffer, int width, int height);

	virtual void SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror);

	virtual void SetShaderIndex(int shaderIndex);

	virtual int GetShaderIndex();

	virtual bool CreateTextures();

	virtual bool UpdateTextures();

	virtual bool DeleteTextures();

	virtual void OnSurfaceCreated();

	virtual void OnSurfaceChanged(int width, int height);

	virtual void OnDrawFrame();

	int CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource);

	void UpdateMVPMatrix(glm::mat4 &mat4Matrix, TransformMatrix &transformMatrix);

	float GetFrameProgress();

	void SetShaderProgramDynamicAttrib(int shaderIndex);

	GLuint UseProgram();

private:
	glm::mat4 m_MVPMatrix;

	GLuint m_YTextureId;
	GLuint m_UTextureId;
	GLuint m_VTextureId;

	GLuint m_Program;
	GLuint m_VertexShader;
	GLuint m_FragShader;

	GLuint m_VertexCoorHandle;
	GLuint m_TextureCoorHandle;
	GLint m_YTextureHandle;
	GLint m_UTextureHandle;
	GLint m_VTextureHandle;
	GLint m_ColorDriftHandle;
	GLint m_ScanLineJitterHandle;

	GLint m_TextureSizeHandle;
	GLint m_OffsetHandle;
	GLint m_MVPHandle;

	std::vector<const char *> m_FragShaders;
	int m_ShaderIndex;
	volatile bool m_IsShaderChanged;

	int m_FrameIndex;

};


#endif //OPENGLCAMERA2_GLBYTEFLOWRENDER_H
