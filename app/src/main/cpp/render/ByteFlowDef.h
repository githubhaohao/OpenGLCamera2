/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/OpenGLCamera2
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#ifndef OPENGLCAMERA2_BYTEFLOWDEF_H
#define OPENGLCAMERA2_BYTEFLOWDEF_H

#include <stdint.h>
#include <stdlib.h>
#include <cstring>

#define BF_OK     0
#define BF_ERROR -1

//for YUV420p_I420
typedef struct ByteFlowFrame
{
	size_t width;
	size_t height;
	size_t yPitch;
	size_t uPitch;
	size_t vPitch;
	uint8_t *pYPlane;
	uint8_t *pUPlane;
	uint8_t *pVPlane;

	ByteFlowFrame()
	{
		width = 0;
		height= 0;
		yPitch= 0;
		uPitch= 0;
		vPitch= 0;

		pYPlane = NULL;
		pUPlane = NULL;
		pVPlane = NULL;
	}

};

typedef struct TransformMatrix
{
	int degree;
	int mirror;
	float translateX;
	float translateY;
	float scaleX;
	float scaleY;

	TransformMatrix():
			translateX(0),
			translateY(0),
			scaleX(1.0),
			scaleY(1.0),
			degree(0),
			mirror(0)
	{

	}
	void Reset()
	{
		translateX = 0;
		translateY = 0;
		scaleX = 1.0;
		scaleY = 1.0;
		degree = 0;
		mirror = 0;

	}

};

class ByteFlowFrameUtil
{
public:
	static void AllocFrame(ByteFlowFrame *pFrame)
	{
		if (pFrame != NULL)
		{
			if (pFrame->yPitch == 0 || pFrame->uPitch == 0 || pFrame->vPitch == 0)
			{
				pFrame->yPitch = pFrame->width;
				pFrame->uPitch = pFrame->width / 2;
				pFrame->vPitch = pFrame->width / 2;
			}

			size_t mem_size = static_cast<size_t>(pFrame->yPitch * pFrame->height +
												  pFrame->uPitch * (pFrame->height >> 1) +
												  pFrame->vPitch * (pFrame->height >> 1));
			pFrame->pYPlane = static_cast<uint8_t *>(malloc(mem_size));
			pFrame->pUPlane = pFrame->pYPlane + pFrame->yPitch * pFrame->height;
			pFrame->pVPlane = pFrame->pUPlane + pFrame->uPitch * (pFrame->height >> 1);
		}

	}

	static void FreeFrame(ByteFlowFrame *pFrame)
	{
		if (pFrame != NULL && pFrame->pYPlane != NULL)
		{
			free(pFrame->pYPlane);
			pFrame->pYPlane = NULL;
		}
	}

	static bool CopyFrame(ByteFlowFrame *pSrcFrame, ByteFlowFrame *pDstFrame)
	{
		if (pSrcFrame == NULL || pSrcFrame->pYPlane == NULL || pDstFrame == NULL ||
			pDstFrame->pYPlane == NULL)
		{
			return false;
		}

		size_t ySize = pSrcFrame->yPitch * pSrcFrame->height;
		size_t uSize = pSrcFrame->uPitch * (pSrcFrame->height >> 1);
		size_t vSize = pSrcFrame->vPitch * (pSrcFrame->height >> 1);
		if (pSrcFrame->width == pSrcFrame->yPitch)
		{

			memcpy(pDstFrame->pYPlane, pSrcFrame->pYPlane, ySize);
		}
		else
		{
			uint8_t *pSrcY = pSrcFrame->pYPlane;
			uint8_t *pDstY = pDstFrame->pYPlane;
			for (int i = 0; i < pSrcFrame->height; ++i)
			{
				memcpy(pDstY, pSrcY, pDstFrame->width);
				pSrcY += pSrcFrame->yPitch;
				pDstY += pDstFrame->width;
			}
		}

		if (pSrcFrame->width / 2 == pSrcFrame->uPitch)
		{

			memcpy(pDstFrame->pUPlane, pSrcFrame->pUPlane, uSize);
		}
		else
		{
			uint8_t *pSrcU = pSrcFrame->pUPlane;
			uint8_t *pDstU = pDstFrame->pUPlane;
			for (int i = 0; i < pSrcFrame->height / 2; ++i)
			{
				memcpy(pDstU, pSrcU, pDstFrame->width / 2);
				pSrcU += pSrcFrame->uPitch;
				pDstU += pDstFrame->width / 2;
			}
		}

		if (pSrcFrame->width / 2 == pSrcFrame->vPitch)
		{

			memcpy(pDstFrame->pVPlane, pSrcFrame->pVPlane, vSize);
		}
		else
		{
			uint8_t *pSrcV = pSrcFrame->pVPlane;
			uint8_t *pDstV = pDstFrame->pVPlane;
			for (int i = 0; i < pSrcFrame->height / 2; ++i)
			{
				memcpy(pDstV, pSrcV, pDstFrame->width / 2);
				pSrcV += pSrcFrame->vPitch;
				pDstV += pDstFrame->width / 2;
			}
		}

		return true;
	}

};

#endif //OPENGLCAMERA2_BYTEFLOWDEF_H
