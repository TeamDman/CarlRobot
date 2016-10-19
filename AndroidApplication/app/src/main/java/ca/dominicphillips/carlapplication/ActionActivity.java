package ca.dominicphillips.carlapplication;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.webkit.WebView;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by s401321 on 26/09/2016.
 */
public class ActionActivity extends Activity {

    private SeekBar s_t_freq;
    private SeekBar s_t_dur;
    private SeekBar s_m_left;
    private SeekBar s_m_right;
    private SeekBar s_c_r;
    private SeekBar s_c_g;
    private SeekBar s_c_b;
    private CheckBox c_line;

    HashMap<SeekBar, String> seekPairs = new HashMap<>();
    ArrayList<SeekBar> seekBars = new ArrayList<SeekBar>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.action_layout);

        s_t_freq = (SeekBar) findViewById(R.id.seek_tone_freq);
        s_t_dur = (SeekBar) findViewById(R.id.seek_tone_dur);
        s_m_left = (SeekBar) findViewById(R.id.seek_motor_left);
        s_m_right = (SeekBar) findViewById(R.id.seek_motor_right);
        s_c_r = (SeekBar) findViewById(R.id.seek_color_r);
        s_c_g = (SeekBar) findViewById(R.id.seek_color_g);
        s_c_b = (SeekBar) findViewById(R.id.seek_color_b);
        c_line = (CheckBox) findViewById(R.id.check_line);

        seekPairs.put(s_t_freq, "freq");
        seekPairs.put(s_t_dur, "dur");
        seekPairs.put(s_c_r, "r");
        seekPairs.put(s_c_g, "g");
        seekPairs.put(s_c_b, "b");
        resetSliders(null);
        for (SeekBar bar : seekPairs.keySet())
            seekBars.add(bar);
        seekBars.add(s_m_left);
        seekBars.add(s_m_right);

        System.out.println(seekBars);
        System.out.println(findViewById(R.id.check_line));

       c_line.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                for (SeekBar bar : seekBars)
                    bar.setIndeterminate(isChecked);
            }
        });
    }

    public void sendInfo(View view) {
        StringBuilder send = new StringBuilder("?");
        int left = s_m_left.getProgress();
        int right = s_m_right.getProgress();
        send.append("apina=" + (left >= 255 ? 0 : 255-left));
        send.append("&apinb=" + (left <= 255 ? 0 : left - 255));
        send.append("&bpina=" + (right >= 255 ? 0 : 255-right));
        send.append("&bpinb=" + (right <= 255 ? 0 : right - 255));
        send.append("&mode="+(c_line.isChecked()?"1":"0"));
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
