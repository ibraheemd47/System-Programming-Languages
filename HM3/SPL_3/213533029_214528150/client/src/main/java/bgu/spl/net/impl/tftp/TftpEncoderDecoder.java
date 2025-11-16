package bgu.spl.net.impl.tftp;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;

public class TftpEncoderDecoder implements MessageEncoderDecoder<byte[]> {
    private byte[] bytes = new byte[1<<10];
    private int len = 0 ;
    private short op = -1 ;
    
        private short bytesToShort(byte[] byteArr){
            short result = (short) ((byteArr[0] & 0xff) << 8);
            result += (short) (byteArr[1] & 0xff);
            return result ;
        }
        
        private byte[] popBytes(){
            byte[] temp = new byte[len];
            System.arraycopy(bytes, 0, temp, 0, len);
            len = 0 ;
            bytes = new byte[1 << 10];
            op = -1 ;
            return temp;
        }
    
        @Override
        public byte[] decodeNextByte(byte nextByte) {
            bytes[len] = nextByte;
            len++ ;
            if(len == 2 && op == -1){
                op =bytesToShort(bytes);
            }
            if(op == 6 || op == 10){
                return popBytes();
            } else if(op==4 &&  len == 4){
                return popBytes();
            } else if(op == 3 & len >= 6){
                short result = (short) ((bytes[2] & 0xff) << 8);
                result += (short) (bytes[3] & 0xff);
                if(len == result + 6){
                    return popBytes();
                }
            } else if((op == 1 || op == 2 || op == 5 || op == 7 || op == 8 || op == 9) && nextByte == '\0'){
                return popBytes();
            }
            return null;
        }
    
        @Override
        public byte[] encode(byte[] message) {
            //TODO: implement this
            return message;
        }
}
