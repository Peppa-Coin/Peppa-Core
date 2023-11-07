package org.peppacoincore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class PeppacoinQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File peppacoinDir = new File(getFilesDir().getAbsolutePath() + "/.peppacoin");
        if (!peppacoinDir.exists()) {
            peppacoinDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
