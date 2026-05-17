import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import LogInfo, TimerAction

def generate_launch_description():
    return LaunchDescription([
        LogInfo(msg="Aura Intelligence: Initializing Charis OS Industrial Stack..."),

        # 1. Hardware Interface Layer (NEMA 17 Steppers)
        Node(
            package='aura_actuators',
            executable='motor_driver_node',
            name='actuator_layer',
            parameters=[{'default_rpm': 60}],
            output='screen'
        ),

        # 2. Vision & VLA Layer (Pi Camera + Local Inference)
        Node(
            package='aura_vision',
            executable='vla_vision_node',
            name='vision_layer',
            output='screen'
        ),

        # 3. The Quantum Cloud Bridge (Delayed to allow network boot)
        TimerAction(
            period=5.0,
            actions=[
                Node(
                    package='aura_vla',
                    executable='quantum_bridge_node',
                    name='cloud_logic_bridge',
                    parameters=[{'api_endpoint': 'https://profsamsonwquantum.pythonanywhere.com/vla_inference'}],
                    output='screen'
                ),
                LogInfo(msg="Quantum Bridge Active. Tunneling to PythonAnywhere Hub.")
            ]
        ),

        # 4. Telemetry Stream (Feeds your PWA App)
        Node(
            package='aura_telemetry',
            executable='web_bridge_node',
            name='pwa_telemetry_hub',
            output='screen'
        )
    ])
