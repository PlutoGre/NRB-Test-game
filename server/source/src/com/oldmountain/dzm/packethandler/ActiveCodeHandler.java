package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.ActiveCode;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.*;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.List;

/**
 * 激活码
 * 状态码status:
 * 			0 : 激活码错误
 * 			1 : 成功
 * 			2 : 激活码已使用
 * */
public class ActiveCodeHandler {
	private static final Logger log = LoggerFactory.getLogger(ActiveCodeHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("ActiveCode");
	public static void validateActiveCode(Packet packet, IoSession session) {


		ActiveCode.OPActiveCode params = null;
		ActiveCode.OPActiveCodeRet.Builder builder = ActiveCode.OPActiveCodeRet.newBuilder();
		try{
			params = ActiveCode.OPActiveCode.parseFrom(packet.data.array());
		}catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		String activeCode = params.getActivecode().toUpperCase().trim();
        String puid = params.getUid().toLowerCase().trim();
		String typeId="";
        int status=1;	//使用结果状态码
        boolean aCode_form = true;	//cdkey格式
        ActiveCodeEntity aCode_entity=null;
        log.info("activeCode  handler received: {}, {}, {}", new Object[]{ puid , activeCode});
        logger2.info("activeCode reward handler received: {}, {}, {}", new Object[]{ puid  , activeCode});
        long current_time = Util.getServerMillTime();
    /********************************************************************************************************************/  
        //激活码格式判断
        if(activeCode.length()!=14)
        {
            aCode_form=false;
        	status=0;
        }
        else 
        {
        	char[] chs=activeCode.toCharArray();
        	if(Character.isLetter(chs[0]) && Character.isLetter(chs[1]) && Character.isDigit(chs[12]) && Character.isDigit(chs[13])){
        		int num_count=0;
        		int abc_count=0;
        		for(int i=2 ; i<=11 ; i++){
        			if(Character.isDigit(chs[i])){
        				num_count++;
        			}
        			else if(Character.isLetter(chs[i])){
        				abc_count++;
        			}
        		}
        		if(num_count != 4 || abc_count != 6){
                    aCode_form=false;
        			status=0;
        		}
        	}
        	else {
                aCode_form=false;
				status=0;
			}
        }
        if(status==1){

            List<Object> list = DZM.getEntityManager().limitQuery("from ActiveCodeEntity where active_code = ?", 0, 1, activeCode);
            if (list.size() == 1)
            {
                aCode_entity=(ActiveCodeEntity) list.get(0);
                //cdkey已使用
                if(aCode_entity.getStatus()==1)
                {
                    status=2;
                }
            }else {
                //数据库中不存在
                status=0;
            }
	        if(status==1){
	        	//数据库中设置状态为已使用
                aCode_entity.setUsed_time(Util.getServerTime());
                aCode_entity.setPlayerid(puid);
                aCode_entity.setStatus(1);
	        	//同步更新数据库
	        	DZM.getEntityManager().updateSync(aCode_entity);
	        }
        }
        builder.setStatus(status);
        Packet pt = new Packet(OP.code.OPCODE_ACTIVE_CODERET_S_VALUE, builder.build().toByteArray());
        session.write(pt);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("activeCode handler ret send: {}, {}, {}", new Object[]{puid ,taken_time});
		logger2.info("activeCode handler ret send: {}, {}, {}", new Object[]{puid ,taken_time});
	}

}
