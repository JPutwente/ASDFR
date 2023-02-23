from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
         Node(
            package='image_tools',
            namespace='UnitTest',
            executable='cam2image',
            name='cam2image'
         ),
        Node(
            package='image_processing',
            namespace='UnitTest',
            executable='grayscaler',
            name='grayscaler'
        ),
        Node(
            package='image_processing',
            namespace='UnitTest',
            executable='brightmask',
            name='brightmask'
        ),
        Node(
            package='image_processing',
            namespace='UnitTest',
            executable='light_tracker',
            name='light_tracker'
        ),
       Node(
            package='jiwy_controller',
            namespace='UnitTest',
            executable='jc_lighttracker',
            name='jc_lighttracker'
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