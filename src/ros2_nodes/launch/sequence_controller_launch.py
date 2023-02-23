from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='jiwy_controller',
            namespace='UnitTest',
            executable='jiwy_controller',
            name='jiwy_controller'
        ),
        Node(
            package='image_tools',
            namespace='UnitTest',
            executable='cam2image',
            name='cam2image'
        ),
        Node(
            package='jiwy_simulator',
            namespace='UnitTest',
            executable='jiwy_simulator',
            name='jiwy_simulator'
        ),
        Node(
            package='jiwy_controller',
            namespace='UnitTest',
            executable='simplesub',
            name='simplesub'
        )
    ])