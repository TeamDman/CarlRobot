package ca.dominicphillips.carlapplication;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;

/**
 * Created by s401321 on 26/09/2016.
 */
public class ControllerActivity extends Activity {
    private WebView mWebView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.controller_layout);
        mWebView= (WebView) findViewById(R.id.webview_controller);
        WebSettings webSettings = mWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setAllowUniversalAccessFromFileURLs(true);
        mWebView.loadUrl("file:///android_asset/joystick.html");
    }
}
