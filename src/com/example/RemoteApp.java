package com.example;

import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class RemoteApp extends JFrame {

    private double lastX, lastY;

    private List<RemoteDevice> bluetoothDevices;
    private DefaultComboBoxModel<String> deviceNames;
    static JComboBox<String> deviceComboBox;
    static JLabel status;
    static Bluetooth bluetooth;
    static Set<Integer> pressedKeys = new HashSet<>();
    static CircularBuffer<Character> keysBuffer = new CircularBuffer(10);

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new RemoteApp().setVisible(true);
            KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(new KeyEventDispatcher() {

                @Override
                public boolean dispatchKeyEvent(KeyEvent e) {
                    if (bluetooth.isConnected()) {
                        try {
                            if (e.getID() == KeyEvent.KEY_PRESSED) {
                                if (!pressedKeys.contains(e.getKeyCode())) {
                                    pressedKeys.add(e.getKeyCode());
                                    bluetooth.send((byte) e.getKeyCode());
                                    bluetooth.send("P\n");
                                    keysBuffer.addToBuffer(e.getKeyChar());
                                    for (Object key : keysBuffer.getBufferArray()) {
                                        System.out.print(key==null?"":(char) key + " ");
                                    }
                                    System.out.println();
                                }
                            } else if (e.getID() == KeyEvent.KEY_RELEASED) {
                                pressedKeys.remove(e.getKeyCode());
                                bluetooth.send((byte) e.getKeyCode());
                                bluetooth.send("R\n");
                            }
                        } catch (IOException e1) {
                            e1.printStackTrace();
                        }
                    }
                    return true;
                }

            });

        });
    }

    public RemoteApp() {
        setTitle("Remote App");
        setSize(800, 300);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        Canvas canvas = new Canvas();
        canvas.setPreferredSize(new Dimension(600, 200));
        canvas.setBackground(Color.GRAY);
        canvas.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                lastX = evt.getX();
                lastY = evt.getY();
                System.out.println(lastX);
            }
        });
        canvas.addMouseMotionListener(new java.awt.event.MouseMotionAdapter() {
            public void mouseDragged(java.awt.event.MouseEvent evt) {
                Graphics g = canvas.getGraphics();
                g.setColor(Color.BLACK);
                g.drawLine((int) lastX, (int) lastY, evt.getX(), evt.getY());
                lastX = evt.getX();
                lastY = evt.getY();
            }
        });

        JPanel contentPane = new JPanel(new BorderLayout(10, 10));

        JButton searchButton = new JButton("Search Devices");
        JButton connectButton = new JButton("Connect");
        JButton testSendButton = new JButton("Send");
        JButton testReadButton = new JButton("Read");

        deviceNames = new DefaultComboBoxModel<>();
        deviceComboBox = new JComboBox<>(deviceNames);
        status = new JLabel("Ready");
        status.setFont(new Font("Consolas", Font.PLAIN, 20));

        try {
            bluetooth = new Bluetooth();
            searchButton.addActionListener(e -> {
                status.setText("Searching...");
                ((JButton) e.getSource()).setEnabled(false);

                try {
                    bluetooth.searchDevices(devices -> {
                        deviceNames.removeAllElements();
                        deviceNames.addAll(devices);
                        // ((JButton) e.getSource()).setText("Search Devices");
                        status.setText(devices.size() + " found");

                        ((JButton) e.getSource()).setEnabled(true);

                    });
                } catch (BluetoothStateException e1) {
                    e1.printStackTrace();
                }

            });
            connectButton.addActionListener(e -> {
                if (((JButton) e.getSource()).getText() == "Disconnect") {
                    bluetooth.disconnect(message -> {
                        status.setText(message);
                        ((JButton) e.getSource()).setText("Connect");
                        ((JButton) e.getSource()).setEnabled(true);
                    }, message -> {
                        ((JButton) e.getSource()).setText("Disconnect");
                        ((JButton) e.getSource()).setEnabled(true);
                        status.setText(message);
                    });
                } else {
                    status.setText("Connecting...");
                    ((JButton) e.getSource()).setEnabled(false);
                    try {
                        bluetooth.connectToDevice(deviceComboBox.getSelectedIndex(), message -> {
                            ((JButton) e.getSource()).setText("Disconnect");
                            ((JButton) e.getSource()).setEnabled(true);
                            status.setText(message);

                        },
                                message -> {
                                    ((JButton) e.getSource()).setText("Connect");
                                    ((JButton) e.getSource()).setEnabled(true);
                                    status.setText(message);

                                });
                    } catch (BluetoothStateException e1) {
                        e1.printStackTrace();
                    }
                }
            });

            testSendButton.addActionListener(e -> {
                try {
                    bluetooth.send("ome test message\n");
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            });

            testReadButton.addActionListener(e -> {
                try {
                    status.setText("Read: " + bluetooth.readPacket());
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            });

            Thread readThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    String readOut = "";
                    while (true) {
                        try {
                            readOut = bluetooth.readPacket();
                        } catch (Exception e) {
                            
                        }
                        if (!readOut.equals(""))
                            status.setText("Read: " + readOut);
                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                    }
                }
            });
            readThread.start();

            contentPane.addKeyListener(new KeyListener() {

                @Override
                public void keyTyped(KeyEvent e) {
                    try {
                        bluetooth.send("" + e.getKeyChar());
                    } catch (IOException e1) {
                        e1.printStackTrace();
                    }
                }

                @Override
                public void keyPressed(KeyEvent e) {

                }

                @Override
                public void keyReleased(KeyEvent e) {

                }

            });

        } catch (BluetoothStateException e) {
            e.printStackTrace();
        }

        JPanel southPanel = new JPanel(new GridLayout(2, 1, 10, 10));
        JPanel bluetoothPanel = new JPanel(new GridLayout(1, 3, 10, 10));
        bluetoothPanel.add(searchButton);
        bluetoothPanel.add(deviceComboBox);
        bluetoothPanel.add(connectButton);
        southPanel.add(bluetoothPanel);
        southPanel.add(status);

        JPanel eastPanel = new JPanel(new GridLayout(10, 1, 10, 10));
        eastPanel.add(testSendButton);
        eastPanel.add(testReadButton);

        contentPane.add(canvas, BorderLayout.CENTER);
        contentPane.add(southPanel, BorderLayout.SOUTH);
        //contentPane.add(eastPanel, BorderLayout.EAST);

        setContentPane(contentPane);
    }
}
