package com.utils.exportConfig;

import net.sf.json.JSONObject;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import java.io.*;
import java.util.*;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-2-20
 * Time: 下午6:46
 * To change this template use File | Settings | File Templates.
 */
public class FileManager
{
    private HashMap<Integer,HashMap<Integer,String>> sheetTitleMap=new HashMap<Integer,HashMap<Integer,String>>();
    public Workbook workBook;
    public int sheetSize;
    private String fileName;
    private HashMap<Integer,Integer> sheetRowMap =new HashMap<Integer,Integer>();
    private HashMap<Integer,Integer> sheetColMap =new HashMap<Integer,Integer>();
    private HashMap<Integer,HashMap<String,String>> excelValueMap=new HashMap<Integer,HashMap<String,String>>();
    public String getFilePath()
    {
        String url=System.getProperty("user.dir");
        return url;
    }
    /**
     * @param outputFilePath
     * @param list
     */
    public void writeToFile(String outputFilePath,List list)
    {
        try
        {
            File f = new File(outputFilePath);
            if (f.exists())
            {
                System.out.println(outputFilePath+"文件存在");
            }
            else
            {
                System.out.println("文件不存在，正在创建...");
                if (f.createNewFile())
                {
                    System.out.println("文件创建成功！");
                }
                else
                {
                    System.out.println("文件创建失败！");
                }
            }
            BufferedReader input = new BufferedReader(new FileReader(f));

            StringBuffer content=new StringBuffer();
            for(int i=0;i<list.size();i++)
            {
                content.append(list.get(i)+"\n");
            }
            input.close();

            BufferedWriter output = new BufferedWriter(new FileWriter(f));
            output.write(content.toString());
            output.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    /**
     * @param outputFilePath
     * @param js
     */
    public void writeToFile(String outputFilePath,JSONObject js)
    {
        try
        {
            File f = new File(getFilePath()+outputFilePath);
            if (f.exists())
            {
                copyFile(getFilePath()+outputFilePath);
            }
            else
            {
                if (f.createNewFile())
                {
                    System.out.println("FileManager.writeToFile:"+outputFilePath+" 文件创建成功！");
                }
                else
                {
                    System.out.println("FileManager.writeToFile:"+outputFilePath+" 文件创建失败！");
                }
            }
            BufferedReader input = new BufferedReader(new FileReader(f));
            input.close();

            BufferedWriter output = new BufferedWriter(new FileWriter(f));
            output.write(js.toString());
            output.close();
            System.out.println("FileManager.writeToFile:"+getFilePath()+outputFilePath+" 文件导出完成！");
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    public List readListFromTxtFile(String fileName)
    {
        fileName=fileName;
        File file = new File(fileName);
        BufferedReader reader = null;
        List urlList = new ArrayList();
        try
        {
            reader = new BufferedReader(new FileReader(file));
            String tempString = null;
            while ((tempString = reader.readLine()) != null)
            {
                urlList.add(tempString);
            }
            reader.close();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        finally
        {
            if (reader != null)
            {
                try
                {
                    reader.close();
                }
                catch (IOException ignored)
                {
                }
            }
        }
        return urlList;
    }

    public void copyFile(String fileName) throws IOException
    {
        FileInputStream fin = new FileInputStream(fileName);
        String foutName=fileName.substring(0,fileName.lastIndexOf("."))+System.currentTimeMillis()+fileName.substring(fileName.lastIndexOf("."),fileName.length());
        FileOutputStream fout = new FileOutputStream(new File(foutName));
        try
        {

            int bytesRead;
            byte[] buf = new byte[1024 * 1024];  // 4K
            while ((bytesRead = fin.read(buf)) != -1)
            {
                fout.write(buf, 0, bytesRead);
            }
            fout.flush();
            fout.close();
            fin.close();
            System.out.println("FileManager.writeToFile:"+fileName+" 文件备份成功！");
        }
        catch (Exception e)
        {
            System.out.println("FileManager.writeToFile:"+fileName+" 文件备份失败！");
        }

    }

    public StringBuffer readSBFromTxtFile(String fileName)
    {
        StringBuffer sb=new StringBuffer();
        fileName=fileName;
        File file = new File(getFilePath()+fileName);
        BufferedReader reader = null;
        try
        {
            reader = new BufferedReader(new FileReader(file));
            while(reader.ready())
            {
                sb.append((char)reader.read());
            }
            reader.close();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        finally
        {
            if (reader != null)
            {
                try
                {
                    reader.close();
                }
                catch (IOException ignored)
                {
                }
            }
        }
        return sb;
    }

    public Map jsonToMap(StringBuffer sb)
    {
        JSONObject jb = JSONObject.fromObject(sb.toString());
        Iterator keyIter = jb.keys();
        String key;
        Object value;
        Map valueMap = new HashMap();
        while (keyIter.hasNext()) {
            key = (String)keyIter.next();
            value = jb.get(key);
            valueMap.put(key, value);
        }
        return valueMap;
    }

    public Workbook readFromXlsFile(String fileName)
    {
        try
        {
            if(fileName == null)
            {
                throw new FileNotFoundException();
            }
            fileName=fileName;
            FileInputStream  fis = new FileInputStream(getFilePath()+fileName);
            if(fileName.indexOf("xlsx")==fileName.length()-4)
            {
                workBook = new XSSFWorkbook(fis);
            }
            else
            {
                workBook = new HSSFWorkbook(fis);
            }
            loadSheet();
        }
        catch  (Exception e)
        {
            System.out.println("FileManager.rewardFromXlsFile catch: "+ e);
        }
        return workBook;
    }

    public int getRowNumBySheetIndex(int sheetIndex)
    {
        int row=-1;
        try
        {
            row= sheetRowMap.get(sheetIndex);
        }
        catch (Exception e)
        {
            row=-1;
        }
        return row;
    }

    public int getColNumBySheetIndex(int sheetIndex)
    {
        int col=-1;
        try
        {
            col= sheetColMap.get(sheetIndex);
        }
        catch (Exception e)
        {
            col=-1;
        }
        return col;
    }

    public String getCellValueBySheetIndexAndCoordinate(int sheetIndex,int row,int col)
    {
        String v=null;
        try
        {
            v= excelValueMap.get(sheetIndex).get(row + "_" + col);
        }
        catch (Exception e)
        {
            v=null;
        }
        return v;
    }

    public String getCellTitleBySheetIndexAndCol(int sheetIndex,int col)
    {
        String v=null;
        try
        {
            v=sheetTitleMap.get(sheetIndex).get(col);
        }
        catch (Exception e)
        {
            v=null;
        }
        return v;
    }


    private void loadSheet()
    {
        sheetSize = workBook.getNumberOfSheets();
        Sheet sheet;
        for (int i = 0; i < sheetSize; i++)
        {
            sheet = workBook.getSheetAt(i);
            readSheetData(sheet, i);
        }
    }

    /**
     * 读每个sheet页的数据*
     */
    private void readSheetData(Sheet sheet,int sheetIndex)
    {
        int rowNumbers = sheet.getPhysicalNumberOfRows();
        if (rowNumbers == 0)
        {
            System.out.println("FileManager file:"+fileName+" 中数据为空！");
        }
        readSheetHeadData(sheet,sheetIndex);
        readSheetColumnData(sheet, sheetIndex);
    }

    /**
     * 读取sheet页中的表头信息*
     */
    @SuppressWarnings({"unchecked", "static-access"})
    private void readSheetHeadData(Sheet sheet,int sheetIndex)
    {
        Row excelheadRow = sheet.getRow(0);
        int excelLastRow = excelheadRow.getLastCellNum();
        String headTitle = "";
        HashMap titleMap=new HashMap();
        for (int i = 0; i < excelLastRow; i++)
        {
            Cell cell = excelheadRow.getCell(i);
            headTitle = getStringCellValue(cell).trim();
            titleMap.put(i, headTitle);
        }
        sheetTitleMap.put(sheetIndex, titleMap);
    }

    /**
     * 读取sheet页里面的数据*
     */
    @SuppressWarnings({"unchecked", "static-access"})
    private void readSheetColumnData(Sheet sheet, int sheetIndex)
    {
        Row excelheadRow = sheet.getRow(0);
        int excelRowNum = sheet.getLastRowNum();  //excel总行数
        int excelLastcell = excelheadRow.getLastCellNum();   //excel总列数
        sheetRowMap.put(sheetIndex,excelRowNum);
        sheetColMap.put(sheetIndex,excelLastcell);
        HashMap<String,String> cellMap=new HashMap<String,String>();
        for (int i = 1; i < excelRowNum + 1; i++)
        {//行循环
            Row columnRow = sheet.getRow(i);
            if (columnRow != null)
            {
                for (int j = 0; j < excelLastcell; j++)
                { //列循环
                    Cell colCell = columnRow.getCell(j);
                    String value = getStringCellValue(colCell);
                    if (value != null)
                    {
                        value = value.trim();
                    }
                    cellMap.put((i-1)+"_"+j,value);
                }
            }
        }
        excelValueMap.put(sheetIndex,cellMap);
    }
    /**
     * 获得单元格字符串
     */
    public String getStringCellValue(Cell cell)
    {
        if (cell == null)
        {
            return null;
        }

        String result = "";
        switch (cell.getCellType())
        {
            case Cell.CELL_TYPE_BOOLEAN:
                result = String.valueOf(cell.getBooleanCellValue());
                break;
            case Cell.CELL_TYPE_NUMERIC:
                if (DateUtil.isCellDateFormatted(cell))
                {
                    java.text.SimpleDateFormat TIME_FORMATTER = new java.text.SimpleDateFormat(
                            "yyyy-MM-dd");
                    result = TIME_FORMATTER.format(cell.getDateCellValue());
                }
                else
                {
                    String doubleValue = String.valueOf(cell.getNumericCellValue());
                    result = "" + doubleValue;
                    if(result.indexOf(".0")==result.length()-2)
                    {
                        result=result.substring(0,result.length()-2);
                    }
                }
                break;
            case Cell.CELL_TYPE_STRING:
                if (cell.getRichStringCellValue() == null)
                {
                    result = null;
                }
                else
                {
                    result = cell.getRichStringCellValue().getString();
                }
                break;
            case Cell.CELL_TYPE_BLANK:
                result = null;
                break;
            case Cell.CELL_TYPE_FORMULA:
                try
                {
                    result = String.valueOf(cell.getNumericCellValue());
                }
                catch (Exception e)
                {
                    result = cell.getRichStringCellValue().getString();
                }
                break;
            default:
                result = "";
        }
        return result;
    }
}
