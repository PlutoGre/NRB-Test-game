package com.oldmountain.dzm.services.daoImpl;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.services.dao.SkillBookDao;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:14
 * To change this template use File | Settings | File Templates.
 */
public class SkillBookDaoImpl implements SkillBookDao
{
    @Override
    public List<SkillBook> getSkillBook(Player player)
    {
        return DZM.getEntityManager().query(SkillBook.class, "from SkillBook where playerId = ?", player.getId());
    }

    @Override
    public void addSkillBook(Player player, int id)
    {
        DZM.getEntityManager().createSync(player.getSkillBooks().get(id));
    }

    @Override
    public void updateSkillBook(Player player, SkillBook skillBook)
    {
        DZM.getEntityManager().updateSync(skillBook);
    }

    @Override
    public void deleteSkillBook(Player player, int skillId)
    {
        DZM.getEntityManager().deleteSync(player.getSkillBooks().get(skillId));
        player.getSkillBooks().remove(skillId);
    }
}
