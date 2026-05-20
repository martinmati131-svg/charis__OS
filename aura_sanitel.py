import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Bool
import subprocess

class AuraSentinel(Node):
    def __init__(self):
        super().__init__('aura_sentinel')
        
        # Publisher to emergency stop the system if compromised
        self.lockdown_pub = self.create_publisher(Bool, '/aura/system/lockdown', 10)
        
        # A simplified 'Threat List' based on Intelligence Feeds
        self.malicious_ips = ["194.31.98.124", "45.147.230.110"] # Examples from Intel repos
        
        # Check network every 30 seconds
        self.timer = self.create_timer(30.0, self.check_network_integrity)
        self.get_logger().info("Sentinel Watchdog Active: Monitoring for Industrial Cyber Threats.")

    def check_network_integrity(self):
        # Scan active connections on the Pi 5
        try:
            output = subprocess.check_output(["netstat", "-ntu"]).decode('utf-8')
            for ip in self.malicious_ips:
                if ip in output:
                    self.get_logger().error(f"THREAT DETECTED: Connection from blacklisted IP {ip}!")
                    self.initiate_lockdown()
        except Exception as e:
            self.get_logger().error(f"Sentinel Error: {e}")

    def initiate_lockdown(self):
        msg = Bool()
        msg.data = True
        self.lockdown_pub.publish(msg)
        self.get_logger().warn("SYSTEM LOCKDOWN: Cutting power to Actuators to prevent unauthorized control.")

def main(args=None):
    rclpy.init(args=args)
    node = AuraSentinel()
    rclpy.spin(node)
    rclpy.shutdown()
