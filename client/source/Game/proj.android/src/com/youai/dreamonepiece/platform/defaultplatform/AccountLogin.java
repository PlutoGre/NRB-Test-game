package com.youai.dreamonepiece.platform.defaultplatform;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.text.InputType;
import android.view.View;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import com.youai.LogUtils;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import org.json.JSONObject;

/** @brief 本地渠道账号窗口，提供服务端注册、登录、凭证恢复和切换账号。 */
public final class AccountLogin {
    private static final String TAG = "AccountLogin";
    private final Activity activity;
    private final Listener listener;
    private SharedPreferences preferences;
    private String base;
    private AlertDialog dialog;
    private boolean busy;
    private int generation;

    /** @brief 已通过服务器验证的账号结果回调。 */
    public interface Listener {
        /** 通知平台保存账号 UID、显示名与登录凭证。 */
        void accepted(String uid, String username, String token);
    }

    /** 使用当前 Activity 和平台回调创建窗口控制器。 */
    public AccountLogin(Activity activity, Listener listener) { this.activity = activity; this.listener = listener; }

    /** 从已加载的资源配置获取账号服务地址，不依赖盘符或固定局域网 IP。 */
    public static native String nativeAccountBaseURL();

    /** 有本地凭证时先向服务端恢复登录，否则显示账号密码窗口。 */
    public void login() {
        activity.runOnUiThread(new Runnable() { public void run() {
            if (busy || (dialog != null && dialog.isShowing())) return;
            base = nativeAccountBaseURL();
            if (base == null || !(base.startsWith("http://") || base.startsWith("https://"))) {
                LogUtils.w(TAG, "login: 账号服务地址尚未加载");
                new AlertDialog.Builder(activity).setMessage("请等待服务器配置加载完成后重试")
                        .setPositiveButton("确定", null).show();
                return;
            }
            // SharedPreferences 文件名不能包含 URL 中的斜线；按地址生成稳定命名空间。
            preferences = activity.getSharedPreferences("nrb_accounts_" + java.util.UUID.nameUUIDFromBytes(
                    base.getBytes(java.nio.charset.Charset.forName("UTF-8"))).toString(), Activity.MODE_PRIVATE);
            String token = preferences.getString("token", "");
            if (token.length() > 0) {
                JSONObject body = new JSONObject();
                try { body.put("token", token); } catch (Exception error) { LogUtils.e(TAG, "login: 请求生成失败", error); }
                request("resume", body, null, null, true);
            } else showForm(false, "");
        }});
    }

    /** 清除本机凭证并尽力撤销该设备会话；保留账号和服务端角色。 */
    public void logout() {
        activity.runOnUiThread(new Runnable() { public void run() {
            ++generation; busy = false;
            if (dialog != null) { dialog.dismiss(); dialog = null; }
            if (preferences == null) return;
            final String previous = preferences.getString("token", ""), endpoint = base;
            preferences.edit().remove("token").commit();
            if (previous.length() > 0) new Thread(new Runnable() { public void run() {
                try { JSONObject body = new JSONObject(); body.put("token", previous); exchange(endpoint, "logout", body); }
                catch (Exception error) { LogUtils.e(TAG, "logout: 服务端暂不可用，本地凭证已清除", error); }
            }}, "AccountLogout").start();
            LogUtils.i(TAG, "logout: 已清除本机登录凭证");
        }});
    }

    /** 切换时直接进入账号输入窗口，避免重新自动登录旧账号。 */
    public void switchAccount() {
        logout();
        activity.runOnUiThread(new Runnable() { public void run() {
            if (base == null || preferences == null) { login(); return; }
            showForm(false, "请输入要登录的账号");
        }});
    }

    /** Activity 结束时忽略在途结果，避免回调旧游戏实例。 */
    public void close() {
        activity.runOnUiThread(new Runnable() { public void run() {
            ++generation; if (dialog != null) dialog.dismiss(); dialog = null;
        }});
    }

    private EditText field(LinearLayout layout, String hint, boolean secret) {
        EditText input = new EditText(activity);
        input.setSingleLine(true); input.setHint(hint);
        input.setInputType(secret ? InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD
                : InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        input.setFilters(new android.text.InputFilter[] {new android.text.InputFilter.LengthFilter(secret ? 64 : 32)});
        layout.addView(input, new LinearLayout.LayoutParams(-1, -2));
        return input;
    }

    private void showForm(final boolean register, String message) {
        if (activity.isFinishing()) return;
        if (dialog != null) dialog.dismiss();
        LinearLayout layout = new LinearLayout(activity); layout.setOrientation(LinearLayout.VERTICAL);
        int padding = (int) (20 * activity.getResources().getDisplayMetrics().density);
        layout.setPadding(padding, padding / 2, padding, padding / 2);
        final EditText username = field(layout, "账号（3–32 位字母或数字）", false);
        username.setText(preferences.getString("username", ""));
        final EditText password = field(layout, "密码（6–64 个字符）", true);
        final EditText confirm = register ? field(layout, "确认密码", true) : null;
        final TextView error = new TextView(activity); error.setText(message); error.setTextColor(0xffb84030);
        layout.addView(error);
        ScrollView scroll = new ScrollView(activity); scroll.addView(layout);
        dialog = new AlertDialog.Builder(activity).setTitle(register ? "注册游戏账号" : "登录游戏账号")
                .setView(scroll).setCancelable(false).setPositiveButton(register ? "注册并登录" : "登录", null)
                .setNeutralButton(register ? "已有账号" : "注册账号", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface ignored, int which) { showForm(!register, ""); }
                }).setNegativeButton("取消", null).create();
        dialog.show();
        final AlertDialog current = dialog;
        current.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
            public void onClick(View ignored) {
                if (busy) return;
                String name = username.getText().toString().trim(), secret = password.getText().toString();
                if (!name.matches("[A-Za-z0-9]{3,32}")) { error.setText("账号须为 3–32 位英文字母或数字"); return; }
                if (secret.length() < 6 || secret.length() > 64) { error.setText("密码须为 6–64 个字符"); return; }
                if (register && !secret.equals(confirm.getText().toString())) { error.setText("两次输入的密码不一致"); return; }
                try {
                    JSONObject body = new JSONObject(); body.put("username", name); body.put("password", secret);
                    if (register) body.put("confirmPassword", confirm.getText().toString());
                    request(register ? "register" : "login", body, current, error, false);
                    password.setText(""); if (confirm != null) confirm.setText("");
                } catch (Exception failure) { LogUtils.e(TAG, "showForm: 请求生成失败", failure); error.setText("请求生成失败，请重试"); }
            }
        });
    }

    private void buttons(AlertDialog form, boolean enabled) {
        if (form == null) return;
        for (int button : new int[] {-1, -2, -3}) form.getButton(button).setEnabled(enabled);
    }

    private void request(final String action, final JSONObject body, final AlertDialog form, final TextView error, final boolean resume) {
        busy = true; buttons(form, false);
        if (error != null) error.setText("正在连接账号服务…");
        final int expected = generation;
        final String endpoint = base;
        LogUtils.d(TAG, "request: action=" + action);
        new Thread(new Runnable() { public void run() {
            JSONObject result = null; String message = null; boolean expired = false;
            try {
                result = exchange(endpoint, action, body);
                if (!result.optBoolean("ok")) { message = result.optString("message", "账号验证失败"); expired = result.optInt("httpStatus") == 401; }
                else if (!result.optString("uid").matches("acct_[a-f0-9]{32}") || !result.optString("token").matches("nrb1_[a-f0-9]{64}")) {
                    message = "服务端账号接口版本不兼容，请更新服务端";
                }
            } catch (Exception failure) { LogUtils.e(TAG, "request: 账号连接失败", failure); message = "无法连接账号服务，请确认服务端已启动后重试"; }
            final JSONObject accepted = result;
            final String failure = message; final boolean clear = expired;
            activity.runOnUiThread(new Runnable() { public void run() {
                if (expected != generation || activity.isFinishing()) return;
                busy = false; buttons(form, true);
                if (failure != null) {
                    if (clear) preferences.edit().remove("token").commit();
                    if (resume) showForm(false, failure); else if (error != null) error.setText(failure);
                    return;
                }
                String uid = accepted.optString("uid"), name = accepted.optString("username"), token = accepted.optString("token");
                preferences.edit().putString("username", name).putString("token", token).commit();
                if (dialog != null) dialog.dismiss(); dialog = null;
                LogUtils.i(TAG, "request: 服务端账号认证成功");
                listener.accepted(uid, name, token);
            }});
        }}, "GameAccount").start();
    }

    private static JSONObject exchange(String base, String action, JSONObject body) throws Exception {
        HttpURLConnection connection = (HttpURLConnection) new URL(base + "/account/v1/" + action).openConnection();
        try {
            connection.setConnectTimeout(12000); connection.setReadTimeout(15000);
            connection.setInstanceFollowRedirects(false); connection.setRequestMethod("POST"); connection.setDoOutput(true);
            connection.setRequestProperty("Content-Type", "application/json; charset=utf-8");
            byte[] data = body.toString().getBytes("UTF-8"); connection.setFixedLengthStreamingMode(data.length);
            OutputStream output = connection.getOutputStream(); try { output.write(data); } finally { output.close(); }
            int status = connection.getResponseCode();
            InputStream input = status >= 400 ? connection.getErrorStream() : connection.getInputStream();
            if (input == null) throw new java.io.IOException("Account endpoint returned no response");
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            try { byte[] buffer = new byte[1024]; int count;
                while ((count = input.read(buffer)) != -1) { if (bytes.size() + count > 8192) throw new java.io.IOException("Account response too large"); bytes.write(buffer, 0, count); }
            } finally { input.close(); }
            JSONObject result = new JSONObject(new String(bytes.toByteArray(), "UTF-8")); result.put("httpStatus", status);
            if (status != 200) result.put("ok", false);
            return result;
        } finally { connection.disconnect(); }
    }
}
