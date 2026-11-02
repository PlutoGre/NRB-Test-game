package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.AdventureTeacherInfo;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

/*
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-3-20
 * Time: 下午4:33
 * To change this template use File | Settings | File Templates.
 */
public class GetAdventureTeacherInfoHandler {
    private static final Logger log = LoggerFactory.getLogger(GetAdventurePowerInfoHandler.class);
    private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
    public static void getInfo(Packet packet, IoSession session)
    {
        AdventureTeacherInfo.OPGetTeacherInfo opGetTeacherInfo = null;

        try
        {
            opGetTeacherInfo = AdventureTeacherInfo.OPGetTeacherInfo.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version = opGetTeacherInfo.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("get adventure teacher info handler received: {}, {}, {}", new Object[]{id , version});
        logger2.info("get adventure teacher info handler received: {}, {}, {}", new Object[]{id , version});
        long current_time = Util.getServerMillTime();
        AdventureTeacherInfo.OPGetTeacherInfoRet.Builder builder = AdventureTeacherInfo.OPGetTeacherInfoRet.newBuilder();
        builder.setStatus(1);
        builder.setServertime((int) System.currentTimeMillis());
        builder.setEndtime((int) System.currentTimeMillis()+3);
        builder.setAdventureitemid(opGetTeacherInfo.getAdventureitemid());
        builder.setTeacherId(1);
        builder.setExp(1000);
        player.sendPacket(OP.code.OPCODE_GET_ADVENTURE_TEACHERINFORET_S_VALUE, builder);// 发送返回包
        player.setLastVerifyTime(Util.getServerTime());
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
        log.info("get adventure teacher info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
        logger2.info("get adventure teacher info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    }

}
