package org.stjr.srb2.layout;

import android.view.KeyEvent;

import org.stjr.srb2.touch.Button;
import org.stjr.srb2.touch.Joystick;
import org.stjr.srb2.touch.Layout;

public class Gameplay extends Layout {
    private static final float finalOffset = 80f;
    private static final float finalRadius = 60;
    private static final float finalSpacing = 130;
    private static final float finalJoystickRadius = 150;
    private static final float finalJoystickSpacing = 100;
    private static final float screenOffset = 25;
    private static final float utilityButtonX = -175;
    private static final float utilityButtonTop = 100;
    private static final float utilityButtonSpacing = 115;
    public Gameplay() {
        this.buttons = new Button[]{

            new Joystick(
                    0f,  
                    1f,  
                    screenOffset + finalJoystickRadius,  
                    -finalJoystickRadius - screenOffset,  
                    finalJoystickRadius - finalJoystickSpacing,  
                    finalJoystickSpacing,  
                    KeyEvent.KEYCODE_A,
                    KeyEvent.KEYCODE_S,
                    KeyEvent.KEYCODE_W,
                    KeyEvent.KEYCODE_D
            ),

            new Button(
                    1f,  
                    1f,  
                    -finalOffset - screenOffset,  
                    -finalOffset - screenOffset,  
                    finalRadius,  
                    "Jump",
                    KeyEvent.KEYCODE_SPACE
            ),
            new Button(
                    1f,  
                    1f,  
                    -finalOffset - screenOffset - finalSpacing,  
                    -finalOffset - screenOffset,  
                    finalRadius,  
                    "Spin",
                    KeyEvent.KEYCODE_SHIFT_LEFT
            ),
            new Button(
                    1f,
                    0f,
                    utilityButtonX,
                    utilityButtonTop,
                    50,
                    "Menu",
                    KeyEvent.KEYCODE_ESCAPE
            ),
            new Button(
                    1f,
                    0f,
                    utilityButtonX,
                    utilityButtonTop + utilityButtonSpacing,
                    50,
                    "Chat",
                    KeyEvent.KEYCODE_T
            ),
            new Button(
                    1f,
                    0f,
                    utilityButtonX,
                    utilityButtonTop + (utilityButtonSpacing * 2),
                    50,
                    "Console",
                    KeyEvent.KEYCODE_GRAVE
            )
        };
    }
}
