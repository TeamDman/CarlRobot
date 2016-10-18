package ca.dominicphillips.carlapplication;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.webkit.WebView;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.HashMap;

/**
 * Created by s401321 on 26/09/2016.
 */
public class ActionActivity extends Activity {
    private WebView mWebView;
    private TextView t_t_freq;
    private TextView t_t_dur;
    private TextView t_m_left;
    private TextView t_m_right;
    private TextView t_c_r;
    private TextView t_c_g;
    private TextView t_c_b;
    private SeekBar s_t_freq;
    private SeekBar s_t_dur;
    private SeekBar s_m_left;
    private SeekBar s_m_right;
    private SeekBar s_c_r;
    private SeekBar s_c_g;
    private SeekBar s_c_b;
    HashMap<SeekBar, String> seekPairs = new HashMap<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.action_layout);
//        mWebView= (WebView) findViewById(R.id.webview_actions);
//        WebSettings webSettings = mWebView.getSettings();
//        webSettings.setJavaScriptEnabled(true);
//        webSettings.setAllowUniversalAccessFromFileURLs(true);
//        mWebView.loadUrl("file:///android_asset/actions.html");
        t_t_freq = (TextView) findViewById(R.id.text_tone_freq);
        t_t_dur = (TextView) findViewById(R.id.text_tone_dur);
        t_m_left = (TextView) findViewById(R.id.text_motor_left);
        t_m_right = (TextView) findViewById(R.id.text_motor_right);
        t_c_r = (TextView) findViewById(R.id.text_color_r);
        t_c_g = (TextView) findViewById(R.id.text_color_g);
        t_c_b = (TextView) findViewById(R.id.text_color_b);

        s_t_freq = (SeekBar) findViewById(R.id.seek_tone_freq);
        s_t_dur = (SeekBar) findViewById(R.id.seek_tone_dur);
        s_m_left = (SeekBar) findViewById(R.id.seek_motor_left);
        s_m_right = (SeekBar) findViewById(R.id.seek_motor_right);
        s_c_r = (SeekBar) findViewById(R.id.seek_color_r);
        s_c_g = (SeekBar) findViewById(R.id.seek_color_g);
        s_c_b = (SeekBar) findViewById(R.id.seek_color_b);

        seekPairs.put(s_t_freq, "freq");
        seekPairs.put(s_t_dur, "dur");
//        seekPairs.put(s_m_left ,"");
//        seekPairs.put(s_m_right ,"");
        seekPairs.put(s_c_r, "r");
        seekPairs.put(s_c_g, "g");
        seekPairs.put(s_c_b, "b");
        resetSliders(null);
    }

    public void sendInfo(View view) {
        StringBuilder send = new StringBuilder("?");
        int left = s_m_left.getProgress();
        int right = s_m_right.getProgress();
        send.append("apina=" + (left >= 255 ? 0 : left));
        send.append("&apinb=" + (left <= 255 ? 0 : left - 255));
        send.append("&bpina=" + (right >= 255 ? 0 : right));
        send.append("&bpinb=" + (right <= 255 ? 0 : right - 255));
        for (SeekBar index : seekPairs.keySet())
            send.append("&" + seekPairs.get(index) + "=" + ((SeekBar) index).getProgress());

        System.out.println(send);
        PendingIntent pendingResult = createPendingResult(0, new Intent(), 0);
        Intent intent = new Intent(getApplicationContext(), SendIntentService.class);
        intent.putExtra("url", send.toString());
        intent.putExtra("pending_result", pendingResult);
        startService(intent);
    }

    @Override
    public void onBackPressed() {
        startActivity(new Intent(this, MainActivity.class));
        finish();
    }

    public void resetSliders(View view) {
        s_t_freq.setProgress(0);
        s_t_dur.setProgress(0);
        s_m_left.setProgress(255);
        s_m_right.setProgress(255);
        s_c_r.setProgress(0);
        s_c_g.setProgress(0);
        s_c_b.setProgress(0);
    }
}
