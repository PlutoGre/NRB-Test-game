
package com.youai.dreamonepiece.platform.punchbox;

public interface SdkHttpListener {

    public void onResponse(String response);

    public void onCancelled();

    public void onError();
}
