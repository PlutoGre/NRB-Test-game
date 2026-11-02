package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.PieceSkillBook.*;
import com.oldmountain.dzm.services.SkillBookServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.PieceSkillBook.OPPieceSkillBook;
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
public class PieceSkillBookHandler
{
	private static final Logger logger = LoggerFactory.getLogger("SkillBookInfo");

    /**
     * 拼合残章
     * @param packet
     * @param session
     */
	public static void pieceSkillBook(Packet packet, IoSession session)
    {
        OPPieceSkillBook skillBookInfo = null;
		try
        {
			skillBookInfo = OPPieceSkillBook.parseFrom(packet.data.array());
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
        logger.info("pieceSkillBook handler received: {}, {}, {}", new Object[]{id, version, skillId});
        long startTime = Util.getServerMillTime();
        OPPieceSkillBookRet.Builder builder = OPPieceSkillBookRet.newBuilder();

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
            SkillBook _skillBook=player.getSkillBooks().get(skillId);

            //获取技能拼合残章列表
            ArrayList<Integer> _list= XMLTemplateService.getSkillBookListBySkillId(skillId);
            boolean canPiece=true;
            for(int _partId:_list)
            {
                int count=_skillBook.getSkillBookCountBySkillBookId(_partId);
                if(count<=0)
                {
                    canPiece=false;
                    break;
                }
                OPGetSkillBookInfoRet.SkillBookPartItem.Builder partItem=OPGetSkillBookInfoRet.SkillBookPartItem.newBuilder();
                partItem.setPartId(_partId);
                partItem.setSkillId(skillId);
                partItem.setCount(count);
                builder.addSkillBookPartItem(partItem);
            }
            if(!canPiece)
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_GET_SKILL_BOOK_INFORET_S_VALUE);
                eBuilder.setErrormsg("PlayerId:"+id+" Current Skill Part Not Exists!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                return;
            }
            else
            {
                //移除不持有残章的此玩家id,并组装给前端proto的协议
                //标示是否移除玩家拼合残章列表
                boolean isNeedRemove=true;
                int maxPartCount=0;
                for(int _partId:_list)
                {
                    _skillBook.changePartCountByBookId(_partId, -1);
                    int count=_skillBook.getSkillBookCountBySkillBookId(_partId);
                    if(count<=0)
                    {
                        //若不持有该残章，从可被查询列表移除
                        SkillBookServices.removePlayerIdFromMap(player.getId(),_partId);
                    }
                    else
                    {
                        maxPartCount=count>maxPartCount?count:maxPartCount;
                        if(maxPartCount>0)
                        {
                            isNeedRemove=false;
                        }
                    }
//                    OPGetSkillBookInfoRet.SkillBookPartItem.Builder partItem=OPGetSkillBookInfoRet.SkillBookPartItem.newBuilder();
//                    partItem.setPartId(_partId);
//                    partItem.setSkillId(skillId);
//                    partItem.setCount(count);
//                    builder.addSkillBookPartItem(partItem);
                }
                //消减拼合机会次数
                _skillBook.changePieceChanceByCount(-1);
                //没有拼合机会
                if(_skillBook.getPieceChance()<=0)
                {
                    //需要移除
                    if(isNeedRemove)
                    {
                        SkillBookServices.deleteSkillBook(player,skillId);
                    }
                    else
                    {//当前扔拥有残章，与拼合次数符合->以最大残章数为准
                        _skillBook.setPieceChance(maxPartCount);
                        player.updateOneSkillBook(_skillBook);
                    }
                }
                else
                {
                    player.updateOneSkillBook(_skillBook);
                }
                builder.addSkillInfo(SkillServices.createSkillAlone(player, skillId));
            }
        }
        
        UserStats stats = player.getUserStats();
        stats.addPieceskilltimes(1);
        player.setUserStats(stats);
        
        TitleUtil.checkNewTitle(player, 4, stats.getPieceskilltimes());
        
        builder.setServertime((int) System.currentTimeMillis());
        builder.setStatus(1);
        builder.setSkillId(skillId);
		player.sendPacket(OP.code.OPCODE_PIECE_SKILL_BOOKRET_S_VALUE, builder);// 发送返回包
		long endTime = Util.getServerMillTime();
		long costTime=endTime-startTime;
		logger.info("pieceSkillBook handler ret send: {}, Cost:{}, {}", new Object[]{id, costTime});
	}

}
