package com.utils.exportConfig;

import java.util.HashMap;
import java.util.Map;

import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-2-20
 * Time: 下午6:34
 * To change this template use File | Settings | File Templates.
 */
public class ExportCareerConfig
{
    public FileManager fileManager;
    public HashMap xlsConfigMap;
    public int version=1;
    public final String BLANK="    ";
    public ExportLangConfig langConfig;
    private Map langMap;
    private JSONObject careerConfigData;
    private String readFileName;
    private String outFilePath;
    private String langFile;

    public ExportCareerConfig()
    {
        fileManager = new FileManager();
        readFileName = "/../../../../Product/策划案/江湖配置文件.xlsx";
        outFilePath = "/../../client/Game/Resources/CareerConfig.cfg";
        langFile = "/../../client/Game/Resources/Lang/chinese.lang";
        langMap=new HashMap();
        careerConfigData =new JSONObject();
    }

    public void loadExportCareerConfig()
    {
        loadXlsConfigMap();
        loadLangConfig();
        export();
    }

    private  void loadLangConfig()
    {
        langConfig=new ExportLangConfig();
        langConfig.loadLangConfig(langFile);
        langConfig.exportLangConfig(langFile,langMap);
    }

    private  void loadXlsConfigMap()
    {
        fileManager.readFromXlsFile(readFileName);
        int sheetSize=fileManager.sheetSize;
        careerConfigData.put("version", version);
        JSONObject chapterJson=new JSONObject();
        int chapterRow=fileManager.getRowNumBySheetIndex(0);
        int chapterCol=fileManager.getColNumBySheetIndex(0);
        for(int m=0;m<chapterRow;m++)
        {
            StringBuffer chapterSb=new StringBuffer();
            int chapterId = 0;
            String chapterName="";
            String chapterPlot="";
            for(int n=0;n<chapterCol;n++)
            {
               switch (n)
               {
                   case 0:
                   chapterId= Integer.parseInt((String)fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n));
                       chapterJson.put("id", chapterId);
                   break;
                   case 1:
                       chapterName="@"+fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n);
                       chapterJson.put("name",chapterName);
                       break;
                   case 2:
                       langMap.put(chapterName,fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n));
                       break;
                   case 3:
                       chapterPlot="@"+fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n);
                       chapterJson.put("plot",chapterPlot);
                       break;
                   case 4:
                       langMap.put(chapterPlot,fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n));
                       break;
                   case 5:
                       chapterJson.put("limitLevel",fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n));
                       break;
                   case 6:
                       chapterJson.put("isOpen",fileManager.getCellValueBySheetIndexAndCoordinate(0,m,n));
                       break;
               }
            }
            int customsRow=fileManager.getRowNumBySheetIndex(chapterId);
            int customsCol=fileManager.getColNumBySheetIndex(chapterId);
            for(int h=0;h<customsRow;h++)
            {
                StringBuffer customsSb=new StringBuffer();
                String customName="";
                String customPlot="";
                JSONObject customsJson=new JSONObject();
                for(int k=0;k<customsCol;k++)
                {
                    switch (k)
                    {
                        case 0:
                            customsJson.put("chaptersId",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 1:
                            customsJson.put("id",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 2:
                            customName="@"+fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k);
                            customsJson.put("name",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 3:
                            langMap.put(customName,fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 4:
                            customsJson.put("icon",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 5:
                            customsJson.put("monsterType",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 6:
                            customPlot="@"+fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k);
                            customsJson.put("plot",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 7:
                            langMap.put(customPlot,fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 8:
                            customsJson.put("consumeStrength",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 9:
                            customsJson.put("limitAttackNum",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 10:
                            customsJson.put("suggestLevel",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 11:
                            customsJson.put("winRewardExp",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 12:
                            customsJson.put("winRewardCoins",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                        case 13:
                            customsJson.put("props",fileManager.getCellValueBySheetIndexAndCoordinate(chapterId,h,k));
                            break;
                    }

                }
                chapterJson.accumulate("customsPass",customsJson);
            }
            careerConfigData.accumulate("chapters", chapterJson);
        }
    }

    private void export()
    {
        fileManager.writeToFile(outFilePath,careerConfigData);
    }

}
