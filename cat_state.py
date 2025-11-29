from typing import Dict


class CatStateEngine:
    """非常简化的「行为引擎」。

    这里只是根据 24 小时涨跌幅和成交量，把市场粗略地映射为
    CALM / ALERT / CAUTIOUS / PANIC 四种状态，用来驱动实体猫的表现。
    """

    def compute_state(self, snapshot: Dict) -> Dict:
        change = snapshot.get("price_change_percent", 0.0)
        volume = snapshot.get("volume", 0.0)

        if abs(change) < 1.0:
            state = "CALM"
        elif abs(change) < 5.0:
            state = "ALERT"
        elif abs(change) < 10.0:
            state = "CAUTIOUS"
        else:
            state = "PANIC"

        return {
            "state": state,
            "meta": {
                "price_change_percent": change,
                "volume": volume,
            },
        }
