package com.oldmountain.dzm.packethandler;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.configEntity.Diamond;
import com.oldmountain.dzm.configEntity.DiamondConfig;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.InlaidStoneInfo.*;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.PunchStoneInfo.*;
import com.oldmountain.dzm.message.RemoveStoneInfo.*;
import com.oldmountain.dzm.message.UpgradeStoneInfo.*;
import com.oldmountain.dzm.message.UserBattleArray;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.util.*;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class EquipDiamondHandler
{

	private static final Logger logger = LoggerFactory.getLogger("EquipDiamond");

    /**
     * 升级宝石
     * @param packet
     * @param session
     */
	public static void UpgradeStone(Packet packet, IoSession session)
    {
        OPUpgradeStoneInfo upgradeStoneInfo = null;
        try
        {
            upgradeStoneInfo = OPUpgradeStoneInfo.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version = upgradeStoneInfo.getVersion();//version default = 1;
        int diamondItemId = upgradeStoneInfo.getStoneid();//version default = 1;
        int clientStoneNum=upgradeStoneInfo.getStonenum();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        long startTime = Util.getServerMillTime();
        logger.info("UpgradeStone received: PlayerId:{}, diamondItemId:{},startTime:{}", new Object[]{id, diamondItemId,startTime});

        if(!checkUserLevelAndOpenStatus(player))
        {
            return;
        }
        //要升级的道具不存在
        if(!player.getBags().containsKey(diamondItemId))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Tools :"+diamondItemId+" Current Tools Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        Bag _bag=player.getBags().get(diamondItemId);
        Diamond _diamond= XMLTemplateService.getDiamondByItemId(diamondItemId);
        if(_diamond.getNextItemId()==0)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Diamond :"+diamondItemId+" Can Not Upgrade!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        int _upgradeNeedDiamondCount=_diamond.getUpgradeConsumeDiamon();
        int _upgradeNeedSilverCoins=_diamond.getUpgradeConsumeSilver();
        //升级到下一等级的宝石数量条件不满足
        if(_bag.getCount()<_upgradeNeedDiamondCount)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Tools Upgrade Need Count Not Satisfy!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //升级所需要的贝里不足
        if(player.getSilvercoins()<_upgradeNeedSilverCoins)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Upgrade Diamond Need SilverCoins Not Satisfy!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        //判断当前宝石可升级的数量
        int _canUpgradeCount=_bag.getCount()/_upgradeNeedDiamondCount;
        if(clientStoneNum<0)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Tools Upgrade Need Count Not Satisfy!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        if(clientStoneNum!=0)
        {
              //modify by dylan 前端提供升级宝石的次数
              if(_canUpgradeCount<clientStoneNum)
              {
                  ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                  eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
                  eBuilder.setErrormsg("Tools Upgrade Need Count Not Satisfy!");
                  player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                  return;
              }
              else
              {
                  _canUpgradeCount=clientStoneNum;
              }
        }
        //判断可升级次数的贝里数是否满足
        long _canUpgradeSilverCoins=player.getSilvercoins()/_upgradeNeedSilverCoins;
        if(_canUpgradeSilverCoins<_canUpgradeCount)
        {
            //modify by dylan 前端提供升级宝石的次数
            if(clientStoneNum!=0)
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE);
                eBuilder.setErrormsg("Upgrade Diamond Need SilverCoins Not Satisfy!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                return;
            }
            _canUpgradeCount = (int) _canUpgradeSilverCoins;
        }
        //宝石消减
        _bag.setCount(_bag.getCount()-_canUpgradeCount*_upgradeNeedDiamondCount);
        player.updateOneBag(_bag);
        //扣除贝里
        player.setSilvercoins(player.getSilvercoins() - _upgradeNeedSilverCoins * _canUpgradeCount);
        //处理升级后的道具宝石数量
        BagUtil.getInstance().oprBagByToolId(player,_diamond.getNextItemId(),_canUpgradeCount);

        OPUpgradeStoneInfoRet.Builder builder = OPUpgradeStoneInfoRet.newBuilder();
        builder.setServertime((int) System.currentTimeMillis());
        builder.addToolInfo(BagToolServices.buildToolBuilder(_bag));
        builder.addToolInfo(BagToolServices.buildToolBuilder(player.getBags().get(_diamond.getNextItemId())));
        builder.setSilvercoins(player.getSilvercoins());
        player.sendPacket(OP.code.OPCODE_UPGRADE_STONEINFORET_S_VALUE, builder);// 发送返回包
        long endTime = Util.getServerMillTime();
        long taken_time=endTime-startTime;
        logger.info("UpgradeStone ret send: {}, Cost:{}, {}", new Object[]{id, taken_time});
    }

    /**
     * 拆除宝石
     * @param packet
     * @param session
     */
    public static void RemoveStone(Packet packet, IoSession session)
    {
        OPRemoveStoneInfo removeStoneInfo = null;
        try
        {
            removeStoneInfo = OPRemoveStoneInfo.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version = removeStoneInfo.getVersion();//version default = 1;
        int equipid = removeStoneInfo.getEquipid();//version default = 1;
        int position= removeStoneInfo.getPosition();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        long startTime = Util.getServerMillTime();
        logger.info("RemoveStone received: PlayerId:{}, EquipId:{},Position:{},startTime:{}", new Object[]{id, equipid,position,startTime});

        if(!checkUserLevelAndOpenStatus(player))
        {
            return;
        }

        //装备不存在
        if(!player.getEquips().containsKey(equipid))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_REMOVE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Current Equip :"+equipid+" Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        Equip _equip=player.getEquips().get(equipid);
        int _diamondItemId=_equip.getInlayDiamonIdByIndex(position);
        //装备上不存在被镶嵌的宝石信息无法拆除
        if(_diamondItemId==0)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_REMOVE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Current DiamondItemId Is Zero Or Can't Remove!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        //包裹里面是否存在宝石相关信息，玩家的包裹必然存在宝石信息，
        if(!player.getBags().containsKey(_diamondItemId))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_REMOVE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Oraginal Tool Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        Diamond _diamond=XMLTemplateService.getDiamondByItemId(_diamondItemId);

        //宝石拆除器不存在
        if(!player.getBags().containsKey(DiamondConfig.DiamondRemoveToolsId))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_REMOVE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Current DiamondRemoveToolsId Is Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        //装备宝石拆除器数量不足
        Bag _removeToolBag=player.getBags().get(DiamondConfig.DiamondRemoveToolsId);
        if(_removeToolBag.getCount()<_diamond.getDismantleConsume())
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_REMOVE_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Current DiamondRemoveToolsId:"+DiamondConfig.DiamondRemoveToolsId+" Is Not Enough!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //装备宝石道具拆除器消耗
        _removeToolBag.setCount(_removeToolBag.getCount()-_diamond.getDismantleConsume());
        player.updateOneBag(_removeToolBag);
        //宝石从装备拿下后对宝石数量进行增加一个操作
        BagUtil.getInstance().oprBagByToolId(player,_diamondItemId,1);

        //拆除宝石
        _equip.openOrRemoveDiamonHoleByIndex(position);
        //计算拆除宝石后的装备属性
        EquipXmlTemplate equipConfig = XMLTemplateService.getEquipTemplate(_equip.getItemid());
        _equip.calcEquipAttr(equipConfig.getQuantity(), equipConfig.getGrowth(),player.getLevel());
        player.updateOneEquip(_equip);
        OPRemoveStoneInfoRet.Builder builder = OPRemoveStoneInfoRet.newBuilder();
        //如果装备存在于阵容，计算阵容信息
        if(_equip.getBattleid()!=0)
        {//处理阵容信息
            int battleid = _equip.getBattleid();
            String battleids = player.getBattleids();
            String[] battleArray = battleids.split(",");
            for (int i = 0; i < battleArray.length; i++)
            {
                if (battleid == Integer.parseInt(battleArray[i]))
                {
                    Battle battle = player.getBattlearray().get(battleid);
                    Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
                    BattleUtil.resetBattleProperty(battle, disciple, player,null,0);
                    UserBattleArray.OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder= UserBattleArray.OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
                    BattleUtil.buildBattleBuilder(battleBuilder, battle,i);
                    builder.setBattle(battleBuilder);
                    player.updateOneBattle(battle);
                    BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(i, battle);
                    break;
                }
            }
        }

        builder.setServertime((int) System.currentTimeMillis());
        builder.addEquipInfo(EquipUtil.buildReturn(_equip));
        builder.addToolInfo(BagToolServices.buildToolBuilder(_removeToolBag));
        builder.addToolInfo(BagToolServices.buildToolBuilder(player.getBags().get(_diamondItemId)));
        player.sendPacket(OP.code.OPCODE_REMOVE_STONEINFORET_S_VALUE, builder);// 发送返回包
        long endTime = Util.getServerMillTime();
        long taken_time=endTime-startTime;
        logger.info("RemoveStone ret send: {}, Cost:{}, {}", new Object[]{id, taken_time});
    }

    /**
     * 镶嵌宝石
     * @param packet
     * @param session
     */
    public static void InlaidStone(Packet packet, IoSession session)
    {
        OPInlaidStoneInfo inlaidStoneInfo = null;
        try
        {
            inlaidStoneInfo = OPInlaidStoneInfo.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version = inlaidStoneInfo.getVersion();//version default = 1;
        int stoneid = inlaidStoneInfo.getStoneid();//version default = 1;
        int equipid = inlaidStoneInfo.getEquipid();//version default = 1;
        int position= inlaidStoneInfo.getPosition();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        long startTime = Util.getServerMillTime();
        logger.info("InlaidStone received: PlayerId:{}, EquipId:{},Position:{},Stoneid:{},startTime:{}", new Object[]{id, equipid,position,stoneid,startTime});

        if(!checkUserLevelAndOpenStatus(player))
        {
            return;
        }
        //装备不存在
        if(!player.getEquips().containsKey(equipid))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_INLAID_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Current Equip :"+equipid+" Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        Equip _equip=player.getEquips().get(equipid);

        //包裹里面是否存在宝石相关信息，玩家的包裹必然存在宝石信息，
        if(!player.getBags().containsKey(stoneid))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_INLAID_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg(stoneid+ " id Tools Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        Diamond _diamond=XMLTemplateService.getDiamondByItemId(stoneid);
        if(_diamond==null)
        { //宝石信息不存在
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_INLAID_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("InlaidStone "+stoneid+" Tool Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        if(!_equip.canInlayStoneByIndex(position))
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_INLAID_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Current Diamond Hole Can't Inlay!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        Bag _inlaidDiamondBag=player.getBags().get(stoneid);
        if(_inlaidDiamondBag.getCount()<1)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_INLAID_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Tools Not Enough!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }

        //宝石镶嵌后，数量减少一个
        BagUtil.getInstance().oprBagByToolId(player,stoneid,-1);
        //镶嵌宝石
        _equip.inlayOrRemoveDiamonInfo(position, Equip.Diamon_Hole_Stats_Inlay, stoneid);
        //计算镶嵌宝石后的装备属性
        EquipXmlTemplate equipConfig = XMLTemplateService.getEquipTemplate(_equip.getItemid());
        _equip.calcEquipAttr(equipConfig.getQuantity(), equipConfig.getGrowth(),player.getLevel());
        player.updateOneEquip(_equip);

        OPInlaidStoneInfoRet.Builder builder = OPInlaidStoneInfoRet.newBuilder();
        //如果装备存在于阵容，计算阵容信息
        if(_equip.getBattleid()!=0)
        {//处理阵容信息
            int battleid = _equip.getBattleid();
            String battleids = player.getBattleids();
            String[] battleArray = battleids.split(",");
            for (int i = 0; i < battleArray.length; i++)
            {
                if (battleid == Integer.parseInt(battleArray[i]))
                {
                    Battle battle = player.getBattlearray().get(battleid);
                    Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
                    BattleUtil.resetBattleProperty(battle, disciple, player,null,0);
                    UserBattleArray.OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder= UserBattleArray.OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
                    BattleUtil.buildBattleBuilder(battleBuilder, battle,i);
                    builder.setBattle(battleBuilder);
                    player.updateOneBattle(battle);
                    BattleUtil.getUserBattleArrayObject(player).getBattlearrays().set(i, battle);
                    break;
                }
            }
        }

        builder.setServertime((int) System.currentTimeMillis());
        builder.addEquipInfo(EquipUtil.buildReturn(_equip));
        builder.addToolInfo(BagToolServices.buildToolBuilder(player.getBags().get(stoneid)));
        builder.setServertime((int) System.currentTimeMillis());
        player.sendPacket(OP.code.OPCODE_INLAID_STONEINFORET_S_VALUE, builder);// 发送返回包
        long endTime = Util.getServerMillTime();
        long taken_time=endTime-startTime;
        logger.info("InlaidStone ret send: {}, Cost:{}, {}", new Object[]{id, taken_time});
    }

    /**
     * 打孔
     * @param packet
     * @param session
     */
    public static void PunchEquipStone(Packet packet, IoSession session)
    {
        OPPunchStoneInfo punchStoneInfo = null;
        try
        {
            punchStoneInfo = OPPunchStoneInfo.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }

        int version = punchStoneInfo.getVersion();
        int equipid = punchStoneInfo.getEquipid();
        int position= punchStoneInfo.getPosition();
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        long startTime = Util.getServerMillTime();
        logger.info("PunchEquipStone received: PlayerId:{}, EquipId:{},Position:{},startTime:{}", new Object[]{id, equipid,position,startTime});

        if(!checkUserLevelAndOpenStatus(player))
        {
            return;
        }
        //当前装备是否存在
        Equip _equipItem=player.getEquips().get(equipid);
        if(_equipItem==null)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_PUNCH_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("EquipId:"+equipid+" Current Equip Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //当前装备是否具备开孔条件
        if(!_equipItem.checkOpenDiamondHoleByIndex(position))
        {
           ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
           eBuilder.setErrorid(OP.code.OPCODE_PUNCH_STONEINFORET_S_VALUE);
           eBuilder.setErrormsg("EquipId:"+equipid+" Current Equip Dunch Hole Not Satisfy!");
           player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
           return;
        }
        //开孔需要的道具是否存在
        Bag _bag=player.getBags().get(DiamondConfig.DiamondOpenToolsId);
        if(_bag==null)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_PUNCH_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Tools :"+DiamondConfig.DiamondOpenToolsId+" Current Tools Not Exists!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //开孔需要消耗的道具数量是否满足
        int openHoleNeedConsumeTool=DiamondConfig.getOpenHoleConsumeByIndex(position);
        if(_bag.getCount()<openHoleNeedConsumeTool)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_PUNCH_STONEINFORET_S_VALUE);
            eBuilder.setErrormsg("Tools :"+DiamondConfig.DiamondOpenToolsId+" Current Tools Not Enough!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }


        _equipItem.openOrRemoveDiamonHoleByIndex(position);  //装备开孔
        _bag.setCount(_bag.getCount() - openHoleNeedConsumeTool);//扣除开孔道具数量
        player.updateOneEquip(_equipItem);
        player.updateOneBag(_bag);

        OPPunchStoneInfoRet.Builder builder = OPPunchStoneInfoRet.newBuilder();
        builder.setServertime((int) System.currentTimeMillis());
        builder.addEquipInfo(EquipUtil.buildReturn(_equipItem));
        builder.addToolInfo(BagToolServices.buildToolBuilder(_bag));
        player.sendPacket(OP.code.OPCODE_PUNCH_STONEINFORET_S_VALUE, builder);// 发送返回包
        long endTime = Util.getServerMillTime();
        long taken_time=endTime-startTime;
        logger.info("PunchEquipStone ret send: {}, Cost:{}, {}", new Object[]{id, taken_time});
    }

    /**
     * 检测playder的等级是否满足
     * @param player
     * @return
     */
    public static boolean checkUserLevelAndOpenStatus(Player player)
    {
        if(player.getLevel()>= DiamondConfig.DiamondOpenLevel&&DiamondConfig.DiamondOpen())
        {
            return true;
        }
        else
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(1);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Level Not Satisfy!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return false;
        }
    }
}
