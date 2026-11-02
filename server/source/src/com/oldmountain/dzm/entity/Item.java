package com.oldmountain.dzm.entity;

public class Item {
	private int templateId;
	private ItemXmlTemplate itemTemplate;
	
	public int getTemplateId() {
		return templateId;
	}
	public void setTemplateId(int templateId) {
		this.templateId = templateId;
	}
	public ItemXmlTemplate getItemTemplate() {
		return itemTemplate;
	}
	public void setItemTemplate(ItemXmlTemplate itemTemplate) {
		this.itemTemplate = itemTemplate;
	}
	
	
}
