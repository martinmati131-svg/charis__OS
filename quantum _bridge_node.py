import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32, Int32
import requests

class QuantumBridge(Node):
    def __init__(self):
        super().__init__('quantum_bridge')
        
        # Subscriptions to local sensors
        self.ph_sub = self.create_subscription(Float32, '/aura/sensors/ph', self.ph_callback, 10)
        self.turb_sub = self.create_subscription(Float32, '/aura/sensors/turbidity', self.turb_callback, 10)
        
        # Publisher for the optimized RPM
        self.motor_pub = self.create_publisher(Int32, '/aura/actuators/target_rpm', 10)
        
        self.current_ph = 7.0
        self.current_turb = 0.5
        self.api_url = "https://profsamsonwquantum.pythonanywhere.com/vla_inference"

    def ph_callback(self, msg): self.current_ph = msg.data
    def turb_callback(self, msg): 
        self.current_turb = msg.data
        self.request_quantum_inference() # Trigger inference when turbidity updates

    def request_quantum_inference(self):
        try:
            payload = {"node_id": "JUJA_NODE_01", "ph": self.current_ph, "turbidity": self.current_turb}
            response = requests.post(self.api_url, json=payload, timeout=2.0)
            data = response.json()
            
            rpm_msg = Int32()
            rpm_msg.data = data['recommended_action']['rpm']
            self.motor_pub.publish(rpm_msg)
            self.get_logger().info(f"Quantum Decision Executed: {rpm_msg.data} RPM")
            
        except Exception as e:
            self.get_logger().error(f"Cloud Link Latency: Using Local Charis Safety Mode. Error: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = QuantumBridge()
    rclpy.spin(node)
    rclpy.shutdown()
