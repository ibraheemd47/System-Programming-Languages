package bgu.spl.net.impl.tftp;

import bgu.spl.net.api.MessagingProtocol;

public class TftpProtocol implements MessagingProtocol{
    private boolean shouldTerminate = false ;
    @Override
    public Object process(Object msg) {
        // TODO Auto-generated method stub
        //if opcode = the client request 
        throw new UnsupportedOperationException("Unimplemented method 'process'");
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate ;
    }

}
