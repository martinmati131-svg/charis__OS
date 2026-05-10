import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 1. Vision Node: Captures batch color/pests
        Node(
            package='aura_vision',
            executable='camera_inference_node',
            name='aura_vision_vla',
            parameters=[{'model_path': '/opt/aura/models/vla_lite_v1.tflite'}]
        ),
        
        # 2. Language Node: Processes the Bio-Extraction SOPs
        Node(
            package='aura_logic',
            executable='sop_interpreter',
            name='aura_sop_engine',
            parameters=[{'sop_file': 'tithonia_synergy_v2.json'}]
        ),
        
        # 3. Action Node: Controls the NEMA 17 Stirrers
        Node(
            package='aura_actuators',
            executable='stepper_controller',
            name='aura_motor_driver',
            output='screen'
        ),
        
        # 4. The VLA Bridge: The "Thinking" Center
        Node(
            package='aura_vla_bridge',
            executable='vla_decision_engine',
            name='aura_brain_center',
            remappings=[('/cmd_vel', '/aura/stirrers/velocity')]
        )
    ])
