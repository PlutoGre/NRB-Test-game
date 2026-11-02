package com.oldmountain.dzm.util;

import java.sql.Timestamp;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 下午5:44
 * To change this template use File | Settings | File Templates.
 */
public class TimeUtil
{
    private static int serverNextDayMinTime=-1;
    /**
     * 获取系统距1970年1月1日总毫秒
     *
     * @return
     */
    public static long getSysCurTimeMillis() {
        return getCalendar().getTimeInMillis();
    }

    /**
     * 获取系统距1970年1月1日总秒
     *
     * @return
     */
    public static long getSysCurSeconds() {
        return getCalendar().getTimeInMillis() / 1000;
    }

    /**
     * 获取系统当前时间
     *
     * @return
     */
    public static Timestamp getSysteCurTime() {
        Timestamp ts = new Timestamp(getCalendar().getTimeInMillis());
        return ts;
    }

    /**
     * 获取指定日期距1970年1月1日总秒
     *
     * @param date
     * @return
     */
    public static long getDateToSeconds(Date date) {
        return getCalendar(date).getTimeInMillis() / 1000;
    }

    /**
     * 指定的毫秒long值转成Timestamp类型
     *
     * @param value
     * @return
     */
    public static java.sql.Timestamp getMillisToDate(long value) {
        return new java.sql.Timestamp(value);
    }

    /**
     * 当前系统时间增加值
     *
     * @param type
     * @param value
     * @return
     */
    public static Calendar addSystemCurTime(int type, int value) {
        Calendar cal = getCalendar();
        switch (type) {
            case Calendar.DATE:// 增加天数
                cal.add(Calendar.DATE, value);
                break;
            case Calendar.HOUR:// 增加小时
                cal.add(Calendar.HOUR, value);
                break;
            case Calendar.MINUTE:// 增加分钟
                cal.add(Calendar.MINUTE, value);
                break;
            case Calendar.SECOND:// 增加秒
                cal.add(Calendar.SECOND, value);
                break;
            case Calendar.MILLISECOND:// 增加毫秒
                cal.add(Calendar.MILLISECOND, value);
                break;
            default:
                System.err.println("当前类型不存在！");
                break;
        }
        return cal;
    }

    /**
     * 服务器下一天的最早时间
     * @return
     */
    public static int getNextDayMinTime()
    {
        if(getSysCurTimeMillis()>serverNextDayMinTime)
        {
            Calendar dt=addSystemCurTime(Calendar.DATE,1);
            dt.set(Calendar.HOUR_OF_DAY, 0);
            dt.set(Calendar.MINUTE, 0);
            dt.set(Calendar.SECOND, 0);
            dt.set(Calendar.MILLISECOND, 0);
            serverNextDayMinTime=(int)(Math.floor(dt.getTimeInMillis()/1000));
        }
        return serverNextDayMinTime;
    }

    /**
     * 格式化日期
     *
     * @param date
     * @return
     */
    public static String getDateFormat(java.util.Date date) {
        DateFormat dataFormat = DateFormat.getDateTimeInstance();
        return dataFormat.format(date);
    }

    /**
     * 获取默认日期2000-01-01
     *
     * @return 返回默认起始时间
     */
    public static java.sql.Timestamp getDefaultDate() {
        java.util.Date defaultDate = null;
        try {
            defaultDate = (java.util.Date) new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").parseObject("2000-01-01 00:00:00");

        } catch (ParseException e) {
            e.printStackTrace();
        }
        return new java.sql.Timestamp(defaultDate.getTime());
    }

    /**
     * 获取默认目上限日期2999-01-01
     *
     * @return 返回默认上限时间
     */
    public static java.sql.Timestamp getDefaultMaxDate() {
        java.util.Date defaultDate = null;
        try {
            defaultDate = (java.util.Date) new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").parseObject("2999-01-01 00:00:00");
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return new java.sql.Timestamp(defaultDate.getTime());
    }

    /**
     * 比较日期是否同一天
     *
     * @param date
     * @return
     */
    public static boolean dateCompare(Date date)
    {
        if(date==null)
            return false;
        java.util.Calendar now = getCalendar();
        java.util.Calendar other = getCalendar(date);
        return dateCompare(now, other) == 0;
    }

    /**
     * 返回两个日期相差天数
     *
     * @param startDate
     *            开始日期
     * @param endDate
     *            结束日期
     * @return
     */
    public static int dateCompare(java.util.Calendar startDate, java.util.Calendar endDate) {
        startDate.set(Calendar.HOUR_OF_DAY, 0);
        startDate.set(Calendar.MINUTE, 0);
        startDate.set(Calendar.SECOND, 0);
        startDate.set(Calendar.MILLISECOND, 0);

        endDate.set(Calendar.HOUR_OF_DAY, 0);
        endDate.set(Calendar.MINUTE, 0);
        endDate.set(Calendar.SECOND, 0);
        endDate.set(Calendar.MILLISECOND, 0);

        int day = (int) (endDate.getTimeInMillis() / 1000 / 60 / 24 - startDate.getTimeInMillis() / 1000 / 60 / 24);
        return day;
    }

    public static long calcDistanceMillis(Date startTime, Date endTime) {
        long startSecond = getDateToSeconds(startTime);
        long endSecond = getDateToSeconds(endTime);
        return (endSecond - startSecond) * 1000;
    }

    /**
     * 间隔时间以小时为单位
     *
     * @param startDate
     * @param interval
     * @return
     */
    @SuppressWarnings("static-access")
    public static boolean isInterval(Date startDate, int interval) {
        // 开始日期
        java.util.Calendar startCalendar = java.util.Calendar.getInstance();
        startCalendar.setTime(startDate);
        // 结束日期
        java.util.Calendar endCalendar = getCalendar();
        if (dateCompare(startCalendar, endCalendar) == 0) {
            if (endCalendar.get(endCalendar.HOUR_OF_DAY) / interval == startCalendar.get(startCalendar.HOUR_OF_DAY) / interval) {
                return true;
            }
        }
        return false;
    }

    public static int timeToFrame(int secondTime) {
        return (secondTime * 25) / 1000;
    }

    /**
     * 获取系统时间
     *
     * @return
     */
    public static java.util.Calendar getCalendar() {
        java.util.Calendar nowCalendar = java.util.Calendar.getInstance();
        nowCalendar.setTime(new java.util.Date());
        return nowCalendar;
    }

    /**
     * 获取指定的时间
     *
     * @param date
     * @return
     */
    private static java.util.Calendar getCalendar(Date date) {
        Calendar calendar = new GregorianCalendar();
        calendar.setTime(date);
        return calendar;
    }

    /**
     * 根据用户传入的时间表示格式，返回当前时间的格式 如果是yyyyMMdd，注意字母y不能大写。
     *
     * @param sformat
     *            yyyyMMddhhmmss
     * @return
     */
    public static String getSysDate(String sformat) {
        Date currentTime = new Date();
        SimpleDateFormat formatter = new SimpleDateFormat(sformat);
        String dateString = formatter.format(currentTime);
        return dateString;
    }

}
