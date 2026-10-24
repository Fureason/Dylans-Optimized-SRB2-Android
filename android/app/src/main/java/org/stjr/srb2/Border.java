package org.stjr.srb2;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Build;
import android.graphics.Region;
import android.util.Log;
import android.util.LruCache;
import android.view.View;

public class Border extends View {
    private Rect gameRect;
    private Bitmap bitmap;
    private Bitmap _bitmap;  
    private Rect positionRect;
    private Rect bitmapRect;

    public Border(Context context, Bitmap bitmap, Rect gameRect) {
        super(context);
        this.bitmap = bitmap;
        this._bitmap = bitmap;
        this.gameRect = gameRect;
    }

    public void setupBorderPositions(int width, int height) {
        if (width <= 0 || height <= 0 || bitmap == null) return;


        float scale = Math.max((float) width / bitmap.getWidth(), (float) height / bitmap.getHeight());

        float drawWidth = bitmap.getWidth() * scale;
        float drawHeight = bitmap.getHeight() * scale;


        float offsetX = (width - drawWidth) / 2f;
        float offsetY = (height - drawHeight) / 2f;


        positionRect = new Rect((int) offsetX, (int) offsetY, (int) (offsetX + drawWidth), (int) (offsetY + drawHeight));
        

        bitmapRect = new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight());

        Log.d("Border", "Layout: " + width + "x" + height + " -> Drawing border at " + positionRect.toShortString());
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        setupBorderPositions(w, h);
        invalidate();
    }

    public void setBorderBitmap(Bitmap bitmap) {
        this.bitmap = bitmap;
        setupBorderPositions(getWidth(), getHeight());
        invalidate();
    }

    public void setHole(Rect gameRect) {
        this.gameRect = gameRect;
        invalidate();
    }

    public void resetBitmap() {
        this.bitmap = _bitmap;
        setupBorderPositions(getWidth(), getHeight());
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (bitmap == null || gameRect == null || positionRect == null) return;

        canvas.save();


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            canvas.clipOutRect(gameRect);
        } else {
            canvas.clipRect(gameRect, Region.Op.DIFFERENCE);
        }


        canvas.drawBitmap(bitmap, bitmapRect, positionRect, null);

        canvas.restore();
    }
}
