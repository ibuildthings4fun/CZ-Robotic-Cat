import time
import yaml
import json
import paho.mqtt.client as mqtt

from services.bnb_client import BNBClient
from services.cat_state import CatStateEngine


def load_config(path: str = "config.yaml") -> dict:
    """加载 YAML 配置文件。"""
    with open(path, "r", encoding="utf-8") as f:
        return yaml.safe_load(f)


def main():
    config = load_config()

    bnb_client = BNBClient(price_api=config["bnb"]["price_api"])
    engine = CatStateEngine()

    mqtt_client = mqtt.Client()
    mqtt_client.connect(config["mqtt"]["host"], config["mqtt"]["port"], 60)

    topic = config["mqtt"]["topic"]
    interval = config.get("poll_interval_seconds", 15)

    print(f"[backend] 启动 CZ Robotic Cat 后端，向主题 '{topic}' 每 {interval}s 发布一次状态。")

    while True:
        try:
            snapshot = bnb_client.get_market_snapshot()
            cat_state = engine.compute_state(snapshot)

            payload = json.dumps(cat_state, ensure_ascii=False)
            mqtt_client.publish(topic, payload)
            print(f"[backend] 发布状态: {payload}")

        except Exception as e:
            print(f"[backend] 出错: {e}")

        time.sleep(interval)


if __name__ == "__main__":
    main()
