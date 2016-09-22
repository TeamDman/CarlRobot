package ca.dominicphillips.carlview;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;

public class NipsActivity extends AppCompatActivity {
    private WebView myWebView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_nips);
        myWebView = (WebView) findViewById(R.id.activity_nips_webview);
        WebSettings myWebSettings = myWebView.getSettings();
        myWebSettings.setJavaScriptEnabled(true);
        myWebSettings.setAllowUniversalAccessFromFileURLs(true);
        myWebView.loadUrl("file:///android_asset/nips.html");
    }
}
