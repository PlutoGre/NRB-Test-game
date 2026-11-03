package com.oldmountain.dzm.net.coderc;


import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolEncoderAdapter;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;

import com.oldmountain.dzm.net.packet.Packet;
/**
 * @brief 下发游戏网络包，按有效数据长度封装并加密。
 * 客户端解析需要注意：
 * 	1、 short(16) = 包头识别id  CodecFactory.PACKET_HEAD
 * 	2、 int(32)opcode
 * 	3、 int(32) buf长度
 * 	4、buf 数据体
 * 
 * @author miaoshengli
 */
public final class Encoder extends ProtocolEncoderAdapter {
	
	/**
	 * 按数据有效长度编码，池化缓冲区容量可能大于实际消息。
	 * @param session 当前游戏连接
	 * @param message 待发送的游戏包
	 * @param out 网络编码输出
	 * @throws Exception 编码或发送失败
	 */
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
			data.flip();
			
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
