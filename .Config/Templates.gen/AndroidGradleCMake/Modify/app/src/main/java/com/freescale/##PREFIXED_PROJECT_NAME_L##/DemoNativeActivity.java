/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Modified to fit the demoframework 2015 

package com.freescale.##PREFIXED_PROJECT_NAME_L##;

import com.freescale.##PREFIXED_PROJECT_NAME_L##.R;
import com.freescale.demoplatform.ShellSplitter;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.NativeActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowManager.LayoutParams;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import java.util.List;

public class DemoNativeActivity extends NativeActivity {
    private List<String> m_commandLineArgs = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 19)
        {
            setImmersiveSticky();

            View decorView = getWindow().getDecorView();
            decorView.setOnSystemUiVisibilityChangeListener
                    (new View.OnSystemUiVisibilityChangeListener() {
                @Override
                public void onSystemUiVisibilityChange(int visibility) {
                    setImmersiveSticky();
                }
            });
        }

        String commandLineArgs = getIntent().getStringExtra("args");
        if(commandLineArgs != null)
        {
            m_commandLineArgs = ShellSplitter.Split(commandLineArgs);
        }
    }

    @TargetApi(19)    
    protected void onResume() {
        super.onResume();

        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        //if(SDK_INT >= 11 && SDK_INT < 14)
        //{
        //    getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
        //}
        //else if(SDK_INT >= 14 && SDK_INT < 19)
        if(SDK_INT >= 14 && SDK_INT < 19)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LOW_PROFILE);
        }
        else if(SDK_INT >= 19)
        {
            setImmersiveSticky();
        }

    }
    // Our popup window, you will call it from your C/C++ code later

    @TargetApi(19)    
    void setImmersiveSticky() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    DemoNativeActivity _activity;
    PopupWindow _popupWindow;
    TextView _label;

    public int GetCommandLineArgumentCount()
    {
        return m_commandLineArgs != null ? m_commandLineArgs.size() : 0;
    }

    public String TryGetCommandLineArgument(int index)
    {
        return m_commandLineArgs != null && index >= 0 && index < m_commandLineArgs.size() ? m_commandLineArgs.get(index)  : null;
    }

    @SuppressLint("InflateParams")
    public void showUI()
    {
        if( _popupWindow != null )
            return;

        _activity = this;

        this.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                LayoutInflater layoutInflater
                = (LayoutInflater)getBaseContext()
                .getSystemService(LAYOUT_INFLATER_SERVICE);
                View popupView = layoutInflater.inflate(R.layout.widgets, null);
                _popupWindow = new PopupWindow(
                        popupView,
                        LayoutParams.WRAP_CONTENT,
                        LayoutParams.WRAP_CONTENT);

                LinearLayout mainLayout = new LinearLayout(_activity);
                MarginLayoutParams params = new MarginLayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
                params.setMargins(0, 0, 0, 0);
                _activity.setContentView(mainLayout, params);

                // Show our UI over NativeActivity window
                _popupWindow.showAtLocation(mainLayout, Gravity.TOP | Gravity.START, 10, 10);
                _popupWindow.update();

                _label = (TextView)popupView.findViewById(R.id.textViewFPS);

            }});
    }

    protected void onPause()
    {
        super.onPause();
    }

    public void updateFPS(final float fFPS)
    {
        if( _label == null )
            return;

        _activity = this;
        this.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                _label.setText(String.format("%2.2f FPS", fFPS));

            }});
    }
}


