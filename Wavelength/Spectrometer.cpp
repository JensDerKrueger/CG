#ifdef SPECTRO

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <filesystem>
#include <vector>
#include <iostream>

#include "Spectrometer.h"

// #define DEBUG_SERIAL 1

Spectrometer::Spectrometer(const std::string& portname) :
  pollThread(nullptr),
  newData(0),
  continueRunning(true),
  command(' '),
  portname(portname)
{
  init();
}

Spectrometer::~Spectrometer() {
  continueRunning = false;
  if (pollThread) {
    try {
      pollThread->join();
    } catch (...) {}
  }
}

void Spectrometer::init() {
  if (portname == "") {
    std::vector<std::string> portnames;
    std::string pattern = "tty.usbmodem";

    for (const auto& entry : std::filesystem::directory_iterator("/dev/")) {
      if (entry.is_regular_file() || entry.is_character_file()) {
        std::string filename = entry.path().filename().string();
        if (filename.find(pattern) != std::string::npos) {
          portnames.push_back(entry.path().string());
        }
      }
    }
    if (portnames.size() > 0) {
      portname = portnames[0];
    } else {
#ifdef DEBUG_SERIAL
      std::cerr << "Error searching for serial port of name " << pattern << "*" << std::endl;
#endif
      return;
    }
  }

  if (pollThread && pollThread->joinable()) {
    continueRunning = false;
    pollThread->join();
  }

  continueRunning = true;
  pollThread = std::make_shared<std::thread>(std::thread(&Spectrometer::pollFunc, this));
}

std::optional<Distribution> Spectrometer::get() {
  if (!isRunning) init();

  if (newData == (1<<data.size())-1) {
    const std::scoped_lock<std::mutex> lock(dataMutex);
    Distribution d = data;
    newData = 0;
    return d;
  } else {
    return {};
  }
}

static std::vector<std::string> splitStringAtNewlines(const std::string& input) {
  std::vector<std::string> result;
  std::string temp;
  for (char ch : input) {
    if (ch == '\n') {
      result.push_back(temp);
      temp.clear();
    } else {
      temp += ch;
    }
  }
  // Add the last part if it's a valid complete line
  if (!temp.empty() && temp.back() == '\r') {
    result.push_back(temp);
  }
  return result;
}

static std::optional<std::pair<uint32_t, uint32_t>> parseLine(const std::string& input) {
  // Check for carriage return at the end
  if (input.empty() || input.back() != '\r') {
    return std::nullopt;
  }

  // Remove the carriage return for easier processing
  std::string trimmedInput = input.substr(0, input.size() - 1);

  // Find the position of the colon
  size_t colonPos = trimmedInput.find(':');
  if (colonPos == std::string::npos) {
    return std::nullopt;
  }

  // Extract the parts before and after the colon
  std::string firstPart = trimmedInput.substr(0, colonPos);
  std::string secondPart = trimmedInput.substr(colonPos + 1);

  // Convert the parts to unsigned integers
  try {
    uint32_t firstNumber = std::stoul(firstPart);
    uint32_t secondNumber = std::stoul(secondPart);
    return std::make_pair(firstNumber, secondNumber);
  } catch (const std::exception&) {
    // Conversion failed
    return std::nullopt;
  }
}

void Spectrometer::parseLines(const std::vector<std::string>& lines) {
  const std::scoped_lock<std::mutex> lock(dataMutex);

  for (const auto& line : lines) {
    const auto r = parseLine(line);
    if (r && r->first < data.size()) {
      data[r->first] = r->second;
      newData = newData | (1 << r->first);
    }
  }
}

void Spectrometer::pollFunc() {
  int fd = open(portname.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

  if (fd < 0) {
#ifdef DEBUG_SERIAL
    std::cerr << "Error opening " << portname << ": " << strerror(errno) << std::endl;
#endif
    return;
  }

  // Switch back to blocking mode for read
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

  // Configure the serial port
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  if (tcgetattr(fd, &tty) != 0) {
#ifdef DEBUG_SERIAL
    std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
#endif
    close(fd);
    return;
  }

  // Set baud rate
  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);

  // Set other port settings
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
  tty.c_iflag &= ~IGNBRK;         // disable break processing
  tty.c_lflag = 0;                // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0;                // no remapping, no delays
  tty.c_cc[VMIN]  = 0;            // read doesn't block
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);    // ignore modem controls,
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);  // shut off parity
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
#ifdef DEBUG_SERIAL
    std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
#endif
    close(fd);
    return;
  }

  char buf[4096];
  std::string message;
  isRunning = true;
  while (continueRunning) {
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
      buf[n] = 0;
    } else if (n < 0) {
#ifdef DEBUG_SERIAL
      std::cerr << "Error reading: " << strerror(errno) << std::endl;
#endif
      break;
    }
    message.append(buf);

    const auto lines = splitStringAtNewlines(message);
    if (lines.size() >= 10) {
      parseLines(lines);
      message = "";
    }

    if (command != ' ') {
      write(fd, &command, 1);
      command = ' ';
    }
  }

  isRunning = false;
}

void Spectrometer::light(bool on) {
  command = on ? '1' : '0';
}

#endif
