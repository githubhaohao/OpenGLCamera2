#ifndef _BYTE_FLOW_GL_UTILS_H_
#define _BYTE_FLOW_GL_UTILS_H_

#include <GLES3/gl3.h>

//For PI define
#define MATH_PI 3.1415926535897932384626433832802

class GLUtils
{
public:
	static GLuint LoadShader(GLenum shaderType, const char *pSource);

	static GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle,
								GLuint &fragShaderHandle);

	static void DeleteProgram(GLuint &program);

	static void CheckGLError(const char *pGLOperation);
};
#endif // _BYTE_FLOW_GL_UTILS_H_