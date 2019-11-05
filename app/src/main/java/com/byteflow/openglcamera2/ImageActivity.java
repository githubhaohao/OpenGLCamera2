package com.byteflow.openglcamera2;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.renderscript.Byte4;
import android.support.annotation.Nullable;
import android.util.Log;
import android.util.Size;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.SeekBar;

import com.byteflow.openglcamera2.frame.ByteFlowFrame;
import com.byteflow.openglcamera2.frame.FrameUtil;
import com.byteflow.openglcamera2.gesture.MyGestureListener;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import static com.byteflow.openglcamera2.render.ByteFlowRender.IMAGE_FORMAT_I420;
import static com.byteflow.openglcamera2.render.ByteFlowRender.IMAGE_FORMAT_RGBA;
import static com.byteflow.openglcamera2.render.ByteFlowRender.PARAM_TYPE_SET_SHADER_INDEX;

public class ImageActivity extends BaseRenderActivity {
    private static final String TAG = "ImageActivity";
    private RelativeLayout mSurfaceViewRoot;
    private ByteFlowFrame mByteFlowFrame;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_image);

        mSurfaceViewRoot = findViewById(R.id.surface_root);

        RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        mSurfaceViewRoot.addView(mGLSurfaceView, p);

        mByteFlowRender.init(mGLSurfaceView);
        mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
        String path = getIntent().getStringExtra("img_path");
        String camera_id = getIntent().getStringExtra("img_ort");
        Log.d(TAG, "onCreate() called with: path = [" + path + "]");
        mByteFlowFrame = FrameUtil.decodeFrame(path);
        updateTransformMatrix(camera_id);

    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mByteFlowFrame != null) {
            mByteFlowRender.setRenderFrame(IMAGE_FORMAT_I420, mByteFlowFrame.getData(), mByteFlowFrame.getWidth(), mByteFlowFrame.getHeight());
            updateGLSurfaceViewSize(new Size(mByteFlowFrame.getWidth(), mByteFlowFrame.getHeight()));
            mByteFlowRender.requestRender();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mByteFlowRender.unInit();
    }

    @Override
    public void onSwipe(MyGestureListener.SwipeDirection direction) {
        Log.d(TAG, "onSwipe() called with: direction = [" + direction + "]");
        switch (direction) {
            case SWIPE_UP:
                break;
            case SWIPE_DOWN:
                break;
            case SWIPE_RIGHT:
                mCurrentShaderIndex++;
                mCurrentShaderIndex = mCurrentShaderIndex % SHADER_NUM;
                switch (mCurrentShaderIndex) {
                    case LUT_A_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_a, 0);
                        break;
                    case LUT_B_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_b, 0);
                        break;
                    case LUT_C_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_c, 0);
                        break;
                    case LUT_D_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_d, 0);
                        break;
                    default:
                }

                if (LUT_A_SHADER_INDEX <= mCurrentShaderIndex && mCurrentShaderIndex <= LUT_D_SHADER_INDEX) {
                    mByteFlowRender.loadShaderFromAssetsFile(LUT_A_SHADER_INDEX, getResources());
                } else {
                    mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
                }

                //mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                mByteFlowRender.requestRender();
                break;
            case SWIPE_LEFT:
                mCurrentShaderIndex--;
                if (mCurrentShaderIndex < 0) {
                    mCurrentShaderIndex += SHADER_NUM;
                }
                switch (mCurrentShaderIndex) {
                    case LUT_A_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_a, 0);
                        break;
                    case LUT_B_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_b, 0);
                        break;
                    case LUT_C_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_c, 0);
                        break;
                    case LUT_D_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_d, 0);
                        break;
                    default:
                }
                if (LUT_A_SHADER_INDEX <= mCurrentShaderIndex && mCurrentShaderIndex <= LUT_D_SHADER_INDEX) {
                    mByteFlowRender.loadShaderFromAssetsFile(LUT_A_SHADER_INDEX, getResources());
                } else {
                    mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
                }
                //mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                mByteFlowRender.requestRender();
                break;
            default:
                break;
        }
    }
}
