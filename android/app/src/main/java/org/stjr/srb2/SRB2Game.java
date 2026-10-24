package org.stjr.srb2;

import org.libsdl.app.SDLActivity;
import org.stjr.srb2.touch.MasterControls;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.util.LruCache;
import android.view.MotionEvent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class SRB2Game extends SDLActivity {
	private static final int BUNDLED_DATA_VERSION = 55202;
	private static final String[] BUNDLED_DATA_FILES = {
			"srb2.pk3", "zones.pk3", "characters.pk3", "music.pk3"
	};
	private static final long[] BUNDLED_DATA_SIZES = {
			50115266L, 18503511L, 2044315L, 99639560L
	};
	private static final String MASTER_SERVER_CERTIFICATE = "hms-cert.pem";
	private static final long MASTER_SERVER_CERTIFICATE_SIZE = 225619L;

	public static MasterControls masterTouchClass;

	public static Border gameBorder;

	public static int gameWidth = 0;
	public static int gameHeight = 0;
	public static int gameDup = 0;

	private int lastLayoutWidth = 0;
	private int lastLayoutHeight = 0;

	private final Handler updateHandler = new Handler();

	public static native int nativeGetGameWidth();
	public static native int nativeGetGameHeight();
	public static native int nativeGetGameDup();

	public static native boolean nativeOnTouchDown(int id, float x, float y);
	public static native boolean nativeOnTouchUp(int id, float x, float y);

	private static LruCache<String, Bitmap> borderCache;

	static {

		final int maxMemory = (int) (Runtime.getRuntime().maxMemory() / 1024);
		final int cacheSize = maxMemory / 8;

		borderCache = new LruCache<String, Bitmap>(cacheSize) {
			@Override
			protected int sizeOf(String key, Bitmap bitmap) {
				return bitmap.getByteCount() / 1024;
			}
		};
	}

	public static void preloadBorderImage(final String name, final byte[] data) {
		mSingleton.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (gameBorder == null) return;


				Bitmap bitmap = borderCache.get(name);

				if (bitmap == null && data != null) {

					bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
					if (bitmap != null) {
						borderCache.put(name, bitmap);
					}
				}
			}
		});
	}

	public static void updateBorderImage(final String name, final byte[] data) {
		mSingleton.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (gameBorder == null) return;


				Bitmap bitmap = borderCache.get(name);

				if (bitmap == null && data != null) {

					bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
					if (bitmap != null) {
						borderCache.put(name, bitmap);
					}
				}

				if (bitmap != null) {
					gameBorder.setBorderBitmap(bitmap);
				}
			}
		});
	}

	public static boolean checkPermission(String permission) {
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M)
			return true;

		Activity activity = (Activity)getContext();
        return activity.checkSelfPermission(permission) == PackageManager.PERMISSION_GRANTED;
    }

	public static boolean inMultiWindowMode() {
		if (Build.VERSION.SDK_INT >= 24) {
            return SRB2Game.mSingleton.isInMultiWindowMode();
		}
		return false;
	}
	private final Runnable updateRunnable = new Runnable() {
		@Override
		public void run() {
			int newWidth = nativeGetGameWidth();
			int newHeight = nativeGetGameHeight();
			int newDup = nativeGetGameDup();

			int curLayoutW = mLayout != null ? mLayout.getWidth() : 0;
			int curLayoutH = mLayout != null ? mLayout.getHeight() : 0;

			if (newWidth != gameWidth || newHeight != gameHeight || newDup != gameDup
					|| curLayoutW != lastLayoutWidth || curLayoutH != lastLayoutHeight) {
				
				onGameResolutionChanged(newWidth, newHeight, newDup);

				if (curLayoutW > 0 && curLayoutH > 0 && newWidth > 0 && newHeight > 0) {
					gameWidth = newWidth;
					gameHeight = newHeight;
					gameDup = newDup;
					lastLayoutWidth = curLayoutW;
					lastLayoutHeight = curLayoutH;
				}
			}

			updateHandler.postDelayed(this, 200);
		}
	};

	protected void onGameResolutionChanged(int width, int height, int dup) {
		Log.d("SRB2", "Resolution changed: " + width + "x" + height + " (dup: " + dup + ")");

		int screenW = mLayout.getWidth();
		int screenH = mLayout.getHeight();

		if (screenW == 0 || screenH == 0 || width == 0 || height == 0)
			return;



		Rect hole = new Rect(0, 0, screenW, screenH);

		if (gameBorder != null) {
			gameBorder.setHole(hole);
		}
	}

	static void resetBorder() {
		gameBorder.resetBitmap();
	}

	private boolean hasBundledGameData() throws IOException {
		String[] assets = getAssets().list("");
		if (assets == null)
			return false;
		for (String asset : assets) {
			if (BUNDLED_DATA_FILES[0].equals(asset))
				return true;
		}
		return false;
	}

	private boolean installBundledGameData() {
		try {
			if (!hasBundledGameData())
				return true;

			File[] mediaDirs = getExternalMediaDirs();
			File dataDir = (mediaDirs != null && mediaDirs.length > 0 && mediaDirs[0] != null)
					? new File(mediaDirs[0], "SRB2 v2.2") : null;
			if (dataDir == null)
				throw new IOException("Android did not provide a public media directory.");
			if (!dataDir.exists() && !dataDir.mkdirs())
				throw new IOException("Could not create " + dataDir.getAbsolutePath());

			SharedPreferences preferences = getSharedPreferences("bundled-data", MODE_PRIVATE);
			boolean current = preferences.getInt("version", 0) == BUNDLED_DATA_VERSION;
			for (int i = 0; i < BUNDLED_DATA_FILES.length; i++) {
				File destination = new File(dataDir, BUNDLED_DATA_FILES[i]);
				if (current && destination.isFile() && destination.length() == BUNDLED_DATA_SIZES[i])
					continue;

				File temporary = new File(dataDir, BUNDLED_DATA_FILES[i] + ".tmp");
				if (temporary.exists() && !temporary.delete())
					throw new IOException("Could not replace " + temporary.getName());

				long copied = 0;
				byte[] buffer = new byte[256 * 1024];
				try (InputStream input = getAssets().open(BUNDLED_DATA_FILES[i]);
					 FileOutputStream output = new FileOutputStream(temporary)) {
					int count;
					while ((count = input.read(buffer)) != -1) {
						output.write(buffer, 0, count);
						copied += count;
					}
					output.flush();
				}

				if (copied != BUNDLED_DATA_SIZES[i]) {
					temporary.delete();
					throw new IOException(BUNDLED_DATA_FILES[i] + " has an unexpected size.");
				}
				if (destination.exists() && !destination.delete())
					throw new IOException("Could not update " + destination.getName());
				if (!temporary.renameTo(destination))
					throw new IOException("Could not install " + destination.getName());
			}

			                                                                 
                                                                
			File certificate = new File(getFilesDir(), MASTER_SERVER_CERTIFICATE);
			if (!certificate.isFile() || certificate.length() != MASTER_SERVER_CERTIFICATE_SIZE) {
				File temporary = new File(getFilesDir(), MASTER_SERVER_CERTIFICATE + ".tmp");
				if (temporary.exists() && !temporary.delete())
					throw new IOException("Could not replace " + temporary.getName());

				long copied = 0;
				byte[] buffer = new byte[64 * 1024];
				try (InputStream input = getAssets().open("hms/cert");
					 FileOutputStream output = new FileOutputStream(temporary)) {
					int count;
					while ((count = input.read(buffer)) != -1) {
						output.write(buffer, 0, count);
						copied += count;
					}
					output.flush();
				}
				if (copied != MASTER_SERVER_CERTIFICATE_SIZE) {
					temporary.delete();
					throw new IOException("The HTTPS trust bundle has an unexpected size.");
				}
				if (certificate.exists() && !certificate.delete())
					throw new IOException("Could not update " + certificate.getName());
				if (!temporary.renameTo(certificate))
					throw new IOException("Could not install " + certificate.getName());
			}

			if (!preferences.edit().putInt("version", BUNDLED_DATA_VERSION).commit())
				throw new IOException("Could not record the installed data version.");
			return true;
		} catch (IOException error) {
			Log.e("SRB2", "Could not install bundled game data", error);
			return false;
		}
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if (!installBundledGameData()) {
			mBrokenLibraries = true;
			new AlertDialog.Builder(this)
					.setTitle("SRB2 data error")
					.setMessage("The bundled SRB2 2.2.15 game data could not be installed. Please reinstall the app.")
					.setPositiveButton("Exit", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							finish();
						}
					})
					.setCancelable(false)
					.show();
			return;
		}




		gameBorder = new Border(this, null, new Rect(0, 0, 0, 0));
		masterTouchClass = new MasterControls(this, "FakeNothing");

		mLayout.addView(gameBorder);
		mLayout.addView(masterTouchClass);

		updateHandler.post(updateRunnable);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		updateHandler.removeCallbacks(updateRunnable);
	}

	@Override
	protected void onResume() {
		super.onResume();
		SDLActivity.setWindowStyle(true);
		if (masterTouchClass != null)
			masterTouchClass.setControlsVisible(true);
	}
}
