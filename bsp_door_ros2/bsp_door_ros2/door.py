# import rclpy
# from rclpy.node import Node
# from rclpy.action import ActionServer, CancelResponse
# from cutter_interfaces.action import CutterControl
# import can
# import time

# OPEN_POSITION = 2900000
# CLOSE_POSITION = 0

# class MotorActionServer(Node):
#     def __init__(self):
#         super().__init__('shear')

#         # Initialize CAN interface
#         self.bus = can.interface.Bus(channel='can0', bustype='socketcan')

#         self.node_id = 0x0E  # Node ID 14 (0x0E)
#         self.control_word_cobid = 0x600 + self.node_id  # SDO request COB-ID
#         self.status_word_cobid = 0x580 + self.node_id  # SDO response COB-ID

#         # Motor initialization sequence
#         self.init_motor()

#         # Create action server
#         self._action_server = ActionServer(
#             self,
#             CutterControl,
#             'cutter_control',
#             execute_callback=self.execute_callback,      
#             cancel_callback=self.cancel_callback,
#         )

#     def init_motor(self):
#         """Initialize motor with required configurations"""
#         self.send_sdo(0x6060, 0, 1)  # Set mode to Profile Position (PP)
#         self.send_sdo(0x6081, 0, 200000)  # Set profile velocity
#         self.send_sdo(0x6083, 0, 200000)  # Set profile acceleration
#         self.send_sdo(0x6084, 0, 200000)  # Set profile deceleration

#         self.send_sdo(0x6040, 0, 0x0026)  # Shutdown
#         time.sleep(0.5)
#         self.send_sdo(0x6040, 0, 0x0027)  # Switch on
#         time.sleep(0.5)
#         self.send_sdo(0x6040, 0, 0x002F)  # Enable operation
#         time.sleep(0.5)

#         self.get_logger().info("Motor initialized successfully.")

#     def send_sdo(self, index, subindex, value):
#         """Send an SDO write request via CAN"""
#         data = [
#             0x23,  # Command byte for 4-byte write
#             index & 0xFF, (index >> 8) & 0xFF,  # Index (Little Endian)
#             subindex,  # Sub-index
#             value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF, (value >> 24) & 0xFF,  # Value (4 bytes)
#         ]
#         msg = can.Message(arbitration_id=self.control_word_cobid, data=data, is_extended_id=False)
#         self.bus.send(msg)
#         self.get_logger().info(f"Sent SDO write: Index 0x{index:04X}, Value {value}")

#     def receive_sdo(self, index):
#         """Receive an SDO response from the motor"""
#         while True:
#             msg = self.bus.recv(timeout=1.0)
#             if msg and msg.arbitration_id == self.status_word_cobid:
#                 if msg.data[1] == (index & 0xFF) and msg.data[2] == ((index >> 8) & 0xFF):
#                     value = int.from_bytes(msg.data[4:8], byteorder='little')
#                     return value

#     def move_to_position(self, target_position):
#         """Move motor to the specified position"""
#         self.send_sdo(0x607A, 0, target_position)  # Set target position
#         self.send_sdo(0x6040, 0, 0x003F)  # Start motion
#         self.get_logger().info(f"Moving to position: {target_position}")

#         while True:
#             status_word = self.receive_sdo(0x6041)  # Get status word
#             position = self.receive_sdo(0x6064)  # Get current position
#             self.get_logger().info(f"Current Position: {position}, Status Word: {status_word}")

#             if (status_word >> 10) & 0b1 == 1:  # Target position reached
#                 self.get_logger().info(f"Target position {target_position} reached.")
#                 self.send_sdo(0x6040, 0, 0x0100)  # Stop motion
#                 break

#             time.sleep(0.1)

#     def stop_motor(self):
#         """Stop the motor"""
#         self.send_sdo(0x6040, 0, 0x0100)  # Stop motion
#         self.get_logger().info("Motor stopped.")

#     def execute_callback(self, goal_handle):
#         """Execute motor commands based on the action request"""
#         command = goal_handle.request.command.lower()
#         feedback_msg = CutterControl.Feedback()

#         self.get_logger().info(f"Received command: {command}")

#         if command == "open":
#             feedback_msg.feedback = "Opening motor..."
#             goal_handle.publish_feedback(feedback_msg)
#             self.move_to_position(OPEN_POSITION)
#             result = CutterControl.Result()
#             result.success = True
#             result.message = "Motor opened successfully."
#             return result

#         elif command == "close":
#             feedback_msg.feedback = "Closing motor..."
#             goal_handle.publish_feedback(feedback_msg)
#             self.move_to_position(CLOSE_POSITION)
#             result = CutterControl.Result()
#             result.success = True
#             result.message = "Motor closed successfully."
#             return result

#         elif command == "stop":
#             feedback_msg.feedback = "Stopping motor..."
#             goal_handle.publish_feedback(feedback_msg)
#             self.stop_motor()
#             result = CutterControl.Result()
#             result.success = True
#             result.message = "Motor stopped successfully."
#             return result

#         else:
#             result = CutterControl.Result()
#             result.success = False
#             result.message = "Invalid command."
#             return result

#     def cancel_callback(self, goal):
#         """Handle action cancellation requests"""
#         self.get_logger().info("Cancel request received.")
#         self.stop_motor()
#         return CancelResponse.ACCEPT

# def main(args=None):
#     rclpy.init(args=args)
#     action_server = MotorActionServer()

#     try:
#         rclpy.spin(action_server)
#     except KeyboardInterrupt:
#         action_server.get_logger().info("Shutting down...")
#     finally:
#         if action_server.bus:
#             action_server.bus.shutdown()
#         rclpy.shutdown()

# if __name__ == "__main__":
#     main()

import rclpy
from rclpy.node import Node
from cutter_interfaces.srv import CutterControl  # Update with the correct service definition
import can

class MotorService(Node):
    def __init__(self):
        super().__init__('motor_service')

        # Initialize CAN interface
        self.bus = can.interface.Bus(channel='can0', bustype='socketcan')

        self.cob_id = 0x60F  # Set COB-ID for the CAN messages

        # Create a ROS2 service
        self.srv = self.create_service(CutterControl, 'cutter_control', self.control_callback)
        self.get_logger().info("Motor control service is ready.")

    def send_can_command(self, data):
        """Send a CAN message with the given data"""
        msg = can.Message(arbitration_id=self.cob_id, data=data, is_extended_id=False)
        try:
            self.bus.send(msg)
            self.get_logger().info(f"Sent CAN message: ID 0x{self.cob_id:X}, Data: {data}")
            return True
        except can.CanError:
            self.get_logger().error("Failed to send CAN message")
            return False

    def control_callback(self, request, response):
        """Handle service requests"""
        command = request.command
        
        if command == "open":
            self.get_logger().info("Extending actuator...")

            # Send CAN message for Extend
            extend_data = [0x08, 0x11, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00]
            success = self.send_can_command(extend_data)

            response.success = success
            response.message = "Actuator extended." if success else "Failed to extend actuator."

        elif command == "close":
            self.get_logger().info("Retracting actuator...")

            # Send CAN message for Retract
            retract_data = [0x08, 0x11, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00]
            success = self.send_can_command(retract_data)

            response.success = success
            response.message = "Actuator retracted." if success else "Failed to retract actuator."

        else:
            response.success = False
            response.message = "Invalid command. Use 'open' or 'close'."

        return response

def main(args=None):
    rclpy.init(args=args)
    motor_service = MotorService()

    try:
        rclpy.spin(motor_service)
    except KeyboardInterrupt:
        motor_service.get_logger().info("Shutting down...")
    finally:
        if motor_service.bus:
            motor_service.bus.shutdown()
        rclpy.shutdown()

if __name__ == "__main__":
    main()
