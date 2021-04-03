/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/OpenGLCamera2
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include <LogUtil.h>
#include <gtc/matrix_transform.hpp>
#include <unistd.h>
#include "GLByteFlowRender.h"
#include "GLUtils.h"
#include "GLShaders.h"

const float DRIFT_SEQ[9] = {0.0f, 0.03f, 0.032f, 0.035f, 0.03f, 0.032f, 0.031f, 0.029f, 0.025f};
//偏移的x值
const float JITTER_SEQ[9] = {0.0f, 0.03f, 0.01f, 0.02f, 0.05f, 0.055f, 0.03f, 0.02f, 0.025f};
//偏移的y值
const float THRESHHOLD_SEQ[9] = {1.0f, 0.965f, 0.9f, 0.9f, 0.9f, 0.6f, 0.8f, 0.5f, 0.5f};

static const float VERTICES_COORS[] =
        {
                -1.f, 1.f,
                -1.f, -1.f,
                1.f, 1.f,
                1.f, -1.f
        };

static const float TEXTURE_COORS[] =
        {
                0, 0,
                0, 1,
                1, 0,
                1, 1
        };

GLByteFlowRender::GLByteFlowRender() :
        m_Program(0),
        m_VertexShader(0),
        m_FragShader(0),
        m_YTextureId(0),
        m_UTextureId(0),
        m_VTextureId(0),
        m_ExtRgbaTextureId(0),
        m_VertexCoorHandle(0),
        m_TextureCoorHandle(0),
        m_YTextureHandle(0),
        m_UTextureHandle(0),
        m_VTextureHandle(0),
        m_LutTextureHandle(0),
        m_TextureSizeHandle(0),
        m_OffsetHandle(0),
        m_TimeHandle(0),
        m_MVPHandle(0),
        m_MVPMatrix(1.0f),
        m_ShaderIndex(0),
        m_PeriodicFrameIndex(0),
        m_FrameIndex(-1) {
    LOGCATE("GLByteFlowRender::GLByteFlowRender");
    m_IsProgramChanged = false;
    m_IsUpdateExtTexture = false;
    m_pFragShaderBuf = nullptr;
    memset(&m_ExtRgbaImage, 0, sizeof(NativeImage));

}

GLByteFlowRender::~GLByteFlowRender() {
    LOGCATE("GLByteFlowRender::~GLByteFlowRender");
}

int GLByteFlowRender::Init(int initType) {
    LOGCATE("GLByteFlowRender::Init");
    return 0;
}

int GLByteFlowRender::UnInit() {
    LOGCATE("GLByteFlowRender::UnInit");
    NativeImageUtil::FreeNativeImage(&m_RenderFrame);
    NativeImageUtil::FreeNativeImage(&m_ExtRgbaImage);
    //DeleteTextures();
    //GLUtils::DeleteProgram(m_Program);
    //m_FragShaders.clear();

    if (m_pFragShaderBuf) {
        free(m_pFragShaderBuf);
        m_pFragShaderBuf = nullptr;
    }

    return 0;
}

void GLByteFlowRender::UpdateFrame(NativeImage *pImage) {
    LOGCATE("GLByteFlowRender::UpdateFrame");
    if (pImage == nullptr) return;
    if (pImage->width != m_RenderFrame.width || pImage->height != m_RenderFrame.height) {
        if (m_RenderFrame.ppPlane[0] != NULL) {
            NativeImageUtil::FreeNativeImage(&m_RenderFrame);
        }
        memset(&m_RenderFrame, 0, sizeof(NativeImage));
        m_RenderFrame.width = pImage->width;
        m_RenderFrame.height = pImage->height;
        m_RenderFrame.format = pImage->format;
        NativeImageUtil::AllocNativeImage(&m_RenderFrame);
    }

    NativeImageUtil::CopyNativeImage(pImage, &m_RenderFrame);
}

void
GLByteFlowRender::SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY,
                                     int degree, int mirror) {
    LOGCATE("GLByteFlowRender::SetTransformMatrix translateX = %f, translateY = %f, scaleX = %f, scaleY = %f, degree = %d, mirror = %d",
            translateX, translateY, scaleX, scaleY, degree, mirror);

    m_TransformMatrix.degree = degree;
    m_TransformMatrix.mirror = mirror;
    m_IsProgramChanged = true;

}

void GLByteFlowRender::SetShaderIndex(int shaderIndex) {
    LOGCATE("GLByteFlowRender::SetShaderIndex shaderIndex = %d", shaderIndex);
    m_ShaderIndex = shaderIndex;
    m_IsShaderChanged = true;

}

int GLByteFlowRender::GetShaderIndex() {
    LOGCATE("GLByteFlowRender::GetShaderIndex");
    return m_ShaderIndex;
}

bool GLByteFlowRender::CreateTextures() {
    LOGCATE("GLByteFlowRender::CreateTextures");
    GLsizei yWidth = static_cast<GLsizei>(m_RenderFrame.width);
    GLsizei yHeight = static_cast<GLsizei>(m_RenderFrame.height);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_YTextureId);
    glBindTexture(GL_TEXTURE_2D, m_YTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yWidth, yHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 NULL);

    if (!m_YTextureId) {
        GLUtils::CheckGLError("GLByteFlowRender::CreateTextures Create Y texture");
        return false;
    }

    GLsizei uWidth = static_cast<GLsizei>(m_RenderFrame.width / 2);
    GLsizei uHeight = yHeight / 2;

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &m_UTextureId);
    glBindTexture(GL_TEXTURE_2D, m_UTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, uWidth, uHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 NULL);

    if (!m_UTextureId) {
        GLUtils::CheckGLError("GLByteFlowRender::CreateTextures Create U texture");
        return false;
    }

    GLsizei vWidth = static_cast<GLsizei>(m_RenderFrame.width / 2);
    GLsizei vHeight = (GLsizei) yHeight / 2;

    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &m_VTextureId);
    glBindTexture(GL_TEXTURE_2D, m_VTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, vWidth, vHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 NULL);

    if (!m_VTextureId) {
        GLUtils::CheckGLError("GLByteFlowRender::CreateTextures Create V texture");
        return false;
    }

    return true;
}

bool GLByteFlowRender::UpdateTextures() {
    LOGCATE("GLByteFlowRender::UpdateTextures");
    if (m_RenderFrame.ppPlane[0] == NULL) {
        return false;
    }

    if (!m_YTextureId && !m_UTextureId && !m_VTextureId && !CreateTextures()) {
        return false;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_YTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_RenderFrame.width,
                 (GLsizei) m_RenderFrame.height, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderFrame.ppPlane[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_UTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_RenderFrame.width >> 1,
                 (GLsizei) m_RenderFrame.height >> 1, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderFrame.ppPlane[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_VTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei) m_RenderFrame.width >> 1,
                 (GLsizei) m_RenderFrame.height >> 1, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderFrame.ppPlane[2]);

    ByteFlowLock lock(&m_SynLock);
    if (m_IsUpdateExtTexture && m_ExtRgbaImage.ppPlane[0]) {
        if (m_ExtRgbaTextureId) {
            glDeleteTextures(1, &m_ExtRgbaTextureId);
        }
        glActiveTexture(GL_TEXTURE3);
        glGenTextures(1, &m_ExtRgbaTextureId);
        glBindTexture(GL_TEXTURE_2D, m_ExtRgbaTextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_ExtRgbaImage.width, m_ExtRgbaImage.height, 0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE, m_ExtRgbaImage.ppPlane[0]);
        m_IsUpdateExtTexture = false;
    }

    return true;
}

bool GLByteFlowRender::DeleteTextures() {
    LOGCATE("GLByteFlowRender::DeleteTextures");
    if (m_YTextureId) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_YTextureId);
        m_YTextureId = 0;
    }

    if (m_UTextureId) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_UTextureId);
        m_UTextureId = 0;
    }

    if (m_VTextureId) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_VTextureId);
        m_VTextureId = 0;
    }

    if (m_ExtRgbaTextureId) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &m_ExtRgbaTextureId);
        m_ExtRgbaTextureId = 0;
    }

    return true;
}

void GLByteFlowRender::OnSurfaceCreated() {
    LOGCATE("GLByteFlowRender::OnSurfaceCreated");
    int nMaxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &nMaxTextureSize);
    LOGCATE("GLByteFlowRender::OnSurfaceCreated GL_MAX_TEXTURE_SIZE = %d", nMaxTextureSize);
    ByteFlowLock lock(&m_ShaderBufLock);
    if (m_pFragShaderBuf != nullptr) {
        if (!CreateProgram(kVertexShader, m_pFragShaderBuf)) {
            LOGCATE("GLByteFlowRender::OnSurfaceCreated create program fail.");
        }
    } else {
        if (!CreateProgram(kVertexShader, kFragmentShader0)) {
            LOGCATE("GLByteFlowRender::OnSurfaceCreated create program fail.");
        }
    }

}

void GLByteFlowRender::OnSurfaceChanged(int width, int height) {
    LOGCATE("GLByteFlowRender::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    m_IsProgramChanged = true;
}

void GLByteFlowRender::OnDrawFrame() {
    LOGCATE("GLByteFlowRender::OnDrawFrame");
    glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_CULL_FACE);

    if (!UpdateTextures() || !UseProgram()) {
        LOGCATE("GLByteFlowRender::OnDrawFrame skip frame");
        return;
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_PeriodicFrameIndex++;
    m_FrameIndex++;
    //glDrawArrays(GL_TRIANGLES, 0, m_FragCoors.nCoorsCount);
}

void GLByteFlowRender::UpdateMVPMatrix(glm::mat4 &mat4Matrix, TransformMatrix &transformMatrix) {
    LOGCATE("GLByteFlowRender::UpdateMVPMatrix");
    float fFactorX = 1.0f;
    float fFactorY = 1.0f;

    if (transformMatrix.mirror == 1) {
        fFactorX = -1.0f;
    } else if (transformMatrix.mirror == 2) {
        fFactorY = -1.0f;
    }

    float fRotate = MATH_PI * transformMatrix.degree * 1.0f / 180;
    if (transformMatrix.mirror == 0) {
        if (transformMatrix.degree == 270) {
            fRotate = MATH_PI * 0.5;
        } else if (transformMatrix.degree == 180) {
            fRotate = MATH_PI;
        } else if (transformMatrix.degree == 90) {
            fRotate = MATH_PI * 1.5;
        }
    } else if (transformMatrix.mirror == 1) {
        if (transformMatrix.degree == 90) {
            fRotate = MATH_PI * 0.5;
        } else if (transformMatrix.degree == 180) {
            fRotate = MATH_PI;
        } else if (transformMatrix.degree == 270) {
            fRotate = MATH_PI * 1.5;
        }
    }

    float ratio = (float) m_RenderFrame.width / m_RenderFrame.height;
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,4.0f/3.0f,0.1f,100.f);
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 1), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(fFactorX * transformMatrix.scaleX,
                                        fFactorY * transformMatrix.scaleY, 1.0f));
    Model = glm::rotate(Model, fRotate, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::translate(Model,
                           glm::vec3(transformMatrix.translateX, transformMatrix.translateY, 0.0f));

    LOGCATE("SetMVP real: rotate %d,%.2f,%0.5f,%0.5f,%0.5f,%0.5f,", transformMatrix.degree, fRotate,
            transformMatrix.translateX, transformMatrix.translateY,
            fFactorX * transformMatrix.scaleX, fFactorY * transformMatrix.scaleY);

    mat4Matrix =
            Projection * View * Model;// Remember, matrix multiplication is the other way around

}

GLuint GLByteFlowRender::UseProgram() {
    LOGCATE("GLByteFlowRender::UseProgram");
    ByteFlowLock lock(&m_ShaderBufLock);
    if (m_IsShaderChanged) {
        GLUtils::DeleteProgram(m_Program);
        CreateProgram(kVertexShader, m_pFragShaderBuf);
        m_IsShaderChanged = false;
        m_IsProgramChanged = true;
    }

    if (!m_Program) {
        LOGCATE("GLByteFlowRender::UseProgram Could not use program.");
        return 0;
    }

    if (m_IsProgramChanged) {
        glUseProgram(m_Program);
        GLUtils::CheckGLError("GLByteFlowRender::UseProgram");

        glVertexAttribPointer(m_VertexCoorHandle, 2, GL_FLOAT, GL_FALSE, 2 * 4, VERTICES_COORS);
        glEnableVertexAttribArray(m_VertexCoorHandle);

        glUniform1i(m_YTextureHandle, 0);
        glUniform1i(m_UTextureHandle, 1);
        glUniform1i(m_VTextureHandle, 2);

        if (m_LutTextureHandle >= 0 && m_ExtRgbaTextureId) {
            glUniform1i(m_LutTextureHandle, 3);
        }

        glVertexAttribPointer(m_TextureCoorHandle, 2, GL_FLOAT, GL_FALSE, 2 * 4, TEXTURE_COORS);
        glEnableVertexAttribArray(m_TextureCoorHandle);

        m_IsProgramChanged = false;
    }

    SetShaderProgramDynamicAttrib(m_ShaderIndex);
    return m_Program;
}

int
GLByteFlowRender::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource) {
    m_Program = GLUtils::CreateProgram(pVertexShaderSource, pFragShaderSource, m_VertexShader,
                                       m_FragShader);
    if (!m_Program) {
        GLUtils::CheckGLError("Create Program");
        LOGCATE("GLByteFlowRender::CreateProgram Could not create program.");
        return 0;
    }

    m_YTextureHandle = glGetUniformLocation(m_Program, "s_textureY");
    m_UTextureHandle = glGetUniformLocation(m_Program, "s_textureU");
    m_VTextureHandle = glGetUniformLocation(m_Program, "s_textureV");

    m_LutTextureHandle = glGetUniformLocation(m_Program, "s_LutTexture");

    m_TextureSizeHandle = glGetUniformLocation(m_Program, "texSize");
    m_OffsetHandle = glGetUniformLocation(m_Program, "u_offset");
    m_TimeHandle = glGetUniformLocation(m_Program, "u_time");

    m_VertexCoorHandle = (GLuint) glGetAttribLocation(m_Program, "position");
    m_TextureCoorHandle = (GLuint) glGetAttribLocation(m_Program, "texcoord");
    m_MVPHandle = glGetUniformLocation(m_Program, "MVP");

    m_ColorDriftHandle = glGetUniformLocation(m_Program, "u_ColorDrift");
    m_ScanLineJitterHandle = glGetUniformLocation(m_Program, "u_ScanLineJitter");

    return m_Program;
}

float GLByteFlowRender::GetFrameProgress() {
    float progress = m_PeriodicFrameIndex * 1.0f / MAX_FRAME_NUM;
    if (progress > 1) {
        progress = 0;
    }

    if (m_PeriodicFrameIndex > MAX_FRAME_NUM + SKIP_FRAME_NUM) {
        m_PeriodicFrameIndex = 0;
    }
    return progress;
}

void GLByteFlowRender::SetShaderProgramDynamicAttrib(int shaderIndex) {
    float progress = GetFrameProgress();

    LOGCATE("GLByteFlowRender::SetShaderProgramDynamicAttrib progress=%f, m_ShaderIndex=%d",
            progress, shaderIndex);
    TransformMatrix transformMatrix = m_TransformMatrix;
    switch (shaderIndex) {
        case COLOR_SHIFT_SHADER_INDEX:
            if (m_OffsetHandle >= 0) {
                glUniform1f(m_OffsetHandle, 0.01f * progress);
            }
            transformMatrix.scaleX = transformMatrix.scaleY = 1.0f + 0.2f * progress;
            LOGCATE("GLByteFlowRender::SetShaderProgramDynamicAttrib transformMatrix.scaleX=%f",
                    transformMatrix.scaleX);
            break;
        case ANTI_WIHITE_SHADER_INDEX:
        case SCALE_CIRCLE_SHADER_INDEX:
            if (m_OffsetHandle >= 0) {
                float offset = progress < 0.5 ? progress : (1.0f - progress);
                glUniform1f(m_OffsetHandle, offset);
            }
            break;
        case GHOST_SHADER_INDEX:
            if (m_OffsetHandle >= 0) {
                glUniform1f(m_OffsetHandle, progress);
            }
            break;
        case ROTATE_CIRCLE_SHADER_INDEX:
            if (m_OffsetHandle >= 0) {
                float offset = static_cast<float>(progress * 2 * MATH_PI);
                glUniform1f(m_OffsetHandle, offset);
            }
            break;
        case DYNAMIC_GLITCH_SHADER_INDEX: {
            int seqIndex = m_PeriodicFrameIndex % 9;
            if (m_ScanLineJitterHandle >= 0) {
                glUniform2f(m_ScanLineJitterHandle, JITTER_SEQ[seqIndex], THRESHHOLD_SEQ[seqIndex]);
            }
            if (m_ColorDriftHandle >= 0) {
                glUniform1f(m_ColorDriftHandle, DRIFT_SEQ[seqIndex]);
            }
        }
            break;
        case DOUYIN_SHADER_INDEX: {
            if (m_OffsetHandle >= 0) {

                float offset = (sin(m_FrameIndex * MATH_PI / 40) + 1.0f) / 2.0f;
                glUniform1f(m_OffsetHandle, offset);
            }
        }
            break;
        case ASCII_SHADER_INDEX: {
            if (m_OffsetHandle >= 0) {
                float offset = (sin(m_FrameIndex * MATH_PI / 80) + 1.0f) / 2.0f;
                glUniform1f(m_OffsetHandle, offset);
            }
            GLUtils::setInt(m_Program, "s_textureMapping", 3);
            GLUtils::setVec2(m_Program, "asciiTexSize", m_ExtRgbaImage.width, m_ExtRgbaImage.height);
        }
        break;
        case ASCII_SHADER_MATTE_BLUR: {
            GLUtils::setFloat(m_Program, "blurSamplerScale", 4.0);
            GLUtils::setFloat(m_Program, "factor", 0.1);
            GLUtils::setInt(m_Program, "samplerRadius", 2);
        }
            break;
        case ASCII_SHADER_POLAR_MOSAIC: {
            GLUtils::setVec2(m_Program, "center", 0.5f, 0.5f);
            GLUtils::setVec2(m_Program, "pixelSize", 0.05f, 0.05f);
        }
            break;
        default:
            break;
    }
    UpdateMVPMatrix(m_MVPMatrix, transformMatrix);
    glUniformMatrix4fv(m_MVPHandle, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    if (m_TextureSizeHandle >= 0) {
        GLfloat size[2];
        size[0] = m_RenderFrame.width;
        size[1] = m_RenderFrame.height;
        glUniform2fv(m_TextureSizeHandle, 1, &size[0]);
    }

    if (m_TimeHandle >= 0) {
        glUniform1f(m_TimeHandle, m_FrameIndex / 10);
    }

}

void GLByteFlowRender::LoadFilterImageData(int index, NativeImage *pImage) {
    LOGCATE("GLByteFlowRender::LoadFilterImageData pImage = %p, index=%d", pImage->ppPlane[0],
            index);
    ByteFlowLock lock(&m_SynLock);
    NativeImageUtil::FreeNativeImage(&m_ExtRgbaImage);
    m_ExtRgbaImage.width = pImage->width;
    m_ExtRgbaImage.height = pImage->height;
    m_ExtRgbaImage.format = pImage->format;
    NativeImageUtil::CopyNativeImage(pImage, &m_ExtRgbaImage);
    m_IsUpdateExtTexture = true;
}

void GLByteFlowRender::LoadFragShaderScript(int shaderIndex, char *pShaderStr, int strLen) {
    LOGCATE("GLByteFlowRender::LoadFragShaderScript pShaderStr = %p, shaderIndex=%d", pShaderStr,
            shaderIndex);
    if (m_ShaderIndex != shaderIndex) {
        ByteFlowLock lock(&m_ShaderBufLock);
        if (m_pFragShaderBuf) {
            free(m_pFragShaderBuf);
            m_pFragShaderBuf = nullptr;
        }
        m_ShaderIndex = shaderIndex;
        m_pFragShaderBuf = static_cast<char *>(malloc(strLen));
        memcpy(m_pFragShaderBuf, pShaderStr, strLen);
        m_IsShaderChanged = true;
    }
}
