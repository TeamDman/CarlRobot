package ca.dominicphillips.carlapplication;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    WifiManager manager;
    WifiInfo info;
    BroadcastReceiver receiver;

    @Override
    public void onBackPressed() {
        finishAffinity();
    }

//    @Override
//    protected void onStop() {
//        try {
//            unregisterReceiver(receiver);
//            receiver=null;
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        super.onStop();
//    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);

        manager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        ((TextView) findViewById(R.id.text_update)).setText("Build " + new Date(BuildConfig.TIMESTAMP).toString().replaceAll("GMT\\+\\d+:\\d+", ""));
        updateTitle();

        registerReceiver(receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                updateTitle();
                String ssid = manager.getConnectionInfo().getSSID();
                if (!(ssid.contains("unknown") || ssid.contains("0x")))
                Toast.makeText(getApplicationContext(), "Connected to " + ssid, Toast.LENGTH_SHORT).show();
            }
        }, new IntentFilter(WifiManager.NETWORK_STATE_CHANGED_ACTION));
    }

    public void showController(View view) {
        startActivity(new Intent(this, ControllerActivity.class));
    }

    public void showActions(View view) {
        startActivity(new Intent(this, ActionActivity.class));
    }

    public void showRobotList(View view) {
        startActivity(new Intent(this, RobotListActivity.class));
    }

    public void showPiano(View view) {
        startActivity(new Intent(this,PianoActivity.class));
    }

    public void restartRobot(View view) {
        sendToRobot("?restart=1");
    }

    public void updateTitle() {
        getSupportActionBar().setTitle("Connected to " + manager.getConnectionInfo().getSSID());
    }

    public void sendToRobot(String toSend) {
        PendingIntent pendingResult = createPendingResult(0, new Intent(), 0);
        Intent intent = new Intent(getApplicationContext(), SendIntentService.class);
        intent.putExtra("url", toSend);
        intent.putExtra("pending_result", pendingResult);
        startService(intent);
    }

}

