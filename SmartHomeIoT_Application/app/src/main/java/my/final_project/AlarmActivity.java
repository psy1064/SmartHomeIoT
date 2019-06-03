package my.final_project;

import android.media.MediaPlayer;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;

import java.util.Calendar;

public class AlarmActivity extends AppCompatActivity {
    Calendar calendar = Calendar.getInstance();
    TextView hourText, minuteText, testText;
    Button stopButton;
    MediaPlayer mediaPlayer;
    CheckBox checkBox;
    Button alarmButton;
    ImageView alarmImage;
    int alarmMode = 0;

    public static final int MODE_REQUEST = 1 ;
    public static final int MESSAGE_WRITE = 2;
    private static final int STATE_SENDING = 1;
    private static final int STATE_NO_SENDING = 2;

    private int mSendingState ;

    private static final String TAG = "TEST+Alarmactivity";

    private synchronized void sendMessage( String message, int mode ) {

        if ( mSendingState == STATE_SENDING ) {
            try {
                wait() ;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        mSendingState = STATE_SENDING ;

        // Check that we're actually connected before trying anything
        if ( MainActivity.bluetoothServiceMain.getState() != BluetoothService.STATE_CONNECTED ) {
            mSendingState = STATE_NO_SENDING ;
            return ;
        }

        // Check that there's actually something to send
        if ( message.length() > 0 ) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = message.getBytes() ;
            MainActivity.bluetoothServiceMain.write(send, mode) ;

            // Reset out string buffer to zero and clear the edit text field
        }
        mSendingState = STATE_NO_SENDING ;
//      notify();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD
                | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        // 잠금 화면 위로 activity 띄워줌
        // https://cofs.tistory.com/173

        if(MainActivity.bluetoothServiceMain.getState()==BluetoothService.STATE_CONNECTED) {
            Log.d(TAG,"send success");
            sendMessage("on.", MODE_REQUEST); // Atmega128에 전등 키라는 명령 전송
            Toast.makeText(getApplicationContext(),"Turn On", Toast.LENGTH_LONG).show();
        }
        else {
            Log.e(TAG, "블루투스 연결 오류");
        }


        hourText = (TextView)findViewById(R.id.hourText);
        minuteText = (TextView)findViewById(R.id.minuteText);
        stopButton = (Button)findViewById(R.id.stopButton);
        alarmButton = (Button)findViewById(R.id.setAlarmTimeButton);
        checkBox = (CheckBox)findViewById(R.id.checkBox);
        alarmImage = (ImageView)findViewById(R.id.alarmImage);
        testText = (TextView)findViewById(R.id.textView);

        Log.d(TAG, "second = " + calendar.get(Calendar.SECOND));
        testText.setText("Second = " + calendar.get(Calendar.SECOND));
        if(calendar.get(Calendar.HOUR_OF_DAY) > 0 && calendar.get(Calendar.HOUR_OF_DAY) < 12) {
            hourText.setText("오전 " + calendar.get(Calendar.HOUR_OF_DAY)  +  "시 ");
            minuteText.setText(calendar.get(Calendar.MINUTE) + "분");
        }
        else if(calendar.get(Calendar.HOUR_OF_DAY) == 12) {
            hourText.setText("오후 " + calendar.get(Calendar.HOUR_OF_DAY)  +  "시 ");
            minuteText.setText(calendar.get(Calendar.MINUTE) + "분");
        }
        else if(calendar.get(Calendar.HOUR_OF_DAY) > 12 && calendar.get(Calendar.HOUR_OF_DAY) <24) {
            hourText.setText("오후 " + (calendar.get(Calendar.HOUR_OF_DAY)-12)  +  "시 ");
            minuteText.setText(calendar.get(Calendar.MINUTE) + "분");
        }
        else if(calendar.get(Calendar.HOUR_OF_DAY) == 0) {
            hourText.setText("오전 0시");
            minuteText.setText(calendar.get(Calendar.MINUTE) + "분");
        }
        alarmMode = MainActivity.alarmMode;
        Log.d("alarmMode","alarmMode" + alarmMode);

        switch (alarmMode) {
            case 0: {
                mediaPlayer = MediaPlayer.create(getApplicationContext(), R.raw.ringtone);
                alarmImage.setImageResource(R.drawable.alarm);
                break;
            }
            case 1: {
                mediaPlayer = MediaPlayer.create(getApplicationContext(), R.raw.bell);
                Glide.with(getApplicationContext())
                        .load(R.drawable.electronic)
                        .fitCenter()
                        .into(alarmImage);
                // gif
                break;
            }
            case 2: {
                mediaPlayer = MediaPlayer.create(getApplicationContext(), R.raw.ssibal);
                Glide.with(getApplicationContext())
                        .load(R.drawable.somis)
                        .fitCenter()
                        .into(alarmImage);
                // gif
                break;
            }

        }
        mediaPlayer.setLooping(true);
        mediaPlayer.start();

        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mediaPlayer.stop();
                finish();
            }
        });
    }
}
