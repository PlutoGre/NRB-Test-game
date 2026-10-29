package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.SkillBook;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-3
 * Time: 下午12:23
 * To change this template use File | Settings | File Templates.
 */
public interface SkillBookDao
{
    public List<SkillBook> getSkillBook(Player player);
    public void addSkillBook(Player player, int id);
    public void updateSkillBook(Player player, SkillBook skillBook);
    public void deleteSkillBook(Player player, int skillId);
}
