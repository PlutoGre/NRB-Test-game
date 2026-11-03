package com.oldmountain.dzm.admin;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.Locale;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import net.sf.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/** @brief 回环地址上的账号 API，由公开资源入口仅转发四种账号操作。 */
public final class AccountHandler implements HttpHandler {
    private static final Logger LOG = LoggerFactory.getLogger(AccountHandler.class);

    /** 读取受限 JSON；不记录账号密码或凭证。 @param exchange 本机代理请求 @throws IOException 连接错误 */
    public void handle(HttpExchange exchange) throws IOException {
        int status = 200;
        JSONObject result;
        try {
            if (!exchange.getRemoteAddress().getAddress().isLoopbackAddress()) throw new SecurityException("禁止访问");
            String route = exchange.getRequestURI().getPath().substring("/account/v1/".length());
            if (!route.matches("login|register|resume|logout") || !"POST".equals(exchange.getRequestMethod())
                    || exchange.getRequestURI().getRawQuery() != null) throw new IllegalArgumentException("账号接口仅支持 JSON POST");
            String contentType = exchange.getRequestHeaders().getFirst("Content-Type");
            if (contentType == null || !contentType.toLowerCase(Locale.ROOT).startsWith("application/json")) {
                throw new IllegalArgumentException("请求必须使用 application/json");
            }
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            try (InputStream in = exchange.getRequestBody()) {
                byte[] buffer = new byte[1024]; int count;
                while ((count = in.read(buffer)) != -1) {
                    if (bytes.size() + count > 4096) throw new IllegalArgumentException("账号请求过长");
                    bytes.write(buffer, 0, count);
                }
            }
            result = AccountService.dispatch(route, JSONObject.fromObject(new String(bytes.toByteArray(), StandardCharsets.UTF_8)));
        } catch (Exception error) {
            status = error instanceof SecurityException ? 401 : error instanceof IllegalArgumentException
                    || error instanceof net.sf.json.JSONException ? 400 : 503;
            result = new JSONObject(); result.put("ok", false);
            result.put("message", status == 503 ? "账号服务不可用，请确认游戏服务端已启动后重试"
                    : error instanceof net.sf.json.JSONException ? "账号请求格式错误" : error.getMessage());
            if (status == 503) LOG.error("handle: account request failed", error);
            else LOG.warn("handle: account request rejected, status={}", status);
        }
        byte[] output = result.toString().getBytes(StandardCharsets.UTF_8);
        exchange.getResponseHeaders().set("Content-Type", "application/json; charset=utf-8");
        exchange.getResponseHeaders().set("Cache-Control", "no-store");
        exchange.sendResponseHeaders(status, output.length);
        try (OutputStream stream = exchange.getResponseBody()) { stream.write(output); }
        finally { exchange.close(); }
    }
}
