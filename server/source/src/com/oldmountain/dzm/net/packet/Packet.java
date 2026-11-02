package com.oldmountain.dzm.net.packet;


import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;


public class Packet {
	public static final short PACKET_HEAD = 0x4346;
	
	public ByteBuffer data;
	public IoSession session;
	public int opcode;
	
	public Packet(ByteBuffer data, int opcode ,IoSession session) {
		this.data = data;
		this.opcode = opcode;
		this.session = session;
	}

	public Packet(int opcode, byte[] dataBytes) {
		this.data  = ByteBuffer.allocate(dataBytes.length);
		this.data.put(dataBytes); 
		this.opcode = opcode;
	}

	@Override
	public String toString() {
		return "method=" + opcode + "&msg=" + data.toString();
	}
	
}
