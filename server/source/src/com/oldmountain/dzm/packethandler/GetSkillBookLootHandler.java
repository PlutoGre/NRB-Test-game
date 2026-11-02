package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.services.SkillBookServices;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.GetSkillBookLoot.OPSkillBookLoot;
import com.oldmountain.dzm.message.GetSkillBookLoot.OPSkillBookLootRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

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
public class GetSkillBookLootHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("SkillBookInfo");
	public static void getSkillBookLoot(Packet packet, IoSession session)
    {
        OPSkillBookLoot opSkillBookLoot = null;
		try
        {
			opSkillBookLoot = OPSkillBookLoot.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opSkillBookLoot.getVersion();//version default = 1;
		int skillBookId = opSkillBookLoot.getSkillBookId();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger2.info("get skill book loot kandler received: {}, {}, {}", new Object[]{id , version , skillBookId});
        long current_time = Util.getServerMillTime();
        player.getSkillBookNpcStats().setLastSkillBookId(skillBookId);
        OPSkillBookLootRet.Builder builder = SkillBookServices.getSkillBookLootBySkillBookId(player, skillBookId);
        builder.setServertime((int) System.currentTimeMillis());
        builder.setStatus(1);
		player.sendPacket(OP.code.OPCODE_GET_SKILL_BOOK_LOOT_INFORET_S_VALUE, builder);// 发送返回包
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
		logger2.info("get skill book loot kandler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}
