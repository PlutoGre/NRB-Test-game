package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.entity.SysMsgInfo;
import net.sf.json.JSONArray;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: lvjc
 * Date: 13-4-4
 * Time: 下午5:01
 * To change this template use File | Settings | File Templates.
 */
public interface SysMsgInfoDao
{
    public List<SysMsgInfo> getSysMsgInfo(int id);
    public int addSysMsgInfo(SysMsgInfo sysMsgInfo);
    public int updateSysMsgInfo(SysMsgInfo sysMsgInfo);
    public boolean deleteSysMsgInvalid(int playerId,int id);
    public int getMaxSequence();
    public int getCountSysMsgByReadTime(int playerId,int readTime);
}
