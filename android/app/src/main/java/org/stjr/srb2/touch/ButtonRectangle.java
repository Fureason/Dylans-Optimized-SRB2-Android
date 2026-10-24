package org.stjr.srb2.touch;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;

public class ButtonRectangle extends Button {
    public float widthScaleX, heightScaleY;
    public float widthOffsetX, heightOffsetY;


    public ButtonRectangle(float sx, float sy, float ox, float oy, float wsx, float hsy, float wox, float hoy, String n, Integer b) {
        super(sx, sy, ox, oy, 0, n, b);
        this.widthScaleX = wsx;
        this.heightScaleY = hsy;
        this.widthOffsetX = wox;
        this.heightOffsetY = hoy;
    }
    public float[] getXYDimensions(int screenWidth, int screenHeight) {
        return new float[]{
                screenWidth * widthScaleX + widthOffsetX,
                screenHeight * heightScaleY + heightOffsetY
        };
    }

    @Override
    public boolean isPointInside(float x, float y, int screenWidth, int screenHeight) {
        float[] pos = getXYPosition(screenWidth, screenHeight);
        float[] dim = getXYDimensions(screenWidth, screenHeight);


        pos[0] = pos[0] - (dim[0] / 2);
        pos[1] = pos[1] - (dim[1] / 2);

        return  x >= pos[0] && x <= pos[0] + dim[0] &&
                y >= pos[1] && y <= pos[1] + dim[1];
    }

    @Override
    public void draw(Canvas canvas, Paint paint, Paint pressedPaint, Paint textPaint) {
        if (!visible) return;

        float[] pos = getXYPosition(canvas.getWidth(), canvas.getHeight());
        float[] dim = getXYDimensions(canvas.getWidth(), canvas.getHeight());

        pos[0] = pos[0] - (dim[0] / 2);
        pos[1] = pos[1] - (dim[1] / 2);


        RectF rect = new RectF(pos[0], pos[1], pos[0] + dim[0], pos[1] + dim[1]);


        canvas.drawRect(rect, inputDown ? pressedPaint : paint);


        float centerX = pos[0] + (dim[0] / 2);
        float centerY = pos[1] + (dim[1] / 2);


        float textY = centerY - ((textPaint.descent() + textPaint.ascent()) / 2);

        canvas.drawText(name, centerX, textY, textPaint);
    }
}