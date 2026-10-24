package org.stjr.srb2.layout;

import android.view.KeyEvent;

import org.stjr.srb2.touch.Button;
import org.stjr.srb2.touch.Layout;
import org.stjr.srb2.touch.ButtonRectangle;

public class FakeNothing extends Layout {
    public FakeNothing() {


        this.buttons = new Button[]{
                new ButtonRectangle(
                       0.5f, 0.5f, 0, 0, 1.0f, 1.0f, 0, 0, "", KeyEvent.KEYCODE_ENTER
                )
        };
        this.buttons[0].visible = false;  
    }
}
