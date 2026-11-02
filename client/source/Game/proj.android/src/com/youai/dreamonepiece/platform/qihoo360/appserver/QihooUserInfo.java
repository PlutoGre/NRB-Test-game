
package com.youai.dreamonepiece.platform.qihoo360.appserver;

import org.json.JSONException;
import org.json.JSONObject;

import android.text.TextUtils;

/**
 * QihooUserInfo，是应用服务器*/
public class QihooUserInfo {

    private String id; // 360用户ID，缺省返回

    private String name; // 360用户名，缺省返回
    private String avatar; // 360用户头像url，缺省返回

    private String sex; // 360用户性别，仅在fields中包含时候才返回

    private String area; // 360用户地区，仅在fields中包含时候才返回
    private String nick; // 360用户昵称，无值时候返回空
    /***
     */
    public static QihooUserInfo parseJson(String jsonString) {
        QihooUserInfo userInfo = null;
        if (!TextUtils.isEmpty(jsonString)) {
            try {
                JSONObject dataJsonObj = new JSONObject(jsonString);
                //String status = jsonObj.getString("status");
                //JSONObject dataJsonObj = jsonObj.getJSONObject("data");
                //if (status != null && status.equals("ok")) {
                    // 必返回项
                	/*
                     * 直接从https://openapi.360.cn/user/me.json
                     * 这里取的json，格式只是data那样
                     * */
                    String id = dataJsonObj.getString("id");
                    String name = dataJsonObj.getString("name");
                    String avatar = dataJsonObj.getString("avatar");

                    userInfo = new QihooUserInfo();
                    userInfo.setId(id);
                    userInfo.setName(name);
                    userInfo.setAvatar(avatar);

                    // 非必返回
                    if (dataJsonObj.has("sex")) {
                        String sex = dataJsonObj.getString("sex");
                        userInfo.setSex(sex);
                    }

                    if (dataJsonObj.has("area")) {
                        String area = dataJsonObj.getString("area");

                        userInfo.setArea(area);
                    }

                    if (dataJsonObj.has("nick")) {
                        String nick = dataJsonObj.getString("nick");
                        userInfo.setNick(nick);
                    }
                //}
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        return userInfo;
    }

    public boolean isValid() {
        return !TextUtils.isEmpty(id);
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getAvatar() {
        return avatar;
    }

    public void setAvatar(String avatar) {
        this.avatar = avatar;
    }

    public String getSex() {
        return sex;
    }

    public void setSex(String sex) {
        this.sex = sex;
    }

    public String getArea() {
        return area;
    }

    public void setArea(String area) {
        this.area = area;
    }

    public String getNick() {
        return nick;
    }

    public void setNick(String nick) {
        this.nick = nick;
    }
    
    public String toJsonString() {
        JSONObject obj = new JSONObject();
        try {
            obj.put("status", "ok");
            
            JSONObject dataObj = new JSONObject();
            dataObj.put("id", id);
            dataObj.put("name", name);
            dataObj.put("avatar", avatar);
            dataObj.put("sex", sex);
            dataObj.put("area", area);
            dataObj.put("nick", nick);
            
            obj.put("data", dataObj);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return obj.toString();
    }

}
