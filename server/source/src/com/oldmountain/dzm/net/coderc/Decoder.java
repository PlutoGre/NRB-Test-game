package com.oldmountain.dzm.net.coderc;

import java.io.IOException;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolDecoderAdapter;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
 
import com.oldmountain.dzm.net.packet.Packet;

/**
 * 解析客户端网络包 客户端上传数据包需要保证相应规则 <br>
 * 1、 short(16) 包头识别id <br>
 * 2、 int(32) opcode <br>
 * 3、 int(32) 数据包长度<br>
 * 4、 buffLen<br>
 * 
 * 
 * @author miaoshengli
 */
public final class Decoder extends ProtocolDecoderAdapter {
	public static final int packetHeadSize = 10;

	private static final String BUFFER = ".UABuffer";
	private static final ByteBuffer EMPTY = ByteBuffer.allocate(0);

	@Override
	public void decode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception {
		
		boolean useSessionBuffer = false;
		ByteBuffer buf = (ByteBuffer)session.getAttribute(BUFFER);
		if(buf!=null){
			buf.put(in);
			buf.flip();
			useSessionBuffer = true;
		}else{
			buf = in;
		} 
		for(;;){
			if(buf.remaining()>packetHeadSize){
				int pos = buf.position();
				short id = buf.getShort();
				if (id == Packet.PACKET_HEAD)// 
				{
					int opCode = buf.getInt();
					System.out.println("opCode: " + opCode);
					int buffLen = buf.getInt();
					System.out.println("get len: " + buffLen);
					if(buf.remaining()>=buffLen){    
						ByteBuffer data = EMPTY;
						byte[] bytes = new byte[buffLen];
						buf.get(bytes);
						if (opCode != 995) {
							for(int i=0;i<bytes.length;++i){
								bytes[i]=(byte) (bytes[i] ^ 0xA5);
							}
						}
						
						data = ByteBuffer.wrap(bytes); 
						out.write(new Packet(data, opCode, session));
					}else{
						buf.position(pos);
						break;
					}
				}else{
					session.setAttribute(BUFFER,null);
					throw new IOException("UA head error.");
				}
			}else{
				break;
			}
		}
		if (buf.hasRemaining()) {
				storeRemainingInSession(buf,session);
		}else{
			if(useSessionBuffer){
				session.setAttribute(BUFFER,null);
			}
		}
	}
	
    private void storeRemainingInSession(ByteBuffer buf, IoSession session) {
        ByteBuffer remainingBuf = ByteBuffer.allocate(buf.capacity());
        remainingBuf.setAutoExpand(true);
        remainingBuf.order(buf.order());
        remainingBuf.put(buf);
        session.setAttribute(BUFFER, remainingBuf);
    }

}
