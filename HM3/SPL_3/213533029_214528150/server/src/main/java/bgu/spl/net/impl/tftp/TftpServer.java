package bgu.spl.net.impl.tftp;

import bgu.spl.net.srv.Server;

public class TftpServer {
    //TODO: Implement this
    public static void main(String[] args) {
        // if(args.length != 1){
        //     System.out.println("Invalid port");
        //     return;
        // }
        //you can use any server... 
        // this is if we want server args Integer.parseInt(args[0])
        Server.threadPerClient(
                7777, //port
                () -> new TftpProtocol(), //protocol factory
                TftpEncoderDecoder::new //message encoder decoder factory
        ).serve();
    }
}
