package com.byteflow.openglcamera2;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.byteflow.openglcamera2.camera.Camera2FrameCallback;
import com.byteflow.openglcamera2.camera.Camera2Wrapper;
import com.byteflow.openglcamera2.frame.ByteFlowFrame;
import com.byteflow.openglcamera2.frame.FrameUtil;
import com.byteflow.openglcamera2.gesture.MyGestureListener;
import com.google.android.material.floatingactionbutton.FloatingActionButton;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;
import static com.byteflow.openglcamera2.render.ByteFlowRender.IMAGE_FORMAT_I420;
import static com.byteflow.openglcamera2.render.ByteFlowRender.PARAM_TYPE_SET_EXAMPLE;
import static com.byteflow.openglcamera2.render.ByteFlowRender.EXAMPLE_TYPE;
import static com.byteflow.openglcamera2.render.ByteFlowRender.EXAMPLE_TYPE_KEY_CONVEYOR_BELT;

public class MainActivity extends BaseRenderActivity implements Camera2FrameCallback, MyGestureListener.SimpleGestureListener, View.OnClickListener {
    private static final String TAG = "MainActivity";
    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };
    private static final String[] SAMPLE_TITLES = {
            "抖音传送带",
            "抖音蓝线挑战",
    };
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 1;
    private RelativeLayout mSurfaceViewRoot;
    private Camera2Wrapper mCamera2Wrapper;
    private ImageButton mSwitchCamBtn, mSwitchRatioBtn, mSwitchFilterBtn;
    private int mSampleSelectedIndex = -1;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
//                        .setAction("Action", null).show();
                if (mCamera2Wrapper != null) {
                    mCamera2Wrapper.capture();
                }
            }
        });

        initViews();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            mCamera2Wrapper.startCamera();
        } else {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, CAMERA_PERMISSION_REQUEST_CODE);
        }
        updateTransformMatrix(mCamera2Wrapper.getCameraId());
        if (mSurfaceViewRoot != null) {
            updateGLSurfaceViewSize(mCamera2Wrapper.getPreviewSize());
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                //mCamera2Wrapper.startCamera();
            } else {
                Toast.makeText(this, "We need the camera permission.", Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    @Override
    protected void onPause() {
        if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            mCamera2Wrapper.stopCamera();
        }
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mByteFlowRender.unInit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_change_resolution) {
            showChangeSizeDialog();
        } else if (id == R.id.action_switch_camera) {
            String cameraId = mCamera2Wrapper.getCameraId();
            String[] cameraIds = mCamera2Wrapper.getSupportCameraIds();
            if (cameraIds != null) {
                for (int i = 0; i < cameraIds.length; i++) {
                    if (!cameraIds[i].equals(cameraId)) {
                        mCamera2Wrapper.updateCameraId(cameraIds[i]);
                        updateTransformMatrix(cameraIds[i]);
                        updateGLSurfaceViewSize(mCamera2Wrapper.getPreviewSize());
                        break;
                    }
                }
            }
        }

        return true;
    }

    @Override
    public void onPreviewFrame(byte[] data, int width, int height) {
        Log.d(TAG, "onPreviewFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
        mByteFlowRender.setRenderFrame(IMAGE_FORMAT_I420, data, width, height);
        mByteFlowRender.requestRender();
    }

    @Override
    public void onCaptureFrame(byte[] data, int width, int height) {
        Log.d(TAG, "onCaptureFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
        ByteFlowFrame byteFlowFrame = new ByteFlowFrame(data, width, height);
        String path = FrameUtil.encodeFrame(byteFlowFrame);
        Toast.makeText(this, path, Toast.LENGTH_SHORT).show();
        Intent intent = new Intent(this, ImageActivity.class);
        intent.putExtra("img_path", path);
        intent.putExtra("img_ort", mCamera2Wrapper.getCameraId());
        startActivity(intent);

    }

    private void initViews() {
        mSwitchCamBtn = (ImageButton) findViewById(R.id.switch_camera_btn);
        mSwitchRatioBtn = (ImageButton) findViewById(R.id.switch_ratio_btn);
        mSwitchFilterBtn = (ImageButton) findViewById(R.id.switch_filter_btn);
        mSwitchCamBtn.bringToFront();
        mSwitchRatioBtn.bringToFront();
        mSwitchFilterBtn.bringToFront();
        mSwitchCamBtn.setOnClickListener(this);
        mSwitchRatioBtn.setOnClickListener(this);
        mSwitchFilterBtn.setOnClickListener(this);

        mSurfaceViewRoot = (RelativeLayout) findViewById(R.id.surface_root);
        RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        mSurfaceViewRoot.addView(mGLSurfaceView, p);
        mByteFlowRender.init(mGLSurfaceView);
        mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());

        mCamera2Wrapper = new Camera2Wrapper(this);
        //mCamera2Wrapper.setDefaultPreviewSize(getScreenSize());

        ViewTreeObserver treeObserver = mSurfaceViewRoot.getViewTreeObserver();
        treeObserver.addOnPreDrawListener(new ViewTreeObserver.OnPreDrawListener() {
            @Override
            public boolean  onPreDraw() {
                mSurfaceViewRoot.getViewTreeObserver().removeOnPreDrawListener(this);
                mRootViewSize = new Size(mSurfaceViewRoot.getMeasuredWidth(), mSurfaceViewRoot.getMeasuredHeight());
                updateGLSurfaceViewSize(mCamera2Wrapper.getPreviewSize());
                return true;
            }
        });
    }

    private void showChangeSizeDialog() {
        if (mCamera2Wrapper.getSupportPreviewSize() == null || mCamera2Wrapper.getSupportPreviewSize().size() == 0)
            return;
        if (mCamera2Wrapper.getSupportPictureSize() == null || mCamera2Wrapper.getSupportPictureSize().size() == 0)
            return;

        final ArrayList<String> previewSizeTitles = new ArrayList<>(mCamera2Wrapper.getSupportPreviewSize().size());
        int previewSizeSelectedIndex = 0;
        String selectedSize = mCamera2Wrapper.getPreviewSize().getWidth() + "x" + mCamera2Wrapper.getPreviewSize().getHeight();
        for (Size size : mCamera2Wrapper.getSupportPreviewSize()) {
            String title = size.getWidth() + "x" + size.getHeight();
            previewSizeTitles.add(title);
            if (selectedSize.equals(title)) {
                previewSizeSelectedIndex = previewSizeTitles.size() - 1;
            }
        }

        final ArrayList<String> captureSizeTitles = new ArrayList<>(mCamera2Wrapper.getSupportPreviewSize().size());
        int captureSizeSelectedIndex = 0;
        selectedSize = mCamera2Wrapper.getPictureSize().getWidth() + "x" + mCamera2Wrapper.getPictureSize().getHeight();
        for (Size size : mCamera2Wrapper.getSupportPictureSize()) {
            String title = size.getWidth() + "x" + size.getHeight();
            captureSizeTitles.add(title);
            if (selectedSize.equals(title)) {
                captureSizeSelectedIndex = captureSizeTitles.size() - 1;
            }
        }


        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = LayoutInflater.from(this);
        final View rootView = inflater.inflate(R.layout.resolution_selected_layout, null);

        final AlertDialog dialog = builder.create();

        Button confirmBtn = rootView.findViewById(R.id.confirm_btn);
        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.cancel();
            }
        });

        final RecyclerView resolutionsListView = rootView.findViewById(R.id.resolution_list_view);

        final MyRecyclerViewAdapter myPreviewSizeViewAdapter = new MyRecyclerViewAdapter(this, previewSizeTitles);
        myPreviewSizeViewAdapter.setSelectIndex(previewSizeSelectedIndex);
        myPreviewSizeViewAdapter.addOnItemClickListener(new MyRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                int selectIndex = myPreviewSizeViewAdapter.getSelectIndex();
                myPreviewSizeViewAdapter.setSelectIndex(position);
                myPreviewSizeViewAdapter.notifyItemChanged(selectIndex);
                myPreviewSizeViewAdapter.notifyItemChanged(position);

                String[] strs = previewSizeTitles.get(position).split("x");
                Size updateSize = new Size(Integer.valueOf(strs[0]), Integer.valueOf(strs[1]));
                Log.d(TAG, "onItemClick() called with: strs[0] = [" + strs[0] + "], strs[1] = [" + strs[1] + "]");
                mCamera2Wrapper.updatePreviewSize(updateSize);
                updateGLSurfaceViewSize(mCamera2Wrapper.getPreviewSize());
                dialog.cancel();
            }
        });

        final MyRecyclerViewAdapter myCaptureSizeViewAdapter = new MyRecyclerViewAdapter(this, previewSizeTitles);
        myCaptureSizeViewAdapter.setSelectIndex(captureSizeSelectedIndex);
        myCaptureSizeViewAdapter.addOnItemClickListener(new MyRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                int selectIndex = myCaptureSizeViewAdapter.getSelectIndex();
                myCaptureSizeViewAdapter.setSelectIndex(position);
                myCaptureSizeViewAdapter.notifyItemChanged(selectIndex);
                myCaptureSizeViewAdapter.notifyItemChanged(position);

                String[] strs = captureSizeTitles.get(position).split("x");
                Size updateSize = new Size(Integer.valueOf(strs[0]), Integer.valueOf(strs[1]));
                Log.d(TAG, "onItemClick() called with: strs[0] = [" + strs[0] + "], strs[1] = [" + strs[1] + "]");
                mCamera2Wrapper.updatePictureSize(updateSize);
                updateGLSurfaceViewSize(mCamera2Wrapper.getPreviewSize());
                dialog.cancel();
            }
        });

        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        resolutionsListView.setLayoutManager(manager);

        resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
        resolutionsListView.scrollToPosition(previewSizeSelectedIndex);

        RadioGroup radioGroup = rootView.findViewById(R.id.radio_group);
        radioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @SuppressLint("ResourceAsColor")
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                Log.d(TAG, "onCheckedChanged() called with: group = [" + group + "], checkedId = [" + checkedId + "]");
                if (checkedId == R.id.capture_size_btn) {
                    ((RadioButton) rootView.findViewById(R.id.capture_size_btn)).setTextColor(getColor(R.color.colorAccent));
                    ((RadioButton) rootView.findViewById(R.id.preview_size_btn)).setTextColor(getColor(R.color.colorText));
                    resolutionsListView.setAdapter(myCaptureSizeViewAdapter);
                    resolutionsListView.scrollToPosition(myCaptureSizeViewAdapter.getSelectIndex());

                } else {
                    ((RadioButton) rootView.findViewById(R.id.capture_size_btn)).setTextColor(getColor(R.color.colorText));
                    ((RadioButton) rootView.findViewById(R.id.preview_size_btn)).setTextColor(getColor(R.color.colorAccent));
                    resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
                    resolutionsListView.scrollToPosition(myPreviewSizeViewAdapter.getSelectIndex());
                }
            }
        });

        dialog.show();
        dialog.getWindow().setContentView(rootView);

    }

    private void showGLSampleDialog() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = LayoutInflater.from(this);
        final View rootView = inflater.inflate(R.layout.sample_selected_layout, null);

        final AlertDialog dialog = builder.create();

        Button confirmBtn = rootView.findViewById(R.id.confirm_btn);
        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mByteFlowRender.setParamsInt(PARAM_TYPE_SET_EXAMPLE, EXAMPLE_TYPE + mSampleSelectedIndex);
                dialog.cancel();
            }
        });

        final RecyclerView resolutionsListView = rootView.findViewById(R.id.resolution_list_view);

        final MyRecyclerViewAdapter myPreviewSizeViewAdapter = new MyRecyclerViewAdapter(this, Arrays.asList(SAMPLE_TITLES));
        myPreviewSizeViewAdapter.setSelectIndex(mSampleSelectedIndex);
        myPreviewSizeViewAdapter.addOnItemClickListener(new MyRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {

                int selectIndex = myPreviewSizeViewAdapter.getSelectIndex();
                myPreviewSizeViewAdapter.setSelectIndex(position);
                myPreviewSizeViewAdapter.notifyItemChanged(selectIndex);
                myPreviewSizeViewAdapter.notifyItemChanged(position);
                mSampleSelectedIndex = position;
                mGLSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);
                int sampleType = position + EXAMPLE_TYPE;

                mByteFlowRender.setParamsInt(PARAM_TYPE_SET_EXAMPLE, sampleType);

                switch (sampleType) {
                    case EXAMPLE_TYPE_KEY_CONVEYOR_BELT:
                        break;
                    default:
                        break;
                }

                dialog.cancel();
            }
        });

        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        resolutionsListView.setLayoutManager(manager);

        resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
        resolutionsListView.scrollToPosition(mSampleSelectedIndex);

        dialog.show();
        dialog.getWindow().setContentView(rootView);

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
                    case ASCII_SHADER_INDEX:
                        loadRGBAImage(R.drawable.ascii_mapping, ASCII_SHADER_INDEX);
                        break;
                        default:
                }

                if (LUT_A_SHADER_INDEX <= mCurrentShaderIndex && mCurrentShaderIndex <= LUT_D_SHADER_INDEX) {
                    mByteFlowRender.loadShaderFromAssetsFile(LUT_A_SHADER_INDEX, getResources());
                } else {
                    mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
                }

                //mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
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
                    case ASCII_SHADER_INDEX:
                        loadRGBAImage(R.drawable.ascii_mapping, ASCII_SHADER_INDEX);
                        break;
                    default:
                }

                if (LUT_A_SHADER_INDEX <= mCurrentShaderIndex && mCurrentShaderIndex <= LUT_D_SHADER_INDEX) {
                    mByteFlowRender.loadShaderFromAssetsFile(LUT_A_SHADER_INDEX, getResources());
                } else {
                    mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
                }

                //mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                break;
            default:
                break;
        }

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.switch_camera_btn:
                String cameraId = mCamera2Wrapper.getCameraId();
                String[] cameraIds = mCamera2Wrapper.getSupportCameraIds();
                if (cameraIds != null) {
                    for (int i = 0; i < cameraIds.length; i++) {
                        if (!cameraIds[i].equals(cameraId)) {
                            mCamera2Wrapper.updateCameraId(cameraIds[i]);
                            updateTransformMatrix(cameraIds[i]);
                            updateGLSurfaceViewSize(mCamera2Wrapper.getPreviewSize());
                            break;
                        }
                    }
                }
                break;
            case R.id.switch_ratio_btn:
                showChangeSizeDialog();
                break;
            case R.id.switch_filter_btn:
                showGLSampleDialog();
                break;
                default:
        }
    }

    public static class MyRecyclerViewAdapter extends RecyclerView.Adapter<MyRecyclerViewAdapter.MyViewHolder> implements View.OnClickListener {
        private List<String> mTitles;
        private Context mContext;
        private int mSelectIndex = 0;
        private OnItemClickListener mOnItemClickListener = null;

        public MyRecyclerViewAdapter(Context context, List<String> titles) {
            mContext = context;
            mTitles = titles;
        }

        public void setSelectIndex(int index) {
            mSelectIndex = index;
        }

        public int getSelectIndex() {
            return mSelectIndex;
        }

        public void addOnItemClickListener(OnItemClickListener onItemClickListener) {
            mOnItemClickListener = onItemClickListener;
        }

        @NonNull
        @Override
        public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.resolution_item_layout, parent, false);
            MyViewHolder myViewHolder = new MyViewHolder(view);
            view.setOnClickListener(this);
            return myViewHolder;
        }

        @Override
        public void onBindViewHolder(@NonNull MyViewHolder holder, int position) {
            holder.mTitle.setText(mTitles.get(position));
            if (position == mSelectIndex) {
                holder.mRadioButton.setChecked(true);
                holder.mTitle.setTextColor(mContext.getResources().getColor(R.color.colorAccent));
            } else {
                holder.mRadioButton.setChecked(false);
                holder.mTitle.setText(mTitles.get(position));
                holder.mTitle.setTextColor(Color.GRAY);
            }
            holder.itemView.setTag(position);
        }

        @Override
        public int getItemCount() {
            return mTitles.size();
        }

        @Override
        public void onClick(View v) {
            if (mOnItemClickListener != null) {
                mOnItemClickListener.onItemClick(v, (Integer) v.getTag());
            }
        }

        public interface OnItemClickListener {
            void onItemClick(View view, int position);
        }

        class MyViewHolder extends RecyclerView.ViewHolder {
            RadioButton mRadioButton;
            TextView mTitle;

            public MyViewHolder(View itemView) {
                super(itemView);
                mRadioButton = itemView.findViewById(R.id.radio_btn);
                mTitle = itemView.findViewById(R.id.item_title);
            }
        }
    }
}
