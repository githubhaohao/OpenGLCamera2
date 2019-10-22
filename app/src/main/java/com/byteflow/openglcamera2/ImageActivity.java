package com.byteflow.openglcamera2;

import android.os.Bundle;
import android.renderscript.Byte4;
import android.support.annotation.Nullable;
import android.util.Log;
import android.util.Size;
import android.widget.RelativeLayout;
import android.widget.SeekBar;

import com.byteflow.openglcamera2.frame.ByteFlowFrame;
import com.byteflow.openglcamera2.frame.FrameUtil;
import com.byteflow.openglcamera2.gesture.MyGestureListener;

import static com.byteflow.openglcamera2.render.ByteFlowRender.PARAM_TYPE_SET_SHADER_INDEX;

public class ImageActivity extends BaseRenderActivity {
    private static final String TAG = "ImageActivity";
    private RelativeLayout mSurfaceViewRoot;
    private ByteFlowFrame mByteFlowFrame;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image);

        mSurfaceViewRoot = findViewById(R.id.surface_root);

        RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        mSurfaceViewRoot.addView(mGLSurfaceView, p);

        mByteFlowRender.init(mGLSurfaceView);

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
            mByteFlowRender.setRenderFrame(mByteFlowFrame.getData(), mByteFlowFrame.getWidth(), mByteFlowFrame.getHeight());
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
                mCurrentShaderIndex = mCurrentShaderIndex % 13;
                mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                mByteFlowRender.requestRender();
                break;
            case SWIPE_LEFT:
                mCurrentShaderIndex--;
                if (mCurrentShaderIndex < 0) {
                    mCurrentShaderIndex += 13;
                }
                mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                mByteFlowRender.requestRender();
                break;
            default:
                break;
        }
    }
}
