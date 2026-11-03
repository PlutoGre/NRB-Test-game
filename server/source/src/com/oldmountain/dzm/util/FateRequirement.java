package com.oldmountain.dzm.util;

import java.util.Collection;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/** @brief 解析缘分条件：下划线连接必需组，井号连接组内可选模板。 */
public final class FateRequirement {
    private static final Logger LOG = LoggerFactory.getLogger(FateRequirement.class);
    private static final ConcurrentMap<String, int[][]> CACHE = new ConcurrentHashMap<String, int[][]>();
    private static final int[][] INVALID = new int[0][];

    private FateRequirement() { }

    /**
     * 对每个必需组至少匹配一个模板，兼容原有单 ID 和下划线条件。
     * 非法配置不激活缘分，并在首次解析时记录警告，避免战斗循环重复刷日志。
     * @param expression XML 中的缘分条件，例如 41#210_17#217
     * @param owned 当前阵容、已穿装备或已带奥义的模板 ID，沿用调用方原有范围
     * @return 所有必需组均已满足时返回 true
     */
    public static boolean matches(String expression, Collection<Integer> owned) {
        if (expression == null || owned == null) return false;
        int[][] groups = CACHE.get(expression);
        if (groups == null) {
            groups = parse(expression);
            int[][] previous = CACHE.putIfAbsent(expression, groups);
            if (previous != null) groups = previous;
            else if (groups == INVALID) LOG.warn("缘分条件格式无效，已禁用该条件: {}", expression);
        }
        if (groups.length == 0) return false;
        for (int[] alternatives : groups) {
            boolean found = false;
            for (int id : alternatives) {
                if (owned.contains(id)) { found = true; break; }
            }
            if (!found) return false;
        }
        return true;
    }

    /**
     * 将整个表达式转换为必需组；任意空项、非整数或非正数均视为无效。
     * @param expression XML 中的完整条件
     * @return 条件分组；格式错误时返回空数组标记
     */
    private static int[][] parse(String expression) {
        String[] required = expression.split("_", -1);
        int[][] result = new int[required.length][];
        try {
            for (int i = 0; i < required.length; i++) {
                String[] alternatives = required[i].split("#", -1);
                result[i] = new int[alternatives.length];
                for (int j = 0; j < alternatives.length; j++) {
                    if (!alternatives[j].matches("[0-9]+")) return INVALID;
                    result[i][j] = Integer.parseInt(alternatives[j]);
                    if (result[i][j] <= 0) return INVALID;
                }
            }
        } catch (NumberFormatException e) {
            return INVALID;
        }
        return result;
    }
}
