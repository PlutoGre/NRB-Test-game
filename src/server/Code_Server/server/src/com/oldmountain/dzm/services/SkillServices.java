package com.oldmountain.dzm.services;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.UserSkillInfo;
import com.oldmountain.dzm.util.Util;

import java.util.ArrayList;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午8:56
 * To change this template use File | Settings | File Templates.
 */
public class SkillServices
{
    public static UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder createSkillAlone(Player player,int itemId)
    {
        UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder _skillBuilder=UserSkillInfo.OPUserSkillInfoRet.SkillInfo.newBuilder();
        int id=DaoManager.getSkillDao().addSkill(player,itemId);
        Skill skill=player.getSkills().get(id);
        _skillBuilder.setId(skill.getId());
        _skillBuilder.setItemid(skill.getItemid());
        _skillBuilder.setLevel(skill.getLevel());
        _skillBuilder.setQuantity(skill.getQuantity());
        _skillBuilder.setConsume(skill.getConsume());
        _skillBuilder.setIsborn(skill.getIsdefaultskill());
        return _skillBuilder;
    }

    public static ArrayList<UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder> createSkillMuli(Player player, int itemId, int count)
    {
        ArrayList<UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder> arrayList=new ArrayList<UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder>();
        for(int i=0;i<count;++i)
        {
            arrayList.add(createSkillAlone(player, itemId));
        }
        return arrayList;
    }
    public static UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder createSkillWithLevel(Player player,int itemId,int level)
    {
        UserSkillInfo.OPUserSkillInfoRet.SkillInfo.Builder _skillBuilder=UserSkillInfo.OPUserSkillInfoRet.SkillInfo.newBuilder();
        int id=Util.rewardSkillWithLevel(player, itemId, level);
        Skill skill=player.getSkills().get(id);
        _skillBuilder.setId(skill.getId());
        _skillBuilder.setItemid(skill.getItemid());
        _skillBuilder.setLevel(skill.getLevel());
        _skillBuilder.setQuantity(skill.getQuantity());
        _skillBuilder.setConsume(skill.getConsume());
        _skillBuilder.setIsborn(skill.getIsdefaultskill());
        return _skillBuilder;
    }
}
