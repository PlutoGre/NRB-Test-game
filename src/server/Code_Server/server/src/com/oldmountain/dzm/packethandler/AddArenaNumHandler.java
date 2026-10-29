package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.ToolsXmlTemplate;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.AddArenaNum;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashMap;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-19
 * Time: 下午5:19
 * To change this template use File | Settings | File Templates.
 */
public class AddArenaNumHandler {
    private static final Logger log = LoggerFactory.getLogger(LoginHandler.class);
    private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
    public static void addArenaNum(Packet packet,IoSession session){
        AddArenaNum.OPAddArenaNum params = null;
        AddArenaNum.OPAddArenaNumRet.Builder builder = AddArenaNum.OPAddArenaNumRet.newBuilder();

        try {
            params = AddArenaNum.OPAddArenaNum.parseFrom(packet.data.array());
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }

        int typeid = params.getTypeid();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("add arenaNum handler received: {}, {}, {}", new Object[]{id , typeid});
        logger2.info("add arenaNum handler received: {}, {}, {}", new Object[]{id , typeid});
        long current_time = Util.getServerMillTime();
        int time = Util.getServerTime();
        switch (typeid){
            case 1:
                ToolsXmlTemplate tm= XMLTemplateService.getToolTemplate(UserToolConst.TIAOZHANSHU);
                if(player.getGoldCoins()>=tm.getPrice())
                {

                    UserStats stats = player.getUserStats();
                    if(stats.getChallengetimes()+3<=player.getViplevel()+4)
                    {
                        player.consumeGold(tm.getPrice());
                        stats.setChallengetimes(stats.getChallengetimes()+3);
                        player.setUserStats(stats);
                        builder.setStatus(301);
                        builder.setServertime(time);
                        builder.setArenanum(stats.getChallengetimes());
                    }
                    else
                    {
                        ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                        eBuilder.setErrorid(OP.code.OPCODE_ADD_ARENA_NUMRET_S_VALUE);
                        eBuilder.setErrormsg("ARENANUM IS MORE THAN LIMIT NUM!");
                        player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                        logger2.info("ARENANUM IS MORE THAN LIMIT NUM {}, {}, {}", new Object[]{player.getId()});
                    }

                }
                else
                {
                    // 元宝数量不够
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_ADD_ARENA_NUMRET_S_VALUE);
                    eBuilder.setErrormsg("GOLD NOT ENOUGH!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Add ArenaNum Gold Not Enough {}, {}, {}", new Object[]{player.getId()});
                }
                break;
            default:
                break;

        }
        
        player.setLastVerifyTime(time);
        
        player.sendPacket(OP.code.OPCODE_ADD_ARENA_NUMRET_S_VALUE, builder);
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
        log.info("add ArenaNum handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
        logger2.info("add ArenaNum handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    }
}
