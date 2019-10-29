package com.byteflow.openglcamera2;

import android.content.pm.PackageManager;
import android.hardware.camera2.CameraCharacteristics;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Size;
import android.view.MotionEvent;
import android.widget.RelativeLayout;

import com.byteflow.openglcamera2.camera.CameraUtil;
import com.byteflow.openglcamera2.gesture.MyGestureListener;
import com.byteflow.openglcamera2.render.GLByteFlowRender;

public abstract class BaseRenderActivity extends AppCompatActivity implements MyGestureListener.SimpleGestureListener {
    private static final String TAG = "BaseRenderActivity";
    protected static final int SHADER_NUM = 14;
    protected GLByteFlowRender mByteFlowRender;
    protected GLSurfaceView mGLSurfaceView;
    protected MyGestureListener mGestureDetector;
    protected int mCurrentShaderIndex = 0;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mGLSurfaceView = new GLSurfaceView(this);
        mGestureDetector = new MyGestureListener(this, this);
        mByteFlowRender = new GLByteFlowRender();

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mGestureDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }

    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    public void updateTransformMatrix(String cameraId) {
        if (Integer.valueOf(cameraId) == CameraCharacteristics.LENS_FACING_FRONT) {
            mByteFlowRender.setTransformMatrix(90, 0);
        } else {
            mByteFlowRender.setTransformMatrix(90, 1);
        }

    }

    public void updateGLSurfaceViewSize(Size previewSize) {
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        Size fitSize = CameraUtil.getFitInScreenSize(previewSize.getWidth(), previewSize.getHeight(),displayMetrics.widthPixels, displayMetrics.heightPixels);

        RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) mGLSurfaceView
                .getLayoutParams();
        params.width = fitSize.getWidth();
        params.height = fitSize.getHeight();
        params.addRule(RelativeLayout.CENTER_IN_PARENT);

        mGLSurfaceView.setLayoutParams(params);
    }
}
