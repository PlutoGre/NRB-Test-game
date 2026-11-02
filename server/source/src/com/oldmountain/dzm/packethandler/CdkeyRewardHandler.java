package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Cdkey;
import com.oldmountain.dzm.entity.CdkeyXmlTemplate;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.entity.ToolContainReward;
import com.oldmountain.dzm.message.CdkeyReward;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.CdkeyReward.OPCdkeyRewardRet;
import com.oldmountain.dzm.message.ContinueLogin.OPContinueLoginRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserRewardInfo.OPRewardInfoRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.EquipServices;
import com.oldmountain.dzm.services.SkillServices;
import com.oldmountain.dzm.services.SoulServices;
import com.oldmountain.dzm.services.UserPropertyServices;
import com.oldmountain.dzm.util.BagUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.SoulUtil;
import com.oldmountain.dzm.util.UserToolConst;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
/**
 * 兑换cdkey奖励
 * 状态码status:
 * 			0 : cdkey错误
 * 			1 : 兑换成功
 * 			2 : cdkey已使用
 * 			3 : 已使用过同类cdkey
 * */
public class CdkeyRewardHandler {
	private static final Logger log = LoggerFactory.getLogger(CdkeyRewardHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("CdkeyReward");

	public static void getReward(Packet packet, IoSession session) {		
		CdkeyReward.OPCdkeyReward params = null;
		CdkeyReward.OPCdkeyRewardRet.Builder builder =CdkeyReward.OPCdkeyRewardRet.newBuilder();
		try{
			params = CdkeyReward.OPCdkeyReward.parseFrom(packet.data.array());
		}catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int version = params.getVersion();
		String cdkey = params.getCdkey().toLowerCase().trim();
		String typeId="";
		int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int playerid=player.getId();
        int status=1;	//使用结果状态码
        boolean cdkey_form = true;	//cdkey格式
        Cdkey cdkey_entity=null;
        log.info("cdkey reward handler received: {}, {}, {}", new Object[]{ playerid , version , cdkey});
        logger2.info("cdkey reward handler received: {}, {}, {}", new Object[]{ playerid , version , cdkey});
        long current_time = Util.getServerMillTime();
    /********************************************************************************************************************/  
        //cdkey格式判断
        if(cdkey.length()!=14)
        {
        	cdkey_form=false;
        	status=0;
        }
        else 
        {
        	char[] chs=cdkey.toCharArray();
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
        			cdkey_form=false;
        			status=0;
        		}
        	}
        	else {
				cdkey_form=false;
				status=0;
			}
        }
        if(status==1){
	        typeId=cdkey.substring(0, 2);
	        CdkeyXmlTemplate cdkeyXmlTemplate=XMLTemplateService.getCdkeyXmlTemplate(typeId);
	        
	        //判断xml中是否有配置
	        if(cdkeyXmlTemplate==null)
	        {
	        	status=0;
	        }
	        else
	        {
		        List<Object> list = DZM.getEntityManager().limitQuery("from Cdkey where cd_key = ?", 0, 1, cdkey);
				if (list.size() == 1) 
				{
					cdkey_entity=(Cdkey) list.get(0);
					//cdkey已使用
					if(cdkey_entity.getStatus()==1)
					{
						status=2;
					}
				}else {
					//数据库中不存在
					status=0;
				}
		        if(status==1){
			        //判断是否使用过同类cdkey
			        String cdkeyTypes=player.getUserStats().getCdkeyTypes();
			        if(StringUtils.isNotBlank(cdkeyTypes)){
			        	String[] types=cdkeyTypes.split(",");
			        	for(int i=0 ; i<types.length ; i++){
			        		if(typeId.equals(types[i])){
			        			status=3;
			        			break;
			        		}
			        	}
			        }
		        }
	        }
	        if(status==1){
	        	ToolContainReward[] containTools = cdkeyXmlTemplate.getContainTools();
	        	 int len = containTools.length;
	                for(int i=0;i<len;i++)
	                {
	                	OPRewardInfoRet.RewardInfo.Builder rewardbBuilder = OPRewardInfoRet.RewardInfo.newBuilder();
	                    int type =UserToolConst.getType(containTools[i].getType());
	                    int giftItemId = containTools[i].getItemId();
	                    switch (type)
	                    {
	                        case UserToolConst.USER_PROPERTY_TYPE:
	                            UserPropertyServices.changeProperty(player,giftItemId,containTools[i].getQuantity());
	                            switch (giftItemId)
	                            {
	                                case UserToolConst.USER_PROPERTY_GOLD_COINS:
	                                    builder.setGoldcoins(player.getSysgoldcoins()+player.getRechargegoldcoins());
	                                    break;
	                                case UserToolConst.USER_PROPERTY_SILVER_COINS:
	                                    builder.setSilvercoins(player.getSilvercoins());
	                                    break;
	                                case UserToolConst.USER_PROPERTY_EXP:
	                                    builder.setExp(player.getExp());
	                                    break;
	                                default:
	                                    break;
	                            }
	                            break;
	                        case UserToolConst.DISCIPLE_TYPE:
	                        	boolean has= false;
	                        	List<Integer> itemIds=player.getDiscipleitems();
	                        	if(itemIds!=null && itemIds.size()>0){
	                        		for(int j=0;j<itemIds.size();j++){
	                        			if(itemIds.get(j)==giftItemId){
	                        				has=true;
	                        				break;
	                        			}
	                        		}
	                        	}
	                        	if(!has){
	                        		Disciple disciple = DiscipleUtil.newDisciple(player, containTools[i].getItemId(), 1, 0);
	                        		builder.addDisciple(DiscipleUtil.buildDiscipleBuilder(disciple));
	                        		Skill defaultSkill = player.getSkills().get(disciple.getSkill1());
	                        		OPUserSkillInfoRet.SkillInfo.Builder skillBuilder = SkillUtil.buildSkillBuilder(defaultSkill);
	                        		builder.addSkillInfo(skillBuilder);
	                        	}
	                        	else{
	                        		HashMap<Integer, Soul> userSouls = player.getSouls();
	                        		DiscipleXmlTemplate discipleXmlTemplate = XMLTemplateService.getDiscipleTemplate(giftItemId);
	                    			Soul soul = new Soul();
	                    			int count = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(discipleXmlTemplate.getQuality());
	                    			if (userSouls.containsKey(giftItemId)) {
	                    				soul = userSouls.get(giftItemId);
	                    				soul.setCount(soul.getCount() + count);
	                    				player.updateOneSoul(soul);
	                    			}
	                    			else {
	                    				soul.setItemid(giftItemId);
	                    				soul.setPlayerid(player.getId());
	                    				soul.setCount(count);
	                    				player.addOneSoul(soul);
	                    			}
	                    			containTools[i].setQuantity(count);
	                    			builder.addSoulInfo(SoulServices.addSoul(player, giftItemId, 0));
	                        	}
	                        	break;
	                        case UserToolConst.SOUL_TYPE:
	                            builder.addSoulInfo(SoulServices.addSoul(player, giftItemId, containTools[i].getQuantity()));
	                            break;
	                        case UserToolConst.SKILL_TYPE:
	                            builder.addSkillInfo(SkillServices.createSkillAlone(player,giftItemId));
	                            break;
	                        case UserToolConst.EQUIP_TYPE:
	                            builder.addEquipInfo(EquipServices.createEquitAlone(player,giftItemId));
	                            break;
	                        case UserToolConst.TOOLS_TYPE:
	                            builder.addToolInfo(BagToolServices.addBagTool(player,giftItemId,containTools[i].getQuantity()));
	                            //添加宝箱运气池的运气值
	                            if(UserToolConst.getType(giftItemId)==UserToolConst.TREASURE_TOOL)
	                            {
	                                BagUtil.getInstance().addLuckPool(player,giftItemId,containTools[i].getQuantity());
	                            }
	                            break;
	                    }
	                    rewardbBuilder.setItemType(containTools[i].getType());
	                    rewardbBuilder.setItemId(giftItemId);
	                    rewardbBuilder.setCount(containTools[i].getQuantity());
	                    builder.addReward(rewardbBuilder);
	    		}
	        	player.getUserStats().addCdkeyType(typeId);
	        	//数据库中设置状态为已使用
	        	cdkey_entity.setUsed_time(Util.getServerTime());
	        	cdkey_entity.setPlayerid(player.getId());
	        	cdkey_entity.setStatus(1);
	        	//同步更新数据库
	        	DZM.getEntityManager().updateSync(cdkey_entity);
	        }
        }
        builder.setServertime(Util.getServerTime());
        builder.setStatus(status);
        player.setLastVerifyTime(Util.getServerTime());
        player.sendPacket(OP.code.OPCODE_CDKEY_REWARDRET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("cdkey reward handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
		logger2.info("cdkey reward handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}

}
