package com.oldmountain.dzm.configEntity;

import com.oldmountain.dzm.entity.ToolContainReward;

public class InviteConfig {
	private int type;//邀请奖励的类型
	private int required;//需要达成的条件额度
	private ToolContainReward[] containTools;
	
	public int getType(){
		return type;
	}
	
	public void setType(int type){
		this.type = type;
	}
	
	public int getRequired(){
		return required;
	}
	
	public void setRequired(int required){
		this.required = required;
	}
	
	public ToolContainReward[] getContainTools() {
        return containTools;
    }
	
	public void setContainTools(String containToolsStr) {
        if(!containToolsStr.equals("none"))
        {
            String[] arrTools = containToolsStr.split(",");
            int arrLen = arrTools.length;
            containTools = new ToolContainReward[arrLen];
            try
            {
                for(int i=0;i<arrLen;i++)
                {
                    String[] arrTmp =  arrTools[i].split(":");
                    containTools[i] = new ToolContainReward();
                    containTools[i].setType(Integer.parseInt(arrTmp[0]));
                    containTools[i].setItemId(Integer.parseInt(arrTmp[1]));
                    containTools[i].setQuantity(Integer.parseInt(arrTmp[2]));
                }
            }catch (Exception e)
            {
               System.out.println(e.getMessage());
            }
        }
    }
}
