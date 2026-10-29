package com.oldmountain.dzm.util;

import java.util.HashMap;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Title;
import com.oldmountain.dzm.entity.TitleXmlTemplate;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.TitleInfo.OPTitleInfoRet;
import com.oldmountain.dzm.message.UpdateTitle.OPUpdateTitle;

public class TitleUtil {
	public static void checkNewTitle(Player player,int type,int require){
		HashMap<Integer, TitleXmlTemplate> titleConfig = XMLTemplateService.getTypeTitleConfig(type);
		Title userTitle = player.getTitle();
		if (type == 3) {
			require = getRealRequire(require);
		}
		if (titleConfig.containsKey(require)) {
			boolean update = false;
			int id = titleConfig.get(require).getId();
			float quantity = titleConfig.get(require).getQuantity();
            if(userTitle!=null)
            {
                switch (type)
                {
                    case 1:
                        if(userTitle.getCareertypeid() < id){
                            userTitle.setCareertypeid(id);
                            userTitle.setCareertypenum((int)quantity);
                            update = true;
                        }
                        break;
                    case 2:
                        if(userTitle.getViptypeid() < id){
                            userTitle.setViptypeid(id);
                            userTitle.setViptypenum(quantity);
                            update = true;
                        }
                        break;
                    case 3:
                        if(userTitle.getChallengetypeid() < id){
                            userTitle.setChallengetypeid(id);
                            userTitle.setChallengetypenum((int)quantity);
                            update = true;
                        }
                        break;
                    case 4:
                        if(userTitle.getChiptypeid() < id){
                            userTitle.setChiptypeid(id);
                            userTitle.setChiptypenum((int)quantity);
                            update = true;
                        }
                        break;
                    case 5:
                        if(userTitle.getFighttypeid() < id){
                            userTitle.setFighttypeid(id);
                            userTitle.setFighttypenum((int)quantity);
                            update = true;
                        }
                        break;
                    case 6:
                        if(userTitle.getBattletypeid() < id){
                            userTitle.setBattletypeid(id);
                            userTitle.setBattletypenum((int)quantity);
                            update = true;
                        }
                        break;
                    case 7:
                        if(userTitle.getFriendtypeid() < id){
                            userTitle.setFriendtypeid(id);
                            userTitle.setFriendtypenum((int)quantity);
                            update = true;
                        }
                        break;
                    case 8:
                        break;
                    case 9:
                        break;
                }
            }
            
			if (update == true) {
				player.setTitle(userTitle);
				float rate=userTitle.getTotalRate();
				int amount=userTitle.getTotalAmount();
				int momentum=Math.round(amount*(1+rate));
				player.setMomentum(momentum);
				BattleUtil.getUserBattleArrayObject(player).setMomentum(player.getMomentum());
				
				OPUpdateTitle.Builder builder = OPUpdateTitle.newBuilder();
				builder.setTitleId(id);
				builder.setTitleTotalNum(player.getMomentum());
				player.sendPacket(OP.code.OPCODE_UPDATE_TITLE_S_VALUE, builder);
			}
		}
	}
	
	public static void checkVipTitle(Player player,int type,int require){
		HashMap<Integer, TitleXmlTemplate> titleConfig = XMLTemplateService.getTypeTitleConfig(type);
		Title userTitle = player.getTitle();
		if (titleConfig.containsKey(require)) {
			boolean update = false;
			int id = titleConfig.get(require).getId();
			float quantity = titleConfig.get(require).getQuantity();
	        if(userTitle!=null){
				if(userTitle.getViptypeid() < id){
			        userTitle.setViptypeid(id);
			        userTitle.setViptypenum(quantity);
			        update = true;
		        }
	        }
	        if (update == true) {
				player.setTitle(userTitle);
				float rate=userTitle.getTotalRate();
				int amount=userTitle.getTotalAmount();
				int momentum=Math.round(amount*(1+rate));
				player.setMomentum(momentum);
				BattleUtil.getUserBattleArrayObject(player).setMomentum(player.getMomentum());
			}
		}
	}
	private static int getRealRequire(int require){
		int returnRequire = 0;
		if (require <= 1) {
			returnRequire = 1;
		}
		else if (require <= 10) {
			returnRequire = 10;
		}
		else if (require <= 20) {
			returnRequire = 20;
		}
		else if (require <= 30) {
			returnRequire = 30;
		}
		else if (require <= 40) {
			returnRequire = 40;
		}
		else if (require <= 50) {
			returnRequire = 50;
		}
		else if (require <= 100) {
			returnRequire = 100;
		}
		else if (require <= 150) {
			returnRequire = 150;
		}
		else if (require <= 200) {
			returnRequire = 200;
		}
		else if (require <= 250) {
			returnRequire = 250;
		}
		else if (require <= 300) {
			returnRequire = 300;
		}
		else if (require <= 400) {
			returnRequire = 400;
		}
		else if (require <= 500) {
			returnRequire = 500;
		}
		else if (require <= 1000) {
			returnRequire = 1000;
		}
		else if (require <= 1500) {
			returnRequire = 1500;
		}
		else if (require <= 2000) {
			returnRequire = 2000;
		}
		else if (require <= 2500) {
			returnRequire = 2500;
		}
		else if (require <= 3000) {
			returnRequire = 3000;
		}
		else if (require <= 4000) {
			returnRequire = 4000;
		}
		else if (require <= 5000) {
			returnRequire = 5000;
		}
		else if (require <= 6000) {
			returnRequire = 6000;
		}
		else if (require <= 7000) {
			returnRequire = 7000;
		}
		else if (require <= 8000) {
			returnRequire = 8000;
		}
		else if (require <= 9000) {
			returnRequire = 9000;
		}
		else if (require <= 10000) {
			returnRequire = 10000;
		}
		
		return returnRequire;
	}
	public static OPTitleInfoRet.Builder buildTitleBuilder(Player player){
		OPTitleInfoRet.Builder builder = OPTitleInfoRet.newBuilder();
		OPTitleInfoRet.TitleItem.Builder titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
		Title userTitle = player.getTitle();
		if (userTitle.getCareertypeid() != 0) {
			titleBuilder.setTitleID(userTitle.getCareertypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getViptypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getViptypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getChallengetypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getChallengetypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getChiptypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getChiptypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getFighttypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getFighttypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getBattletypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getBattletypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getFriendtypeid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getFriendtypeid());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getFightrankid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getFightrankid());
			titleBuilder.setGetTime(userTitle.getFightranktime());
			builder.addTitles(titleBuilder);
		}
		if (userTitle.getBattlerankid() != 0) {
			titleBuilder = OPTitleInfoRet.TitleItem.newBuilder();
			titleBuilder.setTitleID(userTitle.getBattlerankid());
			titleBuilder.setGetTime(userTitle.getBattleranktime());
			builder.addTitles(titleBuilder);
		}
		return builder;
	}
}