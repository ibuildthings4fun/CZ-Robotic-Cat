import requests
from typing import Dict


class BNBClient:
    def __init__(self, price_api: str):
        self.price_api = price_api

    def get_market_snapshot(self) -> Dict:
        """拉取 BNB 24 小时基础行情数据。

        返回:
            dict: 包含 last_price, volume, price_change_percent 等字段。
        """
        resp = requests.get(self.price_api, timeout=5)
        resp.raise_for_status()
        data = resp.json()

        return {
            "last_price": float(data.get("lastPrice", 0.0)),
            "volume": float(data.get("volume", 0.0)),
            "price_change_percent": float(data.get("priceChangePercent", 0.0)),
        }
