package org.stjr.srb2.layout;

import android.view.KeyEvent;

import org.stjr.srb2.touch.Button;
import org.stjr.srb2.touch.Joystick;
import org.stjr.srb2.touch.Layout;

public class MenuNavigation extends Layout {
    private static final float finalOffset = 80f;
    private static final float finalRadius = 60;
    private static final float finalSpacing = 130;
    private static final float finalJoystickRadius = 150;
    private static final float finalJoystickSpacing = 100;
    private static final float screenOffset = 25;

    public MenuNavigation() {
        Joystick navigationJoystick = new Joystick(
                0f,
                1f,
                screenOffset + finalJoystickRadius,
                -finalJoystickRadius - screenOffset,
                finalJoystickRadius - finalJoystickSpacing,
                finalJoystickSpacing,
                KeyEvent.KEYCODE_DPAD_LEFT,
                KeyEvent.KEYCODE_DPAD_DOWN,
                KeyEvent.KEYCODE_DPAD_UP,
                KeyEvent.KEYCODE_DPAD_RIGHT
        );
        navigationJoystick.setDirectionalAutoRepeat(true);

        this.buttons = new Button[]{
                // First, we will create a joystick so the player can move.
                navigationJoystick,

                // Finally, a simple button.
                new Button(
                        1f, // The X scale across the screen.
                        1f, // The Y scale across the screen.
                        -finalOffset - screenOffset, // The X offset for the button.
                        -finalOffset - screenOffset, // The Y offset for the button.
                        finalRadius, // The width of the button.
                        "Select",
                        KeyEvent.KEYCODE_ENTER
                ),
                new Button(
                        1f, // The X scale across the screen.
                        1f, // The Y scale across the screen.
                        -finalOffset - screenOffset - finalSpacing, // The X offset for the button.
                        -finalOffset - screenOffset, // The Y offset for the button.
                        finalRadius, // The width of the button.
                        "Back",
                        KeyEvent.KEYCODE_ESCAPE
                ),
                new Button(
                        1f,
                        0f,
                        -175,
                        100,
                        50,
                        "Console",
                        KeyEvent.KEYCODE_GRAVE
                ),
                new Button(
                        1f,
                        0f,
                        -175,
                        215,
                        50,
                        "Chat",
                        KeyEvent.KEYCODE_T
                )
        };
    }
}
