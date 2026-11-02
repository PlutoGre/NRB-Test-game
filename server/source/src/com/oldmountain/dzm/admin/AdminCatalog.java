package com.oldmountain.dzm.admin;

import java.io.InputStream;
import java.io.ByteArrayOutputStream;
import java.util.LinkedHashMap;
import java.util.Map;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import com.oldmountain.dzm.util.XMLTemplateService;

/** @brief 从当前运行服的 XML 模板生成完整发奖目录，名称取自发布资源表。 */
public final class AdminCatalog {
    private final Map<String, JSONObject> items = new LinkedHashMap<String, JSONObject>();
    private final JSONObject names;

    /**
     * 加载当前模板；客户端缺少名称时使用带模板 ID 的明确占位名称。
     * @throws Exception 资源或 XML 无法读取
     */
    public AdminCatalog() throws Exception {
        try (InputStream in = AdminCatalog.class.getResourceAsStream("/admin-names.json")) {
            if (in == null) throw new IllegalStateException("Missing administration item names");
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buffer = new byte[8192];
            int count;
            while ((count = in.read(buffer)) != -1) out.write(buffer, 0, count);
            names = JSONObject.fromObject(new String(out.toByteArray(), "UTF-8"));
        }
        add("property", 1001, "系统钻石", 100000000, "不增加累计充值或 VIP");
        add("property", 1002, "贝里", 100000000, "系统赠送货币");
        add("property", 1003, "海贼团经验", 100000000, "沿用游戏原有经验处理");
        load("disciple.xml", "Disciple", "id", "card");
        load("equip.xml", "Equip", "id", "equip");
        load("skill.xml", "Skill", "id", "skill");
        load("tools.xml", "Tool", "toolId", "tool");
        load("skillBookConfig.xml", "item", "id", "piece");
    }

    private void load(String file, String tag, String attribute, String kind) throws Exception {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setFeature("http://apache.org/xml/features/disallow-doctype-decl", true);
        NodeList rows = factory.newDocumentBuilder().parse("xml/" + file).getElementsByTagName(tag);
        for (int i = 0; i < rows.getLength(); i++) {
            Element row = (Element) rows.item(i);
            int id = Integer.parseInt(row.getAttribute(attribute));
            if (!valid(kind, id)) continue;
            String name = names.optString(kind + ":" + id, kind + " #" + id);
            String note = names.containsKey(kind + ":" + id) ? "" : "客户端资源表未收录，请先同步客户端模板；";
            int max = 1000000;
            if ("card".equals(kind)) {
                max = 10;
                note += "已拥有的完整卡牌按原品质规则转换为影子；含 NPC/隐藏模板";
                add("soul", id, name + "（影子）", 1000000, "含 NPC/隐藏模板");
            } else if ("equip".equals(kind) || "skill".equals(kind)) {
                max = 100;
            } else if ("piece".equals(kind)) {
                max = 1000;
                note += "奥义残章，进入残章页面查看";
            }
            add(kind, id, name, max, note);
        }
    }

    private void add(String kind, int id, String name, int max, String note) {
        JSONObject item = new JSONObject();
        item.put("key", kind + ":" + id);
        item.put("kind", kind);
        item.put("id", id);
        item.put("name", name);
        item.put("max", max);
        item.put("note", note);
        items.put(item.getString("key"), item);
    }

    /** 返回已加载的完整目录。 @return 可发放物品 */
    public JSONArray list() { return JSONArray.fromObject(items.values()); }

    /** 返回当前目录数量。 @return 条目数 */
    public int size() { return items.size(); }

    /**
     * 验证所选条目仍存在于内存模板，拒绝任意类别和未加载 ID。
     * @param key 类别和模板 ID
     * @param quantity 正整数数量
     * @return 标准化后的附件
     */
    public JSONObject attachment(String key, int quantity) {
        JSONObject item = items.get(key);
        if (item == null || !valid(item.getString("kind"), item.getInt("id"))) {
            throw new IllegalArgumentException("物品不存在或模板已更新，请重新登录刷新目录");
        }
        if (quantity < 1 || quantity > item.getInt("max")) {
            throw new IllegalArgumentException("数量必须在 1 到 " + item.getInt("max") + " 之间");
        }
        JSONObject result = new JSONObject();
        result.put("t", type(item.getString("kind")));
        result.put("i", item.getInt("id"));
        result.put("q", quantity);
        return result;
    }

    private static int type(String kind) {
        if ("property".equals(kind)) return 11001;
        if ("card".equals(kind)) return 31000;
        if ("soul".equals(kind)) return 32000;
        if ("equip".equals(kind)) return 51000;
        if ("skill".equals(kind)) return 41000;
        if ("piece".equals(kind)) return 42000;
        if ("tool".equals(kind)) return 61001;
        throw new IllegalArgumentException("不支持的物品类型");
    }

    private static boolean valid(String kind, int id) {
        if ("property".equals(kind)) return id >= 1001 && id <= 1003;
        if ("card".equals(kind)) return XMLTemplateService.getDiscipleTemplate(id) != null
                && XMLTemplateService.getSkillTemplate(XMLTemplateService.getDiscipleTemplate(id).getDefaultSkill()) != null;
        if ("soul".equals(kind)) return XMLTemplateService.getDiscipleTemplate(id) != null;
        if ("equip".equals(kind)) return XMLTemplateService.getEquipTemplate(id) != null;
        if ("skill".equals(kind)) return XMLTemplateService.getSkillTemplate(id) != null;
        if ("tool".equals(kind)) return XMLTemplateService.getToolTemplate(id) != null;
        if ("piece".equals(kind)) return XMLTemplateService.getSkillBookConfigById(id) != null;
        return false;
    }
}
