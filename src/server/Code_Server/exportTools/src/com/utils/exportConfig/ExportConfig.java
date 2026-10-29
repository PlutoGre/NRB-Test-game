package com.utils.exportConfig;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-2-22
 * Time: 下午12:54
 * To change this template use File | Settings | File Templates.
 */
public class ExportConfig
{
    public static void main(String args[])
    {
        ExportCareerConfig careerConfig=new ExportCareerConfig();
        careerConfig.loadExportCareerConfig();
    }
}
