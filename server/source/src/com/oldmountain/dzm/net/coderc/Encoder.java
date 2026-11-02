package com.oldmountain.dzm.net.coderc;


import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolEncoderAdapter;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;

import com.oldmountain.dzm.net.packet.Packet;
/**
 * 下发包
 * 客户端解析需要注意：
 * 	1、 short(16) = 包头识别id  CodecFactory.PACKET_HEAD
 * 	2、 int(32)opcode
 * 	3、 int(32) buf长度
 * 	4、buf 数据体
 * 
 * @author miaoshengli
 */
public final class Encoder extends ProtocolEncoderAdapter {
	
	@Override
	public void encode(IoSession session, Object message, ProtocolEncoderOutput out) throws Exception {
		if (message instanceof Packet) {
			Packet packet = (Packet) message;
			
			ByteBuffer datasrc = packet.data;
			int size = datasrc.limit();
			ByteBuffer data = ByteBuffer.allocate(size);
			
			datasrc.rewind();
			for(int i=0;i<size;++i)
			{
				byte info = (byte) (datasrc.get() ^ 0xA5);
				data.put(info);
			}
			data.rewind();
			
			int len = 10 + data.remaining();
			ByteBuffer buf = ByteBuffer.allocate(len);
			buf.putShort(Packet.PACKET_HEAD);
			buf.putInt(packet.opcode);
			buf.putInt(len);
			buf.put(data);
			buf.flip();
			out.write(buf);
		}
	}    
}
