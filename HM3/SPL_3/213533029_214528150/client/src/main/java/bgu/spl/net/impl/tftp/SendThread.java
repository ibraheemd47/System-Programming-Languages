package bgu.spl.net.impl.tftp;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.Buffer;
import java.util.Scanner;
import java.util.concurrent.BlockingQueue;

public class SendThread implements Runnable {
    BufferedWriter out ;
    TftpEncoderDecoder EncDec;
    TftpProtocol protocol;
    BlockingQueue<Boolean> messages;
    @Override
    public void run() {
        // TODO Auto-generated method stub
        Scanner scanner = new Scanner(System.in) ;
        while(!protocol.shouldTerminate()){
            String Line = scanner.nextLine() ;
            byte[] bytes = EncDec.encode(Line);
            try{
                out.write(bytes);
                out.flush();
            }catch(IOException ignored){}
            // while(sending data packets){
            //     create packet
            //     send packet
                messages.take();//i am waiting
            // }
            
            messages.put(true);// i am telling the second thread to wake up
        }
    }
    void start(BufferedWriter out,TftpProtocol protocol,BlockingQueue<Boolean> messages){
        this.out = out ;
        this.protocol =protocol ;
        this.messages = messages ;
    }
}
