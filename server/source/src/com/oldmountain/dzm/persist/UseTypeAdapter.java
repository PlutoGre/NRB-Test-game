package com.oldmountain.dzm.persist;

public interface UseTypeAdapter {
	
	public void parse(byte[] bytes) throws Exception;
	
	public byte[] toDbData();
	
}
