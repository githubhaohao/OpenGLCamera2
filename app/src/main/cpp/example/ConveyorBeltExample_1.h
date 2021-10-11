/**
 *
 * Created by 公众号：字节流动 on 2022/8/18.
 * https://github.com/githubhaohao/OpenGLCamera2
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#ifndef NDK_OPENGLES_3_0_CONVEYORBELT_1_H
#define NDK_OPENGLES_3_0_CONVEYORBELT_1_H

#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <vec3.hpp>
#include <vec2.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "GLExampleBase.h"

using namespace glm;

#define BF_LOOP_COUNT 200

class ConveyorBeltExample_1 : public GLExampleBase
{
public:
	ConveyorBeltExample_1();

	virtual ~ConveyorBeltExample_1();

	virtual void LoadImage(NativeImage *pImage);

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

	virtual void Init();
	virtual void Draw(int width, int height);

	virtual void Destroy();

	virtual void SetTransformMatrix(TransformMatrix &transformMatrix);

	void UpdateMVPMatrix(glm::mat4 &mat4Matrix, TransformMatrix &transformMatrix);

private:
	GLuint m_TextureId;
	GLint m_MVPMatLoc;
	GLuint m_VaoId;
	GLuint m_VboIds[3];
	NativeImage m_RenderImage;
	NativeImage m_SrcImage;
    glm::mat4 m_MVPMatrix;

	int m_frameIndex;
	int m_loopCount;

	int m_bannerNum;

};


#endif //NDK_OPENGLES_3_0_CONVEYORBELT_1_H
