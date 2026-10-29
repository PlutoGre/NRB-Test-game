package com.oldmountain.dzm.entity;

public class CdkeyXmlTemplate {
	private String type;	//cdkey类型
	private String rewards;		//对应礼包id; 
	private ToolContainReward[] containTools;
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public String getRewards() {
		return rewards;
	}
	public void setRewards(String rewards) {
		this.rewards = rewards;
	}
	public ToolContainReward[] getContainTools() {
        if(!this.rewards.equals("none"))
        {
            String[] arrTools = this.rewards.split(",");
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
        return containTools;
    }
	
}
