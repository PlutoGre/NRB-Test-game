package main;

import java.awt.Checkbox;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Random;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;

import org.apache.commons.lang.StringUtils;

import util.ExcelUtil;
import util.CdkeyUtil;

import dao.CdkeyDao;

import entity.Cdkey;

public class Main {

	/**
	 * 生成随机字符串
	 */
	private JFrame jFrame;
	private JPanel jp1;
	private JPanel jp2;
	private JButton jb1;
	private JButton jb2;
	private JTextField tf1, tf2, tf3, tf4, tf5, tf6 ;
	private JLabel j1;
	private JLabel j2;
	private JLabel j3;
	private JLabel j4;
	private JLabel j5;
	private JLabel j6;
	private JLabel j7;
	private JLabel j8;
	private JLabel j9;
	private JRadioButton jr;

	public Main() {
		jFrame = new JFrame("cdkey生成工具");
		jp1 = new JPanel();
		jp2 = new JPanel();
		jb1 = new JButton("生成(s)");
		jb2 = new JButton("导出(e)");
		tf1 = new JTextField(5);
		tf2 = new JTextField(3);
		tf3 = new JTextField(3);
		tf4 = new JTextField(20);
		tf5 = new JTextField(3);
		tf6 = new JTextField(3);
		j1 = new JLabel("数量:");
		j2 = new JLabel("    类型:");
		j3 = new JLabel("    渠道:");
		j4 = new JLabel("    操作结果:");
		j5 = new JLabel("   ");
		j6 = new JLabel("    已使用:");
		j7 = new JLabel("");
		
		j8 = new JLabel("查询     类型:");
		j9 = new JLabel("    渠道:");
		
		jr =new JRadioButton();
		
		ProduceActionListener mal1 = new ProduceActionListener();
		ExportExcelActionListener  mal2= new ExportExcelActionListener();
		jp1.add(j1);
		jp1.add(tf1);
		jp1.add(j2);
		jp1.add(tf2);
		jp1.add(j3);
		jp1.add(tf3);
		jp1.add(j5);
		jp1.add(jb1);
		
		jp1.add(j8);
		jp1.add(tf5);
		jp1.add(j9);
		jp1.add(tf6);
		jp1.add(j6);
		jp1.add(jr);
		jp1.add(jb2);
		
		jp1.add(j7);
		jp1.add(j4);
		jp1.add(tf4);
		jFrame.add(jp1);
		jb1.setMnemonic('s');
		jb1.addActionListener(mal1);
		jb2.setMnemonic('e');
		jb2.addActionListener(mal2);

		jFrame.pack();
		int w = jFrame.getToolkit().getScreenSize().width;// 宽度
		int h = jFrame.getToolkit().getScreenSize().height;// 高度
		jFrame.setSize(400, 150);
		jFrame.setLocation(w / 2 - 200, h / 2 - 30);
		jFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jFrame.setVisible(true);
	}
	public static void main(String[] args) {
		new Main();
	}
	class ProduceActionListener implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent e) {
			int start_time=(int)(Math.floor(System.currentTimeMillis()/1000));
			if (e.getSource().equals(jb1)) 
			{
				try 
				{
					int count = Integer.parseInt(tf1.getText().trim());
					String type =tf2.getText().replace(" ", "").substring(0, 2).toUpperCase();
					int channel = Integer.parseInt(tf3.getText().replace(" ","").substring(0, 2));
					List<Cdkey> cdkeys = CdkeyUtil.createCdkeysList(count, type, channel);
					CdkeyDao dao=new CdkeyDao();
					dao.saveList(cdkeys);
					int finish_time=(int)(Math.floor(System.currentTimeMillis()/1000));
					int cost_time=finish_time-start_time;
					tf4.setText("操作成功,耗时: "+cost_time+"s");
				} 
				catch (Exception x) 
				{
					JOptionPane.showMessageDialog(null, "渠道或类型错误");
					tf4.setText("生成失败");
				}
			}
		}
	}
	class ExportExcelActionListener implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent e) {
			int start_time=(int)(Math.floor(System.currentTimeMillis()/1000));
			if (e.getSource().equals(jb2)) 
			{
				try 
				{
					String name="ccc";
					String type="";
					String channel="";
					if(StringUtils.isNotBlank(tf5.getText())){
						type = tf5.getText().replace(" ", "").substring(0, 2).toUpperCase();
					}
					System.out.println(jr.isSelected());
					if(StringUtils.isNotBlank(tf6.getText())){
						channel = tf6.getText().replace(" ","").substring(0, 2);
					}
					String status=(jr.isSelected())?"1":"0";
					String condition=CdkeyUtil.createCondition(type, channel, status);
					CdkeyDao dao=new CdkeyDao();
					List<Cdkey> list=dao.queryWithCondition(condition);
					ExcelUtil.exportExcel(list, name ,type);
					
					
					int finish_time=(int)(Math.floor(System.currentTimeMillis()/1000));
					int cost_time=finish_time-start_time;
					tf4.setText("操作成功,耗时: "+cost_time+"s"+"    "+"数量"+list.size());
				} 
				catch (Exception x) 
				{
					JOptionPane.showMessageDialog(null, "查询条件错误");
					tf4.setText("查询失败");
				}
			}
		}
	}
}