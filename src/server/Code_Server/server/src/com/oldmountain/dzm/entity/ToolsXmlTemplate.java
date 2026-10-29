package com.oldmountain.dzm.entity;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-1
 * Time: 下午2:48
 * To change this template use File | Settings | File Templates.
 */
public class ToolsXmlTemplate {
    private int toolId;
    private int opType;
    private int priceType;
    private int price;
    private int buyTimes;
    private ToolContainReward[] containTools;
    private int vipLevel;

    public int getVipLevel() {
        return vipLevel;
    }

    public void setVipLevel(int vipLevel) {
        this.vipLevel = vipLevel;
    }

    public int getToolId(){
        return toolId;
    }

    public void setToolId(int toolId){
        this.toolId = toolId;
    }

    public int getOpType() {
        return opType;
    }

    public void setOpType(int opType) {
        this.opType = opType;
    }

    public int getPrice() {
        return price;
    }

    public void setPrice(int price) {
        this.price = price;
    }

    public int getPriceType() {
        return priceType;
    }

    public void setPriceType(int priceType) {
        this.priceType = priceType;
    }

    public int getBuyTimes() {
        return buyTimes;
    }

    public void setBuyTimes(int buyTimes) {
        this.buyTimes = buyTimes;
    }

    public ToolContainReward[] getContainTools() {
        return containTools;
    }

    public boolean  isContainTools()
    {
        if(containTools.length>0)
        {
            return true;
        }
        return false;
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
