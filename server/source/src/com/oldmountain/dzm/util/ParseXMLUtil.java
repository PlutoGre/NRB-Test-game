package com.oldmountain.dzm.util;

import java.io.File;  
import java.io.FileInputStream;  
import java.io.FileNotFoundException;  
import java.util.ArrayList;  
import java.util.HashMap;  
import java.util.Iterator;  
import java.util.List;  
import java.util.Map;  
import java.util.Set;  
  
import org.dom4j.Document;  
import org.dom4j.Element;  
import org.dom4j.io.SAXReader; 

public class ParseXMLUtil {
	public Map entityMap;
	public Map columnMap;
	public Map ruleMap;
	public Map columnRulesMap;
	public Map columnListMap;
	public List columnList;
	
	public ParseXMLUtil(File xmlFilePath){
		FileInputStream in = null;  
        try {  
            if(xmlFilePath == null){  
                 throw new FileNotFoundException();  
            }  
            SAXReader reader = new SAXReader();                  
            in = new FileInputStream(xmlFilePath);                
            Document doc = reader.read(in);  
            Element root = doc.getRootElement();      
            Iterator itEntity = root.elements("entity").iterator();  
            while(itEntity.hasNext()){  
                Element entity = (Element) itEntity.next();  
                parseEntity(entity);  
            }  
              
            /**测试entityMap 是否正确**/  
            Map enMap = (Map) this.getEntityMap().get("用户表");  
            Set<?> set = enMap.keySet();  
            Iterator it = set.iterator();  
            while(it.hasNext()){  
                String uu = (String) it.next();  
                System.out.println("entity properties:"+uu+" = "+enMap.get(uu));  
            } 
        }catch(Exception e){  
            e.printStackTrace();  
        }    
    }
	
	public void parseEntity(Element entity){  
        if(entity != null){  
              
            /**对数据进行初始化设置**/  
            columnListMap = new HashMap();  
            columnMap = new HashMap();  
            entityMap = new HashMap();  
            ruleMap = new HashMap();  
            columnRulesMap = new HashMap();  
            columnList = new ArrayList();  
              
            setEntityMap(entity);             
            String entityName = entity.attributeValue("name");  
            Iterator itColumn = entity.elements("column").iterator();  
            while(itColumn.hasNext()){  
                Element column = (Element) itColumn.next();  
                setColumnMap(entityName,column);  
            }  
            columnListMap.put(entityName, columnList);  
        }  
    }
	
	public void setEntityMap(Element entity){         
        Map ent = new HashMap();  
        String name = entity.attributeValue("name");  
        String code = entity.attributeValue("code");  
        ent.put("name", name);  
        ent.put("code", code);  
        entityMap.put(name, ent);             
    }
	
	public void setColumnMap(String entityName,Element column){  
        if(column != null){       
            Map col = new HashMap();  
            String name = column.attributeValue("name");  
            String code = column.attributeValue("code");  
            String type = column.attributeValue("type");  
            col.put("name", name);  
            col.put("code", code);  
            col.put("type", type);  
            String columnMapKey = entityName+"_"+name;    //eg:  用户表_用户名  
            columnMap.put(columnMapKey, col);         
            columnList.add(col);  
            Iterator ruleIt = column.elements("rules").iterator();  //获得rules  
            while(ruleIt.hasNext()){  
                Element rules = (Element)ruleIt.next();   
                Iterator rule  = rules.elements("rule").iterator();   //获得 rule  
                while(rule.hasNext()){  
                    Element ruleValid = (Element) rule.next();     //获得每一行rule  
                    setRuleMap(entityName,name,ruleValid);                    
                }  
            }  
        }  
    }
	
	public void setRuleMap(String entityName,String columnName,Element ruleValid){  
        if(ruleValid != null){            
            String ruleName = ruleValid.attributeValue("name");  
            String ruleMsg = ruleValid.attributeValue("message");  
            Map ruleValidMap = new HashMap();  
            ruleValidMap.put("name", ruleName);  
            ruleValidMap.put("message", ruleMsg);  
            String ruleStrKey = entityName+"_"+columnName+"_"+ruleName;  
            String colStrKey = entityName+"_"+columnName;  
            if(this.getColumnRulesMap().containsKey(colStrKey)){  
                List valids = (List) this.getColumnRulesMap().get(colStrKey);  
                valids.add(ruleValidMap);  
            }else{  
                List valids = new ArrayList();  
                valids.add(ruleValidMap);  
                this.columnRulesMap.put(colStrKey, valids);  //将每个column下的所有rules存入该map中  
            }  
            ruleMap.put(ruleStrKey, ruleValidMap); //将每个column下的一条rule存入该map中  
        }  
    }
	
	public static void main(String[] args) {  
        File file = new File("src/user.xml");  
        new ParseXMLUtil(file);        
    }
	
	public Map getEntityMap() {  
        return entityMap;  
    }
	
	public void setEntityMap(Map entityMap) {  
        this.entityMap = entityMap;  
    }  
  
    public Map getColumnMap() {  
        return columnMap;  
    }  
  
    public void setColumnMap(Map columnMap) {  
        this.columnMap = columnMap;  
    }  
  
    public Map getRuleMap() {  
        return ruleMap;  
    }  
  
    public void setRuleMap(Map ruleMap) {  
        this.ruleMap = ruleMap;  
    }  
  
    public Map getColumnRulesMap() {  
        return columnRulesMap;  
    }  
  
    public void setColumnRulesMap(Map columnRulesMap) {  
        this.columnRulesMap = columnRulesMap;  
    }  
  
    public Map getColumnListMap() {  
        return columnListMap;  
    }  
  
    public void setColumnListMap(Map columnListMap) {  
        this.columnListMap = columnListMap;  
    }
}
