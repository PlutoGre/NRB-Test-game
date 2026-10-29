package com.oldmountain.dzm.persist;

public class DataAccessException extends RuntimeException {
	private static final long serialVersionUID = 1L;

	public DataAccessException(Throwable cause) {
		super(cause);
	}
	
	public DataAccessException(String msg) {
		super(msg);
	}
}
