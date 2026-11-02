package com.oldmountain.dzm.configEntity;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-7-9
 * Time: 下午7:04
 * To change this template use File | Settings | File Templates.
 */
public class Diamond
{
    private int id;
    private int colorType;
    private int level;
    private int weanponAttr;
    private int armorAttr;
    private int jewelryAttr;
    private int upgradeConsumeSilver;
    private int upgradeConsumeDiamon;
    private int dismantleConsume;
    private int nextItemId;

    public int getId()
    {
        return id;
    }

    public void setId(int id)
    {
        this.id = id;
    }

    public int getColorType()
    {
        return colorType;
    }

    public void setColorType(int colorType)
    {
        this.colorType = colorType;
    }

    public int getLevel()
    {
        return level;
    }

    public void setLevel(int level)
    {
        this.level = level;
    }

    public int getWeanponAttr()
    {
        return weanponAttr;
    }

    public void setWeanponAttr(int weanponAttr)
    {
        this.weanponAttr = weanponAttr;
    }

    public int getArmorAttr()
    {
        return armorAttr;
    }

    public void setArmorAttr(int armorAttr)
    {
        this.armorAttr = armorAttr;
    }

    public int getJewelryAttr()
    {
        return jewelryAttr;
    }

    public void setJewelryAttr(int jewelryAttr)
    {
        this.jewelryAttr = jewelryAttr;
    }

    public int getUpgradeConsumeSilver()
    {
        return upgradeConsumeSilver;
    }

    public void setUpgradeConsumeSilver(int upgradeConsumeSilver)
    {
        this.upgradeConsumeSilver = upgradeConsumeSilver;
    }

    public int getUpgradeConsumeDiamon()
    {
        return upgradeConsumeDiamon;
    }

    public void setUpgradeConsumeDiamon(int upgradeConsumeDiamon)
    {
        this.upgradeConsumeDiamon = upgradeConsumeDiamon;
    }

    public int getDismantleConsume()
    {
        return dismantleConsume;
    }

    public void setDismantleConsume(int dismantleConsume)
    {
        this.dismantleConsume = dismantleConsume;
    }

    public int getNextItemId() {
        return nextItemId;
    }

    public void setNextItemId(int nextItemId)
    {
        this.nextItemId = nextItemId;
    }
}
