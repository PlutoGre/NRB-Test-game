package com.oldmountain.dzm.entity;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:06
 * To change this template use File | Settings | File Templates.
 */
public class AdventureFightRank implements Comparable<AdventureFightRank>
{
    private int id;
    private int playerId;
    private String playerName;
    private int playerLevel;
    private int barrierCount;
    private int passBarrier;
    private int obtainStar;
    private int continueInRankAmount;
    private int rankTime;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getPlayerId() {
        return playerId;
    }

    public void setPlayerId(int playerId)
    {
        this.playerId = playerId;
    }

    public String getPlayerName() {
        return playerName;
    }

    public void setPlayerName(String playerName) {
        this.playerName = playerName;
    }

    public int getPlayerLevel() {
        return playerLevel;
    }

    public void setPlayerLevel(int playerLevel) {
        this.playerLevel = playerLevel;
    }

    public int getRankTime() {
        return rankTime;
    }

    public void setRankTime(int rankTime) {
        this.rankTime = rankTime;
    }

    public int getBarrierCount() {
        return barrierCount;
    }

    public void setBarrierCount(int barrierCount) {
        this.barrierCount = barrierCount;
    }

    public int getPassBarrier() {
        return passBarrier;
    }

    public void setPassBarrier(int passBarrier) {
        this.passBarrier = passBarrier;
    }

    public int getObtainStar() {
        return obtainStar;
    }

    public void setObtainStar(int obtainStar) {
        this.obtainStar = obtainStar;
    }

    public int getContinueInRankAmount() {
        return continueInRankAmount;
    }

    public void setContinueInRankAmount(int continueInRankAmount) {
        this.continueInRankAmount = continueInRankAmount;
    }

    @Override
    public int compareTo(AdventureFightRank o)
    {
        return this.getObtainStar()-o.getObtainStar();
    }
}
