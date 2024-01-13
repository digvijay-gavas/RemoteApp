package com.example;

public class CircularBuffer<T> {
    private final int BUFFER_SIZE;
    private final Object[] circularBuffer;
    private int head = 0;
    private int tail = 0;

    public CircularBuffer(int bufferSize) {
        BUFFER_SIZE = bufferSize;
        circularBuffer = new Object[BUFFER_SIZE];
    }

    public void addToBuffer(T item) {
        circularBuffer[head] = item;
        head = (head + 1) % BUFFER_SIZE;

        // If the buffer is full, move the tail pointer
        if (head == tail) {
            tail = (tail + 1) % BUFFER_SIZE;
        }
    }

    public Object[] getBufferArray() {
        Object[] bufferArray = new Object[BUFFER_SIZE];
        int index = (head - 1 + BUFFER_SIZE) % BUFFER_SIZE; // Start from the most recently added item

        for (int i = 0; i < BUFFER_SIZE; i++) {
            bufferArray[i] = circularBuffer[index];
            index = (index - 1 + BUFFER_SIZE) % BUFFER_SIZE;
        }

        return bufferArray;
    }

    public static void main(String[] args) {
        CircularBuffer<Integer> intBuffer = new CircularBuffer<>(5);
        intBuffer.addToBuffer(1);
        intBuffer.addToBuffer(2);
        intBuffer.addToBuffer(3);
        intBuffer.addToBuffer(4);
        intBuffer.addToBuffer(5);
        intBuffer.addToBuffer(6);
        intBuffer.addToBuffer(6);

        intBuffer.addToBuffer(6);

        intBuffer.addToBuffer(6);

        Object[] intArray = intBuffer.getBufferArray();
        for (Object value : intArray) {
            System.out.print(value + " ");
        }
        System.out.println();

        CircularBuffer<String> stringBuffer = new CircularBuffer<>(3);
        stringBuffer.addToBuffer("One");
        stringBuffer.addToBuffer("Two");
        stringBuffer.addToBuffer("Three");

        Object[] stringArray = stringBuffer.getBufferArray();
        for (Object value : stringArray) {
            System.out.print(value + " ");
        }
        System.out.println();
    }
}
