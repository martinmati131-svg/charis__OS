# aura_vla_node.py
import rclpy
from aura_interfaces.msg import BioBatchStatus

class AuraVLA(Node):
    def __init__(self):
        super().__init__('aura_vla')
        self.subscription = self.create_subscription(
            BioBatchStatus, '/aura/vision/batch_analysis', self.evaluate_action, 10)

    def evaluate_action(self, msg):
        # VLA Logic: Visual input (color_index) + Language constraint (SOP)
        if msg.color_index == "DARK_GREEN_TURBID" and msg.ph > 6.5:
            self.get_logger().info("VLA Action: Triggering high-torque stir for Tithonia extraction.")
            self.publish_motor_command(rpm=120, duration=300)
        
        elif "pest_detected" in msg.visual_tags:
            self.get_logger().warn("VLA Action: Formulating Neem-Mufangi repellent batch.")
            self.trigger_extraction_cycle("REP_01")
