package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.ToolsXmlTemplate;
import com.oldmountain.dzm.message.BuyTools.*;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.PayToolServices;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午11:06
 * To change this template use File | Settings | File Templates.
 */
public class BuyToolHandler
{    
	private static final Logger log = LoggerFactory.getLogger(BuyToolHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("MarketInfo");
	private static final Logger logger_goldCost = LoggerFactory.getLogger("GoldCost");
    public static void buyTool(Packet packet, IoSession session)
    {
        OPBuyTool opBuyTool = null;
        try
        {
            opBuyTool = OPBuyTool.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int itemId = opBuyTool.getItemid();
        int count = opBuyTool.getCount();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        if (count <= 0) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_BUY_TOOLSRET_S_VALUE);
			errBuilder.setErrormsg("count not right");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
			return;
		}
        int id=player.getId();
        log.info("buy tool handler received: {}, {}, {}", new Object[]{id , itemId , count});
        logger2.info("buy tool handler received: {}, {}, {}", new Object[]{id , itemId , count});
        long current_time = Util.getServerMillTime();
        ToolsXmlTemplate tm= XMLTemplateService.getToolTemplate(itemId);
        OPBuyToolRet.Builder builder = OPBuyToolRet.newBuilder();
        int buyStatus=1;
        boolean needRecordStats=false;

        if(tm!=null)
        {
            //vip等级不够
            if(tm.getVipLevel()!=-1&&player.getViplevel()<tm.getVipLevel())
            {
                buyStatus=4;
            }
            else
            {
            	//buytimes>0,有购买数量限制
                if(tm.getBuyTimes()>0)
                {
                    if(player.getUserStats().getBuyGiftsTimesById(itemId)>=tm.getBuyTimes())
                    {
                        buyStatus=3;
                    }
                    else
                    {
                        needRecordStats=true;
                    }
                }
                //buytimes=0,不能购买
                else if(tm.getBuyTimes()==0){
                	OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
        			errBuilder.setErrorid(OP.code.OPCODE_BUY_TOOLSRET_S_VALUE);
        			errBuilder.setErrormsg("tool can not be bought");
        			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
        			return;
                }
            }
            if(buyStatus==1)
            {
                //需要元宝数
                int needPrice=tm.getPrice()*count;
                //计算出的needPrice<0,不能购买
                if(needPrice<0){
                	OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
        			errBuilder.setErrorid(OP.code.OPCODE_BUY_TOOLSRET_S_VALUE);
        			errBuilder.setErrormsg("price less than 0");
        			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
        			return;
                }
                if(player.getGoldCoins()>=needPrice)
                {
                	int gold_before = player.getGoldCoins();
                    builder=PayToolServices.BuyTools(player, tm.getContainTools(), itemId, count, needPrice, needRecordStats);
                    buyStatus=1;
                    //添加宝箱运气池的运气值
                    if(UserToolConst.getType(itemId)==UserToolConst.TREASURE_TOOL)
                    {
                    	BagUtil.getInstance().addLuckPool(player,itemId,count);
                    }
                    //时间，用户ID，类型ID，个数，消费之前多少钱，消费之后多少钱，消费多少钱
                   logger_goldCost.info("Gold Cost Info: {} , {} , {} , {} , {}, {} , {}",new Object[]{ Util.getServerTime() , player.getId() , itemId , count , gold_before , player.getGoldCoins() , needPrice});
                }
                else
                {
                    buyStatus=2;
                }
            }
        }
        else
        {
            //道具不存在
            buyStatus=5;
        }
        builder.setStatus(buyStatus);
        builder.setServertime(Util.getServerTime());
        
        player.setLastVerifyTime(Util.getServerTime());

        player.sendPacket(OP.code.OPCODE_BUY_TOOLSRET_S_VALUE, builder);// 发送返回包
        long current_time_2 = Util.getServerMillTime();
        long taken_time=current_time_2-current_time;
        log.info("buy tool handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
        logger2.info("buy tool handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
    }
}
