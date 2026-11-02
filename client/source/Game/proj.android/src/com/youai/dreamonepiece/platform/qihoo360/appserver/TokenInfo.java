
package com.youai.dreamonepiece.platform.qihoo360.appserver;

import org.json.JSONException;
import org.json.JSONObject;

import android.text.TextUtils;

public class TokenInfo {

    private String accessToken; // Access
    private Long expiresIn; // Access 
    private String scope; // Access 

    private String refreshToken; //
    /***
     */
    public static TokenInfo parseJson(String jsonString) {
        TokenInfo tokenInfo = null;
        if (!TextUtils.isEmpty(jsonString)) {
            try {
                JSONObject dataJsonObj = new JSONObject(jsonString);
                //String status = jsonObj.getString("status");
                //JSONObject dataJsonObj = jsonObj.getJSONObject("data");
                //if (status != null && status.equals("ok")) {
                    //
                /*
                 * https://openapi.360.cn/oauth2/access_token?grant_type=authorization_code
                 *
                 * */
                    String accessToken = dataJsonObj.getString("access_token");
                    Long expiresIn = dataJsonObj.getLong("expires_in");
                    String scope = dataJsonObj.getString("scope");
                    String refreshToken = dataJsonObj.getString("refresh_token");

                    tokenInfo = new TokenInfo();
                    tokenInfo.setAccessToken(accessToken);
                    tokenInfo.setExpiresIn(expiresIn);
                    tokenInfo.setScope(scope);
                    tokenInfo.setRefreshToken(refreshToken);
                //}
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        return tokenInfo;
    }

    public String getAccessToken() {
        return accessToken;
    }

    public void setAccessToken(String accessToken) {
        this.accessToken = accessToken;
    }

    public Long getExpiresIn() {
        return expiresIn;
    }

    public void setExpiresIn(Long expiresIn) {
        this.expiresIn = expiresIn;
    }

    public String getScope() {
        return scope;
    }

    public void setScope(String scope) {
        this.scope = scope;
    }

    public String getRefreshToken() {
        return refreshToken;
    }

    public void setRefreshToken(String refreshToken) {
        this.refreshToken = refreshToken;
    }
    
    public String toJsonString() {

        JSONObject obj = new JSONObject();
        try {
            obj.put("status", "ok");

            JSONObject dataObj = new JSONObject();
            dataObj.put("access_token", accessToken);
            dataObj.put("expires_in", expiresIn);
            dataObj.put("scope", scope);
            dataObj.put("refresh_token", refreshToken);

            obj.put("data", dataObj);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return obj.toString();
    }

}
