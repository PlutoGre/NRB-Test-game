package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.AddTeachExp;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

import org.apache.mina.common.IoSession;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-3-22
 * Time: 上午11:31
 * To change this template use File | Settings | File Templates.
 */
public class AddAdventureTeachExpHandler {
    private static final Logger log = LoggerFactory.getLogger(AddAdventureTeachExpHandler.class);
    private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
    public static void addExp(Packet packet, IoSession session){

        AddTeachExp.OPAddTeachExp params = null;
        AddTeachExp.OPAddTeachExpRet.Builder builder = AddTeachExp.OPAddTeachExpRet.newBuilder();
        try {
            params = AddTeachExp.OPAddTeachExp.parseFrom(packet.data.array());
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
            return;
        }
        int version = params.getVersion();//version default = 1;
        int type = params.getType();
        int adventureId=params.getAdventureId();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("add adventure teach exp handler received: {}, {}, {}", new Object[]{ id , version , type , adventureId});
        logger2.info("add adventure teach exp handler received: {}, {}, {}", new Object[]{ id , version , type , adventureId});
        long current_time = Util.getServerMillTime();
        switch (type) {
            case 1:  //普通获取经验
                break;
            case 2: //获取双倍经验
                builder.setStatus(1);
                break;
            default:
                break;
        }
        builder.setStatus(1);
        HashMap<Integer, Disciple> disciples = player.getDisciples();
        AddTeachExp.OPAddTeachExpRet.DiscipleExpInfo.Builder builder2 = AddTeachExp.OPAddTeachExpRet.DiscipleExpInfo.newBuilder();
        Iterator<Map.Entry<Integer, Disciple>> iterator = disciples.entrySet().iterator();
        int index = 0;
        while (iterator.hasNext()) {
            index++;
            if(index>8)
            {
                break;
            }
            Map.Entry<Integer, Disciple> entry = iterator.next();
            Disciple value = entry.getValue();

            builder2.setId(index);
            builder2.setLevel(value.getLevel()+(index%2));
            builder2.setExp(value.getExp()+2500*((index%2)+1));
            builder2.setDiscipleid(value.getId());
            builder2.setExpadd(2500);
            builder2.setExtraexpadd(2500*(index%2));

            builder.addExpinfo(builder2);
        }
        player.setLastVerifyTime(Util.getServerTime());
        player.sendPacket(OP.code.OPCODE_ADD_ADVENTURE_TEACHEXPRET_S_VALUE, builder);// 发送包内容
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
        log.info("add adventure teach exp handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
        logger2.info("add adventure teach exp handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    }
}
