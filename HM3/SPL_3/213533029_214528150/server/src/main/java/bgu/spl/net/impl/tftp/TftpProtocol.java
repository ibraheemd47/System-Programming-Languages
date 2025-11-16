package bgu.spl.net.impl.tftp;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.api.BidiMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class TftpProtocol implements BidiMessagingProtocol<byte[]> {
    private boolean isLoggedIn;
    private int connectionId;
    private Connections<byte[]> connections;
    static private ConcurrentHashMap<Integer, Boolean> Logged = new ConcurrentHashMap<>();
    private List<byte[]> dataPackets;
    private int thisBlockNumberRRQ;
    private int thisBlockNumberWRQ;
    private boolean RRQNow;
    private String fileRRQ ;
    private File thisFile;
    private boolean terminate = false;

    @Override
    public void start(int connectionId, Connections<byte[]> connections) {
        // TODO implement this
        this.isLoggedIn = false;
        this.connectionId = connectionId;
        this.connections = connections;
        this.dataPackets = new LinkedList<>();
        thisBlockNumberRRQ = 1;
        
        thisBlockNumberWRQ = 1;

    }

    private short bytesToShort(byte[] byteArr) {
        short result = (short) ((byteArr[0]) << 8 | (short) (byteArr[1]));

        return result;
    }

    private byte[] shortToBytes(short num) {
        byte[] bytesArray = new byte[2];
        bytesArray[0] = (byte) ((num >> 8) & 0xff);
        bytesArray[1] = (byte) (num & 0xff);
        return bytesArray;
    }

    @Override
    public void process(byte[] message) {
        // TODO implement this
        byte[] toReturn;
        short opcode = bytesToShort(message);
        if (isntValidOp(opcode)) {
            String errorMessage = "Illegal TFTP operation";
            connections.send(connectionId, createErrorPacket((short) 5, (short) 4, errorMessage.getBytes()));
        }
        if (opcode == 1) { // RRQ
            // if there is a logged in client if not send an error
            thisBlockNumberRRQ = 1 ;
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }
            String fileName = new String(message, 2, message.length - 3, StandardCharsets.UTF_8);
            File currFF = new File("./Flies/" + fileName);
            fileRRQ = fileName ;
            if (!currFF.exists()) {
                String errorMessage = "File not found";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 1, errorMessage.getBytes()));
            } else {

                try (FileInputStream myFile = new FileInputStream("./Flies/" + fileName)) {
                    byte[] buffer = new byte[512];
                    int bytesRead1 = -5;
                    int blockNum = 0;
                    while ((bytesRead1 = myFile.read(buffer)) != -1) {
                        dataPackets.add(blockNum, buffer.clone());
                        blockNum++;
                        buffer = new byte[512];
                    }
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }

                toReturn = createPacket((short) 3, (short) dataPackets.get(0).length, (short) 1, dataPackets.remove(0));
                thisBlockNumberRRQ++;
                RRQNow = true;
                connections.send(connectionId, toReturn);

            }

        } else if (opcode == 2) { // WRQ
            thisBlockNumberWRQ = 1;
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }
            // if there is a logged in client if not send an error
            String fileName = new String(message, 2, message.length - 3, StandardCharsets.UTF_8);

            File f = new File("./Flies/" + fileName);
            if (f.exists()) {
                String errorMessage = "File already exists";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 5, errorMessage.getBytes()));
            } else {
                try {
                    f.createNewFile();

                    thisFile = f;
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                connections.send(connectionId, createPacket((short) 4, (short) 0, (short) 0, null));
            }
        } else if (opcode == 3) { // data
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }
            short result = (short) ((message[2] & 0xff) << 8);
            result += (short) (message[3] & 0xff);
            byte[] data = new byte[result];
            System.arraycopy(message, 6, data, 0, data.length);
            try {
                FileOutputStream writeToFile = new FileOutputStream(thisFile);

                try {
                    writeToFile.write(data);

                    if (result < 512) {

                        writeToFile.close();
                    }
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            } catch (FileNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            toReturn = createPacket((short) 4, (short) 0, (short) thisBlockNumberWRQ, null);

            connections.send(connectionId, toReturn);
            if (result < 512) {
                String completemsgWRQ = "WRQ " + thisFile.getName() + " complete";
                byte[] comp = new byte[completemsgWRQ.length() + 1];
                System.arraycopy(completemsgWRQ.getBytes(), 0, comp, 0, completemsgWRQ.getBytes().length);
                connections.send(connectionId,
                        createPacket((short) 3, (short) comp.length, (short) thisBlockNumberWRQ, comp));
                sendBcast((short) 1, thisFile.getName());
            }
            thisBlockNumberWRQ++;
        } else if (opcode == 4) { // ACK
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }
            if (RRQNow) {
                if (dataPackets.size() > 0) {

                    toReturn = createPacket((short) 3, (short) dataPackets.get(0).length, (short) thisBlockNumberRRQ,
                            dataPackets.remove(0));
                    thisBlockNumberRRQ++;
                    connections.send(connectionId, toReturn);
                } else {
                    String completemsgWRQ = "RRQ " + fileRRQ + " complete";
                    byte[] comp = new byte[completemsgWRQ.length() + 1];
                    System.arraycopy(completemsgWRQ.getBytes(), 0, comp, 0, completemsgWRQ.getBytes().length);
                    connections.send(connectionId,
                            createPacket((short) 3, (short) comp.length, (short) thisBlockNumberRRQ, comp));
                    RRQNow = false;
                }
            }

        } else if (opcode == 6) { // DIRQ
            // you need to handle it again because it's returning the name in a wrong way
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }

            File folder = new File("./Flies/");
            File[] filesList = folder.listFiles();

            String allNames = "";
            for (int j = 0; j < filesList.length - 1; j++) {
                allNames = allNames + filesList[j].getName() + '0';
            }
            allNames = allNames + filesList[filesList.length - 1];
            byte[] toSend = new byte[allNames.getBytes().length + 1];
            System.arraycopy(allNames.getBytes(), 0, toSend, 0, allNames.getBytes().length);
            connections.send(connectionId, createPacket((short) 3, (short) toSend.length, (short) 1, toSend));

        } else if (opcode == 7) { // LOGRQ
            isLoggedIn = true;

            if (Logged.contains(connectionId) && Logged.get(connectionId) == true) {
                // error
                String errorMessage = "User already logged in - Login username already connected";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 7, errorMessage.getBytes()));

            } else {
                Logged.put(connectionId, isLoggedIn);
                connections.send(connectionId, createPacket((short) 4, (short) 0, (short) 0, null));

            }
        } else if (opcode == 8) { // DELRQ
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }
            String fileName = new String(message, 2, message.length - 3, StandardCharsets.UTF_8);
            File currFile = new File("./Flies/" + fileName);
            if (currFile.delete()) {
                connections.send(connectionId, createPacket((short) 4, (short) 0, (short) 0, null));
                sendBcast((short) 0, fileName);

            } else {
                String errorMessage = "DELRQ of non-existing file";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 1, errorMessage.getBytes()));
            }

        } else if (opcode == 10) { // DISC
            if (!isLoggedIn) {
                String errorMessage = "User not Logged in";
                connections.send(connectionId, createErrorPacket((short) 5, (short) 6, errorMessage.getBytes()));
            }
            if (Logged.replace(connectionId, false) != null) {
                terminate = true;
                connections.send(connectionId, createPacket((short) 4, (short) 0, (short) 0, null));
            } else {
                String errorMessage = "Not defined";

                connections.send(connectionId, createErrorPacket((short) 5, (short) 1, errorMessage.getBytes()));
            }
        }
    }

    @Override
    public boolean shouldTerminate() {
        // TODO implement this
        return terminate;
    }

    private boolean isntValidOp(short opcode) {
        return opcode != 1 & opcode != 2 & opcode != 3 & opcode != 4 & opcode != 5 & opcode != 6 & opcode != 7
                & opcode != 8 & opcode != 9 & opcode != 10;
    }

    private void sendBcast(short added, String fileName) {
        byte[] toReturn = createPacket((short) 9, (short) 0, added, fileName.getBytes());
        for (Integer connectionId : Logged.keySet()) {
            if (Logged.get(connectionId)) {
                connections.send(connectionId, toReturn);
            }
        }

    }

    private byte[] createPacket(short opcode, short packetSize, short blockNum, byte[] data) {
        byte[] res;
        if (data == null) {
            res = new byte[6];
        }
        if (data == null && packetSize == 0) {
            res = new byte[4];
            res[0] = shortToBytes(opcode)[0];
            res[1] = shortToBytes(opcode)[1];
            res[2] = shortToBytes(blockNum)[0];
            res[3] = shortToBytes(blockNum)[1];
            return res;
        }
        if (data != null && packetSize == 0) {
            res = new byte[4 + data.length];
            res[0] = shortToBytes(opcode)[0];
            res[1] = shortToBytes(opcode)[1];
            res[2] = shortToBytes(blockNum)[0];
            res[3] = shortToBytes(blockNum)[1];
            System.arraycopy(data, 0, res, 4, data.length);
            return res;
        } else {
            res = new byte[6 + data.length];
        }

        res[0] = shortToBytes(opcode)[0];
        res[1] = shortToBytes(opcode)[1];
        res[2] = shortToBytes(packetSize)[0];
        res[3] = shortToBytes(packetSize)[1];
        res[4] = shortToBytes(blockNum)[0];
        res[5] = shortToBytes(blockNum)[1];

        if (data != null) {
            System.arraycopy(data, 0, res, 6, data.length);
        }
        return res;
    }

    private byte[] createErrorPacket(short opcode, short errorCode, byte[] errorMessage) {
        byte[] res;
        if (errorMessage == null) {
            res = new byte[4];
        } else {
            res = new byte[4 + errorMessage.length + 1];
        }
        res[0] = shortToBytes(opcode)[0];
        res[1] = shortToBytes(opcode)[1];
        res[2] = shortToBytes(errorCode)[0];
        res[3] = shortToBytes(errorCode)[1];

        if (errorMessage != null) {
            System.arraycopy(errorMessage, 0, res, 4, errorMessage.length);
        }

        return res;
    }
}
