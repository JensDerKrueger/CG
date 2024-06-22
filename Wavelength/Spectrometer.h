#ifdef SPECTRO

#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <optional>

constexpr std::array<uint32_t,8> wavelengths = {415,445,480,515,555,590,630,680};

/*
 415 nm
 445 nm
 480 nm
 515 nm
 555 nm
 590 nm
 630 nm
 680 nm
 NIR
 All
 */
typedef std::array<uint32_t,10> Distribution;


class Spectrometer {
public:
  Spectrometer(const std::string& portname="");
  virtual ~Spectrometer();

  std::optional<Distribution> get();
  void light(bool on);
  bool isConnected() const {return isRunning;}
  void reconnect() {init();}

private:
  mutable std::mutex dataMutex;
  std::shared_ptr<std::thread> pollThread;

  Distribution data;
  uint16_t newData;
  bool continueRunning;
  char command;
  std::string portname;
  bool isRunning{false};

  void pollFunc();
  void parseLines(const std::vector<std::string>& lines);
  void init();
};
#endif
