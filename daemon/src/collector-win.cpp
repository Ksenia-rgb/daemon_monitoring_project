#include "collector-win.hpp"

#if IS_WINDOWS
#include <direct.h>
#include <windows.h>
#include <iostream>
#include <cmath>

namespace
{
  __int64 toInt64(FILETIME ft)
  {
    return static_cast< __int64 >(ft.dwHighDateTime) << 32 | ft.dwLowDateTime;
  }
  void ErrorShow() 
  { 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL) == 0) {
        MessageBox(NULL, TEXT("FormatMessage failed"), TEXT("Error"), MB_OK);
    }

    MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
  }

  float getTotalDiskSize()
  {
    int drive = _getdrive();
    char drive_letter = 'A' + drive - 1;
    std::string drive_str = "\\\\.\\";
    drive_str.push_back(drive_letter);
    drive_str.push_back(':');
    std::cout << drive_str << '\n';
    HANDLE h_drive = CreateFileA(
      drive_str.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      0,
      NULL
    );
    if (h_drive == INVALID_HANDLE_VALUE)
    {
      std::cout << "handle error\n";
      ErrorShow();
      return 0.0;
    }
    GET_LENGTH_INFORMATION disk_length;
    bool success = DeviceIoControl(
      h_drive,
      IOCTL_DISK_GET_LENGTH_INFO,
      NULL,
      0,
      &disk_length,
      sizeof(disk_length),
      NULL,
      NULL
    );
    if (!success)
    {
      std::cout << "device io error\n";
      CloseHandle(h_drive);
      return 0.0;
    }
    CloseHandle(h_drive);
    unsigned long long size_bytes = disk_length.Length.QuadPart;
    float size_gb = size_bytes / (1024ull * 1024ull * 1024ull);
    return size_gb;
  }
}

float CollectorWin::getCpuUsage() const
{
  static FILETIME idle_time_prev{}, kernel_time_prev{}, user_time_prev{};
  
  FILETIME idle_time, kernel_time, user_time;
  if (!GetSystemTimes(&idle_time, &kernel_time, &user_time))
  {
    return -1.0;
  }

  __int64 idle_diff = toInt64(idle_time) - toInt64(idle_time_prev);
  __int64 kernel_diff = toInt64(kernel_time) - toInt64(kernel_time_prev);
  __int64 user_diff = toInt64(user_time) - toInt64(user_time_prev);
  idle_time_prev = idle_time;
  kernel_time_prev = kernel_time;
  user_time_prev = user_time;

  float total = static_cast< float >(kernel_diff + user_diff);
  float idle = static_cast< float >(idle_diff);
  
  return total ? std::ceil((1.0 - idle / (total + idle)) * 100.0) : -1.0;
}
float CollectorWin::getMemoryUsage() const
{
  MEMORYSTATUSEX mem_info;
  mem_info.dwLength = sizeof(mem_info);
  if (!GlobalMemoryStatusEx(&mem_info))
  {
    return -1.0;
  }
  
  return static_cast< float >(mem_info.dwMemoryLoad);
}
float CollectorWin::getDiskUsage() const
{
  char drive_letter = 'A' + _getdrive() - 1;
  _ULARGE_INTEGER free_bytes_available;
  _ULARGE_INTEGER total_bytes_available;
  if (!GetDiskFreeSpaceEx(NULL, &free_bytes_available, &total_bytes_available, NULL))
  {
    return -1.0;
  }
  float load = static_cast< float >(total_bytes_available.QuadPart - free_bytes_available.QuadPart);
  float total = static_cast< float >(total_bytes_available.QuadPart);
  return total ? std::ceil((load / total) * 100.0) : -1.0;
}
float CollectorWin::getTemperature() const
{
  return -1.0;
}

#endif
