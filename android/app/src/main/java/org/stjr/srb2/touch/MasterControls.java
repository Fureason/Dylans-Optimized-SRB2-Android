package org.stjr.srb2.touch;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;


import org.stjr.srb2.layout.MenuNavigation;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class MasterControls extends View {
    private final Paint paint, pressedPaint, textPaint;
    private Layout currentLayout;  
    private String currentLayoutName;
    private final Map<Integer, Boolean> currentPressedIDs = new HashMap<>();
    private final Set<Integer> suppressedPointerIDs = new HashSet<>();

    private int cameraId = -1;
    private float cameraX = 0;
    private float cameraY = 0;

    public MasterControls(Context context, String layoutName) {
        super(context);

        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(Color.RED);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5);

        pressedPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        pressedPaint.setColor(Color.RED);
        pressedPaint.setStyle(Paint.Style.FILL_AND_STROKE);
        pressedPaint.setAlpha(128);

        textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        textPaint.setColor(Color.WHITE);
        textPaint.setTextSize(30);
        textPaint.setTextAlign(Paint.Align.CENTER);


        setLayoutByName(layoutName);
    }


    public void setLayoutByName(String layoutName) {
        this.currentLayoutName = layoutName;
        post(() -> {
            try {

                String className = "org.stjr.srb2.layout." + layoutName;
                Class<? extends Layout> layoutClass = Class.forName(className).asSubclass(Layout.class);


                Layout newLayout = layoutClass.getDeclaredConstructor().newInstance();
                setLayout(newLayout);
                setVisibility(VISIBLE);
            } catch (Exception e) {
                android.util.Log.e("MasterControls", "Could not find or create layout: " + layoutName, e);
            }
        });
    }


    public void setControlsVisible(final boolean visible) {
        post(() -> setVisibility(visible ? VISIBLE : GONE));
    }


    public boolean getControlsVisible() {
        return getVisibility() == VISIBLE;
    }


    public String getLayoutName() {
        return currentLayoutName;
    }


    public void setLayout(Layout layout) {
        if (currentLayout != null) {
            suppressedPointerIDs.addAll(currentPressedIDs.keySet());
            currentLayout.releaseAll();  
        }
        if (cameraId != -1) {
            org.stjr.srb2.SRB2Game.nativeOnTouchUp(cameraId, cameraX, cameraY);
            suppressedPointerIDs.add(cameraId);
        }
        this.currentLayout = layout;
        currentPressedIDs.clear();
        cameraId = -1;
        invalidate();
    }


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (currentLayout == null) return false;

        int action = event.getActionMasked();
        int width = getWidth();
        int height = getHeight();

        if (action == MotionEvent.ACTION_CANCEL) {
            currentLayout.releaseAll();
            currentPressedIDs.clear();
            suppressedPointerIDs.clear();
            cameraId = -1;
        } else if (action == MotionEvent.ACTION_MOVE) {
            for (int i = 0; i < event.getPointerCount(); i++) {
                float x = event.getX(i);
                float y = event.getY(i);
                int id = event.getPointerId(i);

                if (suppressedPointerIDs.contains(id))
                    continue;

                if (id == cameraId) {
                    float offsetX = x - cameraX;
                    float offsetY = y - cameraY;

                    org.libsdl.app.SDLActivity.onNativeMouse(0, MotionEvent.ACTION_MOVE, offsetX, offsetY, true);

                    cameraX = x;
                    cameraY = y;
                } else {
                    currentLayout.handleTouch(action, x, y, id, width, height);
                }
            }
        } else {
            int idx = event.getActionIndex();
            int id = event.getPointerId(idx);
            float x = event.getX(idx);
            float y = event.getY(idx);

            if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP) {
                if (suppressedPointerIDs.remove(id)) {
                    invalidate();
                    return true;
                }
            }

            if ((action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) && !org.stjr.srb2.SRB2Game.nativeOnTouchDown(id, x, y)) {
                suppressedPointerIDs.remove(id);
                if (currentLayout.handleTouch(action, x, y, id, width, height)) {
                    currentPressedIDs.put(id, true);
                } else if (cameraId == -1) {





                    cameraId = id;
                    cameraX = x;
                    cameraY = y;
                    org.stjr.srb2.SRB2Game.nativeOnTouchDown(id, x, y);
                }
            } else if ((action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP || action == MotionEvent.ACTION_CANCEL) && !org.stjr.srb2.SRB2Game.nativeOnTouchUp(id, x, y)) {
                if (currentPressedIDs.containsKey(id)) {
                    currentLayout.handleTouch(action, x, y, id, width, height);
                    currentPressedIDs.remove(id);
                } else if (id == cameraId) {
                    cameraId = -1;
                }
            }
        }

        invalidate();
        return true;  
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (currentLayout == null) return;
        currentLayout.draw(canvas, paint, pressedPaint, textPaint);
    }
}



































































































