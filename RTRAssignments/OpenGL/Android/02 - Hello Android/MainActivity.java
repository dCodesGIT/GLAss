package com.rtr3_android.apptitle;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import androidx.appcompat.widget.AppCompatTextView;
import android.graphics.Color;
import android.view.Gravity;

public class MainActivity extends AppCompatActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		AppCompatTextView myTextView = new AppCompatTextView(this);

		super.onCreate(savedInstanceState);

		getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));
	/*	Win = getWindow();
		DView = Win.getDecorView();
		DView.setBackgroundColor(Color.rgb(0, 0, 0));
	*/

		myTextView.setText("Hello Android !!!");
		myTextView.setTextSize(32);
		myTextView.setTextColor(Color.rgb(0, 255, 0));
		myTextView.setGravity(Gravity.CENTER);
		myTextView.setBackgroundColor(Color.rgb(0, 0, 0));

	//	setContentView(R.layout.activity_main);
		setContentView(myTextView);
	}
}