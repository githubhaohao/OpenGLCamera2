/**
 *
 * Created by 公众号：字节流动 on 2022/4/18.
 * https://github.com/githubhaohao/OpenGLCamera2
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include <gtc/matrix_transform.hpp>
#include <ImageDef.h>
#include "ConveyorBeltExample.h"
#include "GLUtils.h"

ConveyorBeltExample::ConveyorBeltExample()
{

	m_MVPMatLoc = GL_NONE;

    for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
        m_TextureIds[i] = GL_NONE;
    }
	m_VaoId = GL_NONE;

    m_frameIndex = 0;
	m_loopCount = 0;
}

ConveyorBeltExample::~ConveyorBeltExample()
{
    NativeImageUtil::FreeNativeImage(&m_RenderImage);

    for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
        NativeImageUtil::FreeNativeImage(&m_TexImages[i]);
    }
}

void ConveyorBeltExample::Init()
{
	if(m_RenderImage.ppPlane[0] == nullptr || m_ProgramObj)
		return;

    LOGCATE("ConveyorBeltExample::Init");

    for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
        glGenTextures(1, &m_TextureIds[i]);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec2 v_texCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * a_position;\n"
            "    v_texCoord = a_texCoord;\n"
            "}";

	char fShaderStr[] =
	        "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D u_texture;\n"
            "uniform float u_offset;\n"
            "uniform vec2 u_texSize;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(u_texture, v_texCoord);\n"
            "}";

	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    GO_CHECK_GL_ERROR();
	if (m_ProgramObj)
	{
		m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
	}
	else
	{
		LOGCATE("ConveyorBeltExample::OnSurfaceCreated create program fail");
	}
    vec2 pTmpCoords[BF_TEXTURE_NUM* 2 * 3];
	//第一块矩形
    pTmpCoords[0] = vec2(0,0);
    pTmpCoords[1] = vec2(0,0.5);
    pTmpCoords[2] = vec2(1,0.5);

    pTmpCoords[3] = vec2(0,0);
    pTmpCoords[4] = vec2(1,0.5);
    pTmpCoords[5] = vec2(1,0);

    m_pTexCoords[0] = vec2(0,0);
    m_pTexCoords[1] = vec2(0,1);
    m_pTexCoords[2] = vec2(1,1);

    m_pTexCoords[3] = vec2(0,0);
    m_pTexCoords[4] = vec2(1,1);
    m_pTexCoords[5] = vec2(1,0);

    float delta = 0.5f / BF_BANNER_NUM;
    //其他小矩形
    for (int i = 1; i <= BF_BANNER_NUM; ++i) {
        vec2 p0 = vec2(0, 0.5 + (i-1)*delta);
        vec2 p1 = vec2(0, 0.5 + i*delta);
        vec2 p2 = vec2(1, 0.5 + i*delta);
        vec2 p3 = vec2(1, 0.5 + (i-1)*delta);

        pTmpCoords[6 * i]     = p0;
        pTmpCoords[6 * i + 1] = p1;
        pTmpCoords[6 * i + 2] = p2;

        pTmpCoords[6 * i + 3] = p0;
        pTmpCoords[6 * i + 4] = p2;
        pTmpCoords[6 * i + 5] = p3;

        m_pTexCoords[6 * i]     = vec2(0,0);
        m_pTexCoords[6 * i + 1] = vec2(0,1);
        m_pTexCoords[6 * i + 2] = vec2(1,1);

        m_pTexCoords[6 * i + 3] = vec2(0,0);
        m_pTexCoords[6 * i + 4] = vec2(1,1);
        m_pTexCoords[6 * i + 5] = vec2(1,0);
    }

    for (int i = 0; i < BF_TEXTURE_NUM * 2 * 3; ++i) {
        m_pVtxCoords[i] = GLUtils::texCoordToVertexCoord(pTmpCoords[i]);
    }

	// Generate VBO Ids and load the VBOs with data
	glGenBuffers(2, m_VboIds);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_pVtxCoords), m_pVtxCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_pTexCoords), m_pTexCoords, GL_STATIC_DRAW);

	// Generate VAO Id
	glGenVertexArrays(1, &m_VaoId);
	glBindVertexArray(m_VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindVertexArray(GL_NONE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unique_lock<mutex> lock(m_Lock);
	m_TexImages[0].width  = m_RenderImage.width;
	m_TexImages[0].height = m_RenderImage.height / 2;
	m_TexImages[0].format = m_RenderImage.format;
	NativeImageUtil::AllocNativeImage(&m_TexImages[0]);
	memcpy(m_TexImages[0].ppPlane[0], m_RenderImage.ppPlane[0], m_TexImages[0].width * m_TexImages[0].height * 4);

	uint8 *pBuf = m_RenderImage.ppPlane[0] + m_TexImages[0].width * m_TexImages[0].height * 4;
    for (int i = 1; i <= BF_BANNER_NUM; ++i) {
        m_TexImages[i].width  = m_RenderImage.width;
        m_TexImages[i].height = m_RenderImage.height / 2 / BF_BANNER_NUM;
        m_TexImages[i].format = m_RenderImage.format;
        NativeImageUtil::AllocNativeImage(&m_TexImages[i]);
        memcpy(m_TexImages[i].ppPlane[0], pBuf, m_TexImages[i].width * m_TexImages[i].height * 4);
        pBuf += m_TexImages[i].width * m_TexImages[i].height * 4;
    }

//    for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
//        glActiveTexture(GL_TEXTURE0 + i);
//        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_TexImages[i].width, m_TexImages[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TexImages[i].ppPlane[0]);
//        glBindTexture(GL_TEXTURE_2D, GL_NONE);
//    }

}

void ConveyorBeltExample::LoadImage(NativeImage *pImage)
{
    LOGCATE("ConveyorBeltExample::LoadImage pImage = %p,[w=%d,h=%d,f=%d]", pImage->ppPlane[0], pImage->width, pImage->height, pImage->format);
    unique_lock<mutex> lock(m_Lock);
    if(pImage && pImage->format == IMAGE_FORMAT_I420) {
        cv::Mat mati420 = cv::Mat(pImage->height * 3 / 2, pImage->width, CV_8UC1, pImage->ppPlane[0]);

        if(m_RenderImage.ppPlane[0] == nullptr) {
            m_RenderImage.width = pImage->width;
            m_RenderImage.height = pImage->height;
            m_RenderImage.format = IMAGE_FORMAT_RGBA;
            NativeImageUtil::AllocNativeImage(&m_RenderImage);
        } else if(m_RenderImage.width != pImage->width || m_RenderImage.height != pImage->height) {
            NativeImageUtil::FreeNativeImage(&m_RenderImage);
            for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
                NativeImageUtil::FreeNativeImage(&m_TexImages[i]);
            }

            m_RenderImage.width = pImage->width;
            m_RenderImage.height = pImage->height;
            m_RenderImage.format = IMAGE_FORMAT_RGBA;
            NativeImageUtil::AllocNativeImage(&m_RenderImage);

            m_TexImages[0].width  = m_RenderImage.width;
            m_TexImages[0].height = m_RenderImage.height / 2;
            m_TexImages[0].format = m_RenderImage.format;
            NativeImageUtil::AllocNativeImage(&m_TexImages[0]);

            for (int i = 1; i <= BF_BANNER_NUM; ++i) {
                m_TexImages[i].width  = m_RenderImage.width;
                m_TexImages[i].height = m_RenderImage.height / 2 / BF_BANNER_NUM;
                m_TexImages[i].format = m_RenderImage.format;
                NativeImageUtil::AllocNativeImage(&m_TexImages[i]);
            }
        }

        cv::Mat matRgba = cv::Mat(m_RenderImage.height, m_RenderImage.width, CV_8UC4, m_RenderImage.ppPlane[0]);

        cv::cvtColor(mati420, matRgba, CV_YUV2RGBA_I420);
        LOGCATE("ConveyorBeltExample::cvtColor");
	}
}

void ConveyorBeltExample::Draw(int width, int height)
{
    glViewport(0, 0, width, height);
    glClearColor(1.0f,1.0f,1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	if(m_ProgramObj == GL_NONE || m_TextureIds[0] == GL_NONE) return;
    LOGCATE("ConveyorBeltExample::Draw[w, h]=[%d, %d]",width, height);

    UpdateMVPMatrix(m_MVPMatrix, m_TransformMatrix);

    unique_lock<mutex> lock(m_Lock);
    m_frameIndex ++;

	// Use the program object
	glUseProgram (m_ProgramObj);
    GO_CHECK_GL_ERROR();
	glBindVertexArray(m_VaoId);
    GO_CHECK_GL_ERROR();
	glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    GO_CHECK_GL_ERROR();

	float offset = (m_frameIndex % BF_LOOP_COUNT) * 1.0f / BF_LOOP_COUNT;
	if(m_frameIndex % BF_LOOP_COUNT == 0)
		m_loopCount ++;

    memcpy(m_TexImages[0].ppPlane[0], m_RenderImage.ppPlane[0], m_TexImages[0].width * m_TexImages[0].height * 4);

    uint8 *pBuf = m_RenderImage.ppPlane[0] + m_TexImages[0].width * m_TexImages[0].height * 4 - m_TexImages[1].width * m_TexImages[1].height * 4;
    for (int i = BF_BANNER_NUM; i >= 2; --i) {
        memcpy(m_TexImages[i].ppPlane[0], m_TexImages[i-1].ppPlane[0], m_TexImages[i].width * m_TexImages[i].height * 4);
    }
    memcpy(m_TexImages[1].ppPlane[0], pBuf, m_TexImages[1].width * m_TexImages[1].height * 4);

    for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_TexImages[i].width, m_TexImages[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TexImages[i].ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    GO_CHECK_GL_ERROR();
	GLUtils::setFloat(m_ProgramObj, "u_offset", offset);

    for (int i = 0; i < BF_TEXTURE_NUM; ++i) {
        GLUtils::setVec2(m_ProgramObj, "u_texSize", m_TexImages[i].width,  m_TexImages[i].height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        GLUtils::setInt(m_ProgramObj, "u_texture", 0);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
    glBindVertexArray(GL_NONE);
}

void ConveyorBeltExample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		glDeleteBuffers(2, m_VboIds);
		glDeleteVertexArrays(1, &m_VaoId);
		glDeleteTextures(BF_TEXTURE_NUM, m_TextureIds);
	}
}

void ConveyorBeltExample::SetTransformMatrix(TransformMatrix &transformMatrix)
{
    LOGCATE("ConveyorBeltExample::SetTransformMatrix translateX = %f, translateY = %f, scaleX = %f, scaleY = %f, degree = %d, mirror = %d",
            transformMatrix.translateX, transformMatrix.translateY, transformMatrix.scaleX, transformMatrix.scaleY, transformMatrix.degree, transformMatrix.mirror);
    m_TransformMatrix.degree = transformMatrix.degree;
    m_TransformMatrix.mirror = transformMatrix.mirror;
}

void ConveyorBeltExample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
	LOGCATE("ConveyorBeltExample::LoadMultiImageWithIndex pImage = %p,[w=%d,h=%d,f=%d]", pImage->ppPlane[0], pImage->width, pImage->height, pImage->format);
}

void ConveyorBeltExample::UpdateMVPMatrix(mat4 &mat4Matrix, TransformMatrix &transformMatrix) {
    LOGCATE("ConveyorBeltExample::UpdateMVPMatrix");
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

    LOGCATE("ConveyorBeltExample::UpdateMVPMatrix: rotate %d,%.2f,%0.5f,%0.5f,%0.5f,%0.5f,", transformMatrix.degree, fRotate,
            transformMatrix.translateX, transformMatrix.translateY,
            fFactorX * transformMatrix.scaleX, fFactorY * transformMatrix.scaleY);

    mat4Matrix =
            Projection * View * Model;// Remember, matrix multiplication is the other way around
}
