package com.rtr3_android.eventHandling;

import android.content.Context;
import android.graphics.Color;
import androidx.appcompat.widget.AppCompatTextView;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.view.Gravity;

public class MyView extends AppCompatTextView implements OnGestureListener, OnDoubleTapListener {

	private GestureDetector gestureDetector;

	MyView(Context context) {
		super(context);

		setGravity(Gravity.CENTER);
		setBackgroundColor(Color.rgb(0, 0, 0));
		setTextColor(Color.rgb(0, 255, 0));
		setTextSize(64);
		setText("Hello Android !!!");

		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// Code
		int eventAction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return (true);
	}

	@Override
	public boolean onDoubleTap(MotionEvent event) {
		// Code
		setText("Double Tap");

		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent event) {
		// Code
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent event) {
		// Code
		setText("Single Tap");

		return (true);
	}

	@Override
	public boolean onDown(MotionEvent event) {
		return (true);
	}

	@Override
	public void onLongPress(MotionEvent event) {
		setText("Long Press");
	}

	@Override
	public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
		setText("Scroll");
		System.exit(0);
		return (true);
	}

	@Override
	public void onShowPress(MotionEvent event) {
	}

	@Override
	public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY) {
		return (true);
	}

	@Override
	public boolean onSingleTapUp(MotionEvent event) {
		return (true);
	}
}
