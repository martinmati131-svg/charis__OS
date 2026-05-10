from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Existing Actuator Node
        Node(package='aura_actuators', executable='stepper_controller'),
        
        # New Quantum Bridge Node
        Node(
            package='aura_vla',
            executable='quantum_bridge_node',
            name='aura_quantum_bridge',
            output='screen'
        ),
        
        # Vision Node (VLA)
        Node(package='aura_vision', executable='camera_node')

    ])
