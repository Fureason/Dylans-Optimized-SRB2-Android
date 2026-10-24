package org.stjr.srb2.touch;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.MotionEvent;
import org.libsdl.app.SDLActivity;

public class Button {


    public float scaleX, scaleY, offsetX, offsetY, radius;
    public String name;
    public Integer bind;
    public boolean inputDown;
    public int touchId = -1;
    public boolean visible = true;
    private boolean autoRepeat;
    private static final long AUTO_REPEAT_DELAY_MS = 350;
    private static final long AUTO_REPEAT_INTERVAL_MS = 95;
    private final Handler repeatHandler = new Handler(Looper.getMainLooper());
    private final Runnable repeatRunnable = new Runnable() {
        @Override
        public void run() {
            if (!autoRepeat || !inputDown || bind == null)
                return;
            SDLActivity.onNativeKeyUp(bind);
            SDLActivity.onNativeKeyDown(bind);
            repeatHandler.postDelayed(this, AUTO_REPEAT_INTERVAL_MS);
        }
    };

    public Button(float sx, float sy, float ox, float oy, float r, String n, Integer b) {
        this.scaleX = sx;
        this.scaleY = sy;
        this.offsetX = ox;
        this.offsetY = oy;
        this.radius = r;
        this.name = n;
        this.bind = b;
    }

    public void setAutoRepeat(boolean enabled) {
        autoRepeat = enabled;
        if (!enabled)
            repeatHandler.removeCallbacks(repeatRunnable);
    }

    public float[] getXYPosition(int width, int height) {
        return new float[]{ (width * scaleX) + offsetX, (height * scaleY) + offsetY };
    }

    public boolean isPointInside(float x, float y, int width, int height) {
        float[] pos = getXYPosition(width, height);
        float dx = x - pos[0];
        float dy = y - pos[1];
        return (dx * dx + dy * dy) <= (radius * radius);
    }

    public void press(boolean activate, int id) {
        if (this.inputDown == activate) return;  
        this.inputDown = activate;
        this.touchId = activate ? id : -1;

        Integer keyCode = this.bind;
        if (keyCode != null) {
            if (activate) {
                SDLActivity.onNativeKeyDown(keyCode);
                if (autoRepeat) {
                    repeatHandler.removeCallbacks(repeatRunnable);
                    repeatHandler.postDelayed(repeatRunnable, AUTO_REPEAT_DELAY_MS);
                }
            } else {
                repeatHandler.removeCallbacks(repeatRunnable);
                SDLActivity.onNativeKeyUp(keyCode);
            }
        }
    }

    public boolean handleTouch(int action, float x, float y, int id, int w, int h) {
        boolean inside = isPointInside(x, y, w, h);

        if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) {
            if (inside && !this.inputDown) {
                press(true, id);
                Log.i("TouchButton", "Pressed (HARD): " + id);
            }
        } else if (action == MotionEvent.ACTION_MOVE) {
            if (this.touchId == id && !inside && this.inputDown) {
                press(false, id);


            } else if (this.touchId == -1 && inside && !this.inputDown) {

                press(true, id);
                Log.i("TouchButton", "Pressed (SOFT): " + id);
            }
        } else if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP || action == MotionEvent.ACTION_CANCEL) {
            if (this.touchId == id && this.inputDown) {
                press(false, -1);
                Log.i("TouchButton", "Released: " + id);
            }
        }
        return inside;
    }

    public void draw(Canvas canvas, Paint paint, Paint pressedPaint, Paint textPaint) {
        if (!visible) return;

        float[] pos = getXYPosition(canvas.getWidth(), canvas.getHeight());
        canvas.drawCircle(pos[0], pos[1], radius, inputDown ? pressedPaint : paint);
        float textY = pos[1] - ((textPaint.descent() + textPaint.ascent()) / 2);
        canvas.drawText(name, pos[0], textY, textPaint);
    }
}
