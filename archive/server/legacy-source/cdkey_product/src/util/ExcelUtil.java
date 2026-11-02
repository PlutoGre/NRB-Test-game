package util;

import java.io.File;
import java.io.FileOutputStream;
import java.util.List;

import org.apache.poi.hssf.usermodel.HSSFCell;
import org.apache.poi.hssf.usermodel.HSSFCellStyle;
import org.apache.poi.hssf.usermodel.HSSFFont;
import org.apache.poi.hssf.usermodel.HSSFRow;
import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.hssf.util.Region;

import entity.Cdkey;

public class ExcelUtil {
	public static boolean exportExcel(List<Cdkey> list,String name , String type) throws Exception{
		boolean success=true;
		String strFilePath = "D:\\";
		String strFileName = type+(int)(System.currentTimeMillis()/1000)+ ".xls";
		try {
			if (!(new File(strFilePath).isDirectory())) {
				new File(strFilePath).mkdirs();
			}

			HSSFWorkbook hwb= new HSSFWorkbook();
			HSSFSheet sheet=hwb.createSheet("cdkey");
			sheet.setColumnWidth((short)0,(short)6000);
			sheet.setColumnWidth((short)1,(short)4500);
			sheet.setColumnWidth((short)2,(short)4500);
			sheet.setColumnWidth((short)3,(short)4000);
			sheet.setColumnWidth((short)4,(short)2000);
			/************************************************************************/
			for (int index = 0; index <=list.size(); index++) {
				// 在索引0的位置创建行（第一行：sheet.createRow((short) 0);）:从index判断行数
				HSSFRow row = sheet.createRow((short) index);
				Cdkey cdkey=null;
				if (index == 0) {
					row.setHeightInPoints((short) 40);// 设置ex单元格的高度
				} else {
					row.setHeightInPoints((short) 20);// 设置ex单元格的高度
					cdkey=list.get(index-1);
				}
				//列
				for (int i = 0; i < 5; i++) 
				{
					// 在索引0的位置创建单元格（第一列从i中片段列数多少或者说应用格式
					HSSFCell cell = row.createCell((short) i);
					cell.setEncoding(HSSFCell.ENCODING_UTF_16);// 中文编码
					HSSFCellStyle style = hwb.createCellStyle();
					String strValue = "";
					if (index == 0) 
					{
						// 创建字体，设置其为红色、粗体：
						HSSFFont font = hwb.createFont();
						font.setBoldweight(HSSFFont.BOLDWEIGHT_BOLD);
						font.setFontHeightInPoints((short) 8);

						style.setAlignment((short) 16);
						style.setBorderBottom((short) 16);
						style.setFont(font);
						style.setAlignment(HSSFCellStyle.ALIGN_CENTER);
						style.setVerticalAlignment(HSSFCellStyle.VERTICAL_CENTER);
						cell.setCellStyle(style);
						switch (i) {
						case 0:
							strValue="Cdkey";
							break;
						case 1:
							strValue="创建时间";
							break;
						case 2:
							strValue="激活时间";
							break;
						case 3:
							strValue="使用玩家id";
							break;
						case 4:
							strValue="激活码状态";
							break;
						default:
							break;
						}
						cell.setCellValue(strValue);
					} 
					else  
					{
						switch (i) {
						case 0:
							strValue=cdkey.getCd_key();
							break;
						case 1:
							strValue=""+cdkey.getCreate_time();
							break;
						case 2:
							strValue=""+cdkey.getUsed_time();
							break;
						case 3:
							strValue=""+cdkey.getPlayerid();
						case 4:
							strValue=""+cdkey.getStatus();
							break;
						default:
							break;
						}
							cell.setCellValue(strValue);
					}
				}
			
			}
			
			/************************************************************************/
			strFilePath = strFilePath + strFileName;
			File fTarget = new File(strFilePath);
			// 新建一输出文件流
			FileOutputStream fileOut = new FileOutputStream(fTarget);
			//		 把相应的Excel 工作簿存盘
			hwb.write(fileOut);
			//		 操作结束，关闭文件
			fileOut.close();
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		return success;
	}
}
