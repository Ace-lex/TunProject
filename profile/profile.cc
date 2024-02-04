// Profile UDPTunSend using memcpy and UDPTunSend using writev

#include <dirent.h>

#include <chrono>

#include "tun.h"

const char *const kScriptPath =
    "../script.sh";  // The script path to configure tun device
const char *const kSrcIP =
    "10.10.10.1";  // The source ip of tested packets(refer the script)
const char *const kDstIP = "192.168.0.39";
const int kPathLen = 1024;    // The max file path length
const int kPacketLen = 8192;  // The max packet length
const int kSrcPort = 31233;   // The source port of tested packets
const int kDestPort = 8080;   // The destination port of tested packets
const int kInternal = 1;      // The send interval(second)

void MemcpyProfile(int tun, const char *dir_name) {
  DIR *dir;
  struct dirent *entry;
  char path[kPathLen];

  // Open payload directory
  dir = opendir(dir_name);

  if (dir == NULL) {
    perror("Error opening directory");
    exit(EXIT_FAILURE);
  }

  // Traverse the directory
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      int ret;
      uint8_t payload[kPacketLen];
      int payload_length;
      sprintf(path, "%s/%s", dir_name, entry->d_name);
      payload_length = FileSize(path);
      FILE *fp = fopen(path, "rb");
      if (fp == NULL) {
        fprintf(stdout, "Error opening the file: %s\n", strerror(errno));
      }
      fread(payload, 1, payload_length, fp);
      fclose(fp);

      // Send the udp packets
      ret = UDPTunSend(tun, kSrcIP, kDstIP, kSrcPort, kDestPort, payload,
                       payload_length);

      // Ensure successful reception at the receiving end.
      // sleep(kInternal);
    }
  }

  closedir(dir);
}

void WritevProfile(int tun, const char *dir_name) {
  DIR *dir;
  struct dirent *entry;
  char path[kPathLen];

  // Open payload directory
  dir = opendir(dir_name);

  if (dir == NULL) {
    perror("Error opening directory");
    exit(EXIT_FAILURE);
  }

  // Traverse the directory
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      int ret;
      uint8_t payload[kPacketLen];
      int payload_length;
      sprintf(path, "%s/%s", dir_name, entry->d_name);
      payload_length = FileSize(path);
      FILE *fp = fopen(path, "rb");
      if (fp == NULL) {
        fprintf(stdout, "Error opening the file: %s\n", strerror(errno));
      }
      fread(payload, 1, payload_length, fp);
      fclose(fp);

      // Send the udp packets
      ret = UDPTunSendv2(tun, kSrcIP, kDstIP, kSrcPort, kDestPort, payload,
                         payload_length);

      // Ensure successful reception at the receiving end.
      // sleep(kInternal);
    }
  }

  closedir(dir);
}

int main(int argc, char *argv[]) {
  int tun, ret;
  char tun_name[IFNAMSIZ];
  char dir_name[kPathLen];
  strcpy(dir_name, argv[1]);

  // Create tun devices
  tun_name[0] = '\0';
  tun = TunCreate(tun_name, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }

  // Configure the tun device
  system(kScriptPath);

  // Profile UDPTunSend using memcpy
  auto start = std::chrono::steady_clock::now();
  MemcpyProfile(tun, dir_name);
  auto end = std::chrono::steady_clock ::now();
  int duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();
  printf("UDPTunSend using memcpy duration(microseconds): %d\n", duration);

  // Profile UDPTunSend using writev
  start = std::chrono::steady_clock::now();
  WritevProfile(tun, dir_name);
  end = std::chrono::steady_clock ::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                 .count();
  printf("UDPTunSend using writev duration(microseconds): %d\n", duration);
  return 0;
}
