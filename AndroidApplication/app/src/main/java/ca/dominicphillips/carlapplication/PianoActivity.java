package ca.dominicphillips.carlapplication;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Button;

import java.util.ArrayList;

/**
 * Created by s401321 on 21/10/2016.
 */
public class PianoActivity extends Activity {
    ArrayList<Button> keys = new ArrayList<Button>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.piano_layout);
        ViewGroup group = (ViewGroup) findViewById(R.id.layout_keys);
        for (int index=0; index<group.getChildCount()-1; index++) {
            Button btn = (Button) group.getChildAt(index);
            final int pass=index;
            btn.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
//                    System.out.println(MotionEvent.actionToString(event.getAction()));
                    if (event.getAction()==MotionEvent.ACTION_DOWN) {
                       sendToRobot("?freq=" + ((pass + 1) * 300));
//                        sendToRobot("?freq=" + Math.pow(2, (((pass + 28) - 49) / 12)) * 440);
                    } else if (event.getAction()==MotionEvent.ACTION_UP){
                        sendToRobot("?freq=0");
                    }
                    return true;
                }
            });
        }
    }

    public void onPianoPressed(View view) {

    }

    public void sendToRobot(String toSend) {
        PendingIntent pendingResult = createPendingResult(0, new Intent(), 0);
        Intent intent = new Intent(getApplicationContext(), SendIntentService.class);
        intent.putExtra("url", toSend);
        intent.putExtra("pending_result", pendingResult);
        startService(intent);
    }
}
