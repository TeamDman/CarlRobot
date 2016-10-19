package ca.dominicphillips.carlapplication;

import android.app.IntentService;
import android.app.PendingIntent;
import android.content.Intent;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * Created by s401321 on 18/10/2016.
 */
public class SendIntentService extends IntentService {
    public SendIntentService() {
        super("SendService");
    }

    public static final int RESULT_CODE = 0;
    public static final int INVALID_URL_CODE = 1;
    public static final int ERROR_CODE = 2;

    @Override
    protected void onHandleIntent(Intent intent) {
        PendingIntent reply = intent.getParcelableExtra("pending_result");
        InputStream in = null;
        try {
            try {
                URL url = new URL("http://192.168.4.1/" + intent.getStringExtra("url"));
                System.out.println(".\n.\nSENDING\n"+url.toString()+"\n.\n.");
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                try {
                    in = new BufferedInputStream(urlConnection.getInputStream());
                    System.out.println(in);
                } finally {
                    urlConnection.disconnect();
                }

            } catch (MalformedURLException exc) {
                reply.send(INVALID_URL_CODE);
            } catch (Exception exc) {
                // could do better by treating the different sax/xml exceptions individually
                reply.send(ERROR_CODE);
            }
        } catch (PendingIntent.CanceledException exc) {
            exc.printStackTrace();
        }
    }
}
