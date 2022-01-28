package com.Raunak.mods;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;


public abstract class Raunak3 extends TextView {
    public Raunak3(Context context) {
        this(context, null);
    }

    public Raunak3(Context context, AttributeSet attributeSet) {
        this(context, attributeSet, 0);
    }

    public Raunak3(Context context, AttributeSet attributeSet, int i) {
        super(context, attributeSet, i);
    }

    public abstract void animateText(CharSequence charSequence);

    public abstract void setAnimationListener(Raunak1 animationListener);

    public abstract void setProgress(float f);
}

