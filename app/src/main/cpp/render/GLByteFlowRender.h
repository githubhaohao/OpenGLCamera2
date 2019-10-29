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

typedef struct _tag_ByteFlowCoordinate
{
	int nCoorsCount;
	float *pCoordiantes;

	_tag_ByteFlowCoordinate()
	{
		nCoorsCount = 0;
		pCoordiantes = NULL;
	}

} ByteFlowCoordinate;

typedef struct _tag_ByteFlowMask
{
	size_t width;
	size_t height;
	uint8_t *pMaskPlane;

	_tag_ByteFlowMask()
	{
		width = 0;
		height = 0;
		pMaskPlane = NULL;
	}

} ByteFlowMask;

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

	GLint m_TextureSizeHandle;
	GLint m_MVPHandle;

	std::vector<const char *> m_FragShaders;
	int m_ShaderIndex;
	volatile bool m_IsShaderChanged;

};


#endif //OPENGLCAMERA2_GLBYTEFLOWRENDER_H
