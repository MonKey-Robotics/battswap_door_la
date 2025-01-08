#include <fcntl.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "std_srvs/srv/set_bool.hpp"
#include "rclcpp/rclcpp.hpp"
#ifdef __GLIBC__
#include <sys/perm.h>
#endif

#include <memory>

// #include "rclcpp/rclcpp.hpp"
// #include "bsp_msgs/srv/trigger_actuator.hpp"

#define INDEX_IO ((short)0x4E)
#define DATA_IO ((short)0x4F)

unsigned int SMBus_Base = 0xF040;
unsigned int SMBus_SlaveAddress = 0x94;

#pragma region root
int get_io_permission(void)  // Admin
{
  if (iopl(3)) {
    fprintf(stderr, "iopl(): %s\n", strerror(errno));
    return errno;
  }

  return 0;
}
#pragma endregion
#pragma region SMBus R/W Function
unsigned int SMB_read(int PORT, int DEVICE, int REG_INDEX)  // SMB Read
{
  unsigned int SMB_Value;

  outb(0x00, PORT + 02);
  outb(0xff, PORT + 00);
  usleep(10000);
  outb(DEVICE + 1, PORT + 04);
  usleep(10000);
  outb(REG_INDEX, PORT + 03);
  usleep(10000);
  outb(0x48, PORT + 02);
  usleep(10000);
  SMB_Value = inb(PORT + 05);
  return SMB_Value;
}

unsigned int SMB_write(int PORT, int DEVICE, int REG_INDEX,
                       int REG_DATA)  // SMB Write
{
  outb(0x00, PORT + 02);
  outb(0xFF, PORT + 00);
  usleep(10000);
  outb(DEVICE, PORT + 04);
  usleep(10000);
  outb(REG_INDEX, PORT + 03);
  usleep(10000);
  outb(REG_DATA, PORT + 05);
  usleep(10000);
  outb(0x48, PORT + 02);
  usleep(10000);
}
#pragma endregion

#pragma region Linear Actuator Extend
void LA_Ext() {
  unsigned int DO_Value;
  unsigned int D1_Value;
  unsigned int D2_Value;

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D2_Value = DO_Value | 0x02;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
  printf("DO2 set to High\n");

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D1_Value = DO_Value & 0xFE;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
  printf("DO1 set to Low\n");
}
#pragma endregion

#pragma region Linear Actuator Off
void LA_Off() {
  unsigned int DO_Value;
  unsigned int D1_Value;
  unsigned int D2_Value;

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D1_Value = DO_Value & 0xFE;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
  printf("DO1 set to Low\n");

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D2_Value = DO_Value & 0xFD;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
  printf("DO2 set to Low\n");
}
#pragma endregion

#pragma region Linear Actuator Retract
void LA_Ret() {
  unsigned int DO_Value;
  unsigned int D1_Value;
  unsigned int D2_Value;

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D2_Value = DO_Value | 0x02;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
  printf("DO2 set to High\n");

  sleep(0.1);
  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D1_Value = DO_Value | 0x01;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
  printf("DO1 set to High\n");
}
#pragma endregion

#pragma region Set DO Function

class BSPDoorServer : public rclcpp::Node {
 public:
  explicit BSPDoorServer(const std::string &node_name) : Node(node_name) {
    // Create set home service
    bsp_door_srv_ = this->create_service<std_srvs::srv::SetBool>(
        node_name + "/BSP_door",
        std::bind(&BSPDoorServer::bsp_door_control, this, std::placeholders::_1,
                  std::placeholders::_2));

    RCLCPP_INFO(this->get_logger(), "%s/%s is ready.", this->get_namespace(),
                this->get_name());
  }

  void bsp_door_control(
      const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
      std::shared_ptr<std_srvs::srv::SetBool::Response> response) {
    // Extend
    if (request->data) {
      RCLCPP_INFO(this->get_logger(), "BSP door opening ... ");
      LA_Off();
      LA_Ext();
      // sleep(10);
      // LA_Off();
      response->success = true;
      response->message = "BSP door opened, preparing for battery swap";
      RCLCPP_INFO(this->get_logger(), response->message.c_str());
    }

    // Retract
    if (request->data == 0) {
      RCLCPP_INFO(this->get_logger(), "BSP door closing ... ");
      LA_Off();
      LA_Ret();
      // sleep(10);
      // LA_Off();
      response->success = true;
      response->message = "BSP door closed, resuming work";
      RCLCPP_INFO(this->get_logger(), response->message.c_str());
    }

    else {
      LA_Off();
      response->success = false;
      response->message = "Failed to actuate BSP door";
      RCLCPP_INFO(this->get_logger(), response->message.c_str());
    }
  }

 private:
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr bsp_door_srv_;
};
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<BSPDoorServer>("bsp_door_server");
  // std::shared_ptr<BSPDoorServer> node =
  //     rclcpp::Node::make_shared("bsp_door_server");

  // rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr service =
  //     node->create_service<std_srvs::srv::SetBool>("bsp_door_control",
  //                                                          &bsp_door_control);

  rclcpp::spin(node);
  rclcpp::shutdown();
}