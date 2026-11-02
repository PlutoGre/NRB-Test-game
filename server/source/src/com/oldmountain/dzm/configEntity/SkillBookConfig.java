package com.oldmountain.dzm.configEntity;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-11
 * Time: 上午10:05
 * To change this template use File | Settings | File Templates.
 */
public class SkillBookConfig
{
    private int skillBookId;
    private int skillId;
    private int skillQuality;

    public int getSkillBookId()
    {
        return skillBookId;
    }

    public void setSkillBookId(int skillBookId)
    {
        this.skillBookId = skillBookId;
    }

    public int getSkillId()
    {
        return skillId;
    }

    public void setSkillId(int skillId)
    {
        this.skillId = skillId;
    }

    public int getSkillQuality()
    {
        return skillQuality;
    }

    public void setSkillQuality(int skillQuality)
    {
        this.skillQuality = skillQuality;
    }
}
