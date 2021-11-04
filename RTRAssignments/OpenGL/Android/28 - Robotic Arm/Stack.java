package com.rtr3_android.roboticarm;

import android.opengl.Matrix;

public class Stack {
	private int stackSize;
	private float[][] stack;
	private int top = -1;
	
	public Stack() {
		// Code
		stackSize = 10;
		stack = new float[stackSize][16];
	}
	
	public Stack(int size) {
		// Code
		stackSize = size;
		stack = new float[stackSize][16];
	}
	
	public void PushMatrix(float matrix[]) {
		// Code
		if(top > stackSize)
			return;
		stack[++top] = matrix.clone();
	}
	
	public float[] PopMatrix() {
		// Code
		if(top == -1) {
			float[] identity = new float[16];
			Matrix.setIdentityM(identity, 0);
			return identity;
		}
		return  stack[top--].clone();
	}
}
