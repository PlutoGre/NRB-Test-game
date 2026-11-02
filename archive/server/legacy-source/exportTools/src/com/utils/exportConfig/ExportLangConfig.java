package com.utils.exportConfig;

import net.sf.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-2-22
 * Time: 上午10:14
 * To change this template use File | Settings | File Templates.
 */
public class ExportLangConfig
{
    public int version;
    private StringBuffer sb =new StringBuffer();
    private FileManager fileManager;
    public JSONObject jsonData;
    private String fileName;
    public ExportLangConfig()
    {
        fileManager=new FileManager();
    }
    public void loadLangConfig(String fileName)
    {
        fileName=fileName;
        sb =fileManager.readSBFromTxtFile(fileName);
        jsonData=JSONObject.fromObject(sb.toString());
    }

    public void exportLangConfig(String outFile,Map m)
    {
        Iterator iteratorArray=jsonData.getJSONArray("strings").iterator();
        int i=0;
        while(iteratorArray.hasNext())
        {
            JSONObject it=(JSONObject) iteratorArray.next();
            if(m.containsKey(it.get("k")))
            {
                jsonData.getJSONArray("strings").getJSONObject(i).element("v", (String) m.get(it.get("k")));
                m.remove(it.get("k"));
            }
            i++;
        }
        Iterator iter = m.entrySet().iterator();
        while(iter.hasNext())
        {
            Map.Entry entry = (Map.Entry) iter.next();
            Object key = entry.getKey();
            Object val = entry.getValue();
            JSONObject json=new JSONObject();
            json.put("k",key);
            json.put("v",val);
            jsonData.getJSONArray("strings").add(json);
        }
        fileManager.writeToFile(outFile,jsonData);
    }
}
