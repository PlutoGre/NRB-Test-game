package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.SkillBookInfo.OPGetSkillBookInfo;
import com.oldmountain.dzm.message.SkillBookInfo.OPGetSkillBookInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

import java.util.ArrayList;

/**
 * 留言包
 * 1， 解析数据包
 * 2， 保存留言数据
 * 3， 推送给相关玩家数据信息
 * 4， 下发留言成功
 * 
 *
 * @author lvjc
 */
public class SkillBookInfoHandler
{
	private static final Logger logger = LoggerFactory.getLogger("SkillBookInfo");

    /**
     * 获取残章的信息
     * @param packet
     * @param session
     */
	public static void getSkillBookInfo(Packet packet, IoSession session)
    {
        OPGetSkillBookInfo skillBookInfo = null;
		try
        {
			skillBookInfo = OPGetSkillBookInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = skillBookInfo.getVersion();//version default = 1;
		int skillId = skillBookInfo.getSkillId();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger.info("getSkillBookInfo received: {}, {}, {}", new Object[]{id, version});
        long startTime = Util.getServerMillTime();
        OPGetSkillBookInfoRet.Builder builder = OPGetSkillBookInfoRet.newBuilder();
        //check 玩家是否能拼合次残章
        if(!player.getSkillBooks().containsKey(skillId))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_SKILL_BOOK_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+id+" Current Skill Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        else
        {
            //获取技能拼合残章列表
            ArrayList<Integer> _list= XMLTemplateService.getSkillBookListBySkillId(skillId);
            SkillBook _skillBook=player.getSkillBooks().get(skillId);
            for(int _partId:_list)
            {
                int count=_skillBook.getSkillBookCountBySkillBookId(_partId);
                OPGetSkillBookInfoRet.SkillBookPartItem.Builder partItem=OPGetSkillBookInfoRet.SkillBookPartItem.newBuilder();
                partItem.setPartId(_partId);
                partItem.setCount(count);
                builder.addSkillBookPartItem(partItem);
            }
        }
        builder.setServertime((int) System.currentTimeMillis());
        builder.setStatus(1);
        builder.setSkillId(skillId);
		player.sendPacket(OP.code.OPCODE_GET_SKILL_BOOK_INFORET_S_VALUE, builder);// 发送返回包
		long endTime = Util.getServerMillTime();
		long taken_time=endTime-startTime;
		logger.info("getSkillBookInfo ret send: {}, Cost:{}, {}", new Object[]{id, taken_time});
	}

}
