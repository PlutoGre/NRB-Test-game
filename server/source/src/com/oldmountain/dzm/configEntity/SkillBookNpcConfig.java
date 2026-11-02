package com.oldmountain.dzm.configEntity;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-9
 * Time: 下午8:49
 * To change this template use File | Settings | File Templates.
 */
public class SkillBookNpcConfig
{
    private int minDiscipleId;
    private int maxDiscipleId;
    private float hpAddition;
    private float attackAddition;
    private float defenseAddition;
    private float rpAddition;
    private int floorCaptainLevel;
    private int ceilCaptainLevel;
    private int floorTeamLevel;
    private int ceilTeamLevel;
    private int teamLevelRatio;

    public int getMinDiscipleId() {
        return minDiscipleId;
    }

    public void setMinDiscipleId(int minDiscipleId) {
        this.minDiscipleId = minDiscipleId;
    }

    public int getMaxDiscipleId() {
        return maxDiscipleId;
    }

    public void setMaxDiscipleId(int maxDiscipleId) {
        this.maxDiscipleId = maxDiscipleId;
    }

    public float getHpAddition() {
        return hpAddition;
    }

    public void setHpAddition(float hpAddition) {
        this.hpAddition = hpAddition;
    }

    public float getAttackAddition() {
        return attackAddition;
    }

    public void setAttackAddition(float attackAddition) {
        this.attackAddition = attackAddition;
    }

    public float getDefenseAddition() {
        return defenseAddition;
    }

    public void setDefenseAddition(float defenseAddition) {
        this.defenseAddition = defenseAddition;
    }

    public float getRpAddition() {
        return rpAddition;
    }

    public void setRpAddition(float rpAddition) {
        this.rpAddition = rpAddition;
    }

    public int getFloorCaptainLevel() {
        return floorCaptainLevel;
    }

    public void setFloorCaptainLevel(int floorCaptainLevel) {
        this.floorCaptainLevel = floorCaptainLevel;
    }

    public int getCeilCaptainLevel() {
        return ceilCaptainLevel;
    }

    public void setCeilCaptainLevel(int ceilCaptainLevel) {
        this.ceilCaptainLevel = ceilCaptainLevel;
    }

    public int getFloorTeamLevel() {
        return floorTeamLevel;
    }

    public void setFloorTeamLevel(int floorTeamLevel) {
        this.floorTeamLevel = floorTeamLevel;
    }

    public int getCeilTeamLevel() {
        return ceilTeamLevel;
    }

    public void setCeilTeamLevel(int ceilTeamLevel) {
        this.ceilTeamLevel = ceilTeamLevel;
    }

    public int getTeamLevelRatio() {
        return teamLevelRatio;
    }

    public void setTeamLevelRatio(int teamLevelRatio) {
        this.teamLevelRatio = teamLevelRatio;
    }
}
