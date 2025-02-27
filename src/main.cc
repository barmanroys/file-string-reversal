#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <execution>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

class Logger {
public:
  enum LogLevel { DEBUG, INFO, WARNING, ERROR };

  static void log(LogLevel level, const std::string &message) {
    std::string levelStr;
    switch (level) {
    case DEBUG:
      levelStr = "DEBUG";
      break;
    case INFO:
      levelStr = "INFO";
      break;
    case WARNING:
      levelStr = "WARNING";
      break;
    case ERROR:
      levelStr = "ERROR";
      break;
    }

    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&nowTime) << " [" << levelStr << "] " << message
              << std::endl;
  }
};

// Macros for easier logging
#define LOG_DEBUG(msg) Logger::log(Logger::DEBUG, msg)
#define LOG_INFO(msg) Logger::log(Logger::INFO, msg)
#define LOG_WARNING(msg) Logger::log(Logger::WARNING, msg)
#define LOG_ERROR(msg) Logger::log(Logger::ERROR, msg)

class AbstractFileHandler {
public:
  virtual std::string read() = 0;              // Read a line from the file
  virtual void write(const std::string &) = 0; // Write a line to the file
  virtual bool isOpen() const = 0;             // Check if the file is open
};

class AbstractIOHandler {
public:
  virtual std::vector<std::string>
  get_original_lines() = 0; // Get the original strings to reverse
  virtual void persist_reversed_strings(
      std::vector<std::string>) = 0; // Dump the reversed strings
};

class AbstractInPlaceStringManipulator {
public:
  virtual void manipulate() = 0; // Reverse the component strings in place
  virtual std::vector<std::string>
  get_out_strings() const = 0; // Get the reversed strings
};

class FDWithContext
    : public AbstractFileHandler { // Imitates a python context manager, to
                                   // avoid forgetting to close the file

private:
  std::fstream fs;
  std::string fpath;

public:
  FDWithContext(const std::string &filePath, std::ios::openmode mode) {
    fs = std::fstream(filePath, mode);
    fpath = filePath;

    if (!fs.is_open()) {
      throw std::runtime_error("Failed to open file: " + filePath);
    }
  }

  // Destructor that closes the file
  ~FDWithContext() {
    fs.flush();
    if (fs.is_open())
      fs.close();
  }

  // Function to read from the file
  std::string read() {
    std::string content;
    std::getline(fs, content);
    return content;
  }

  bool iswritable() const {
    return (fs.good() && (fs.flags() & std::ios::out));
  }

  // Function to write to the file (only works if opened in write mode)
  void write(const std::string &data) {
    fs << data << std::endl;
    LOG_DEBUG("Wrote " + data + " to file: " + fpath);
  }

  // Check if the file is open
  bool isOpen() const { return fs.is_open(); }
};

class IOHandler : public AbstractIOHandler {
private:
  AbstractFileHandler *in_file, *out_file;

public:
  IOHandler(AbstractFileHandler *in_file, AbstractFileHandler *out_file)
      : in_file(in_file), out_file(out_file) {}

  // Get the original strings to reverse
  std::vector<std::string> get_original_lines() override {
    std::vector<std::string> lines;
    while (in_file->isOpen()) {
      std::string line = in_file->read();
      if (line.empty())
        break;
      lines.push_back(line);
    }
    return lines;
  }

  // Dump the reversed strings
  void persist_reversed_strings (std::vector<std::string> lines) override

  {
    for (const std::string &line : lines)
      out_file->write(line);
  }

  // Clean up the file handlers
  ~IOHandler() {
    delete in_file;
    delete out_file;
  };
};

class StringReverser : public AbstractInPlaceStringManipulator {
private:
  std::vector<std::string>& lines; // Reference to the original string to ensure shallow copy 

  static void reverse_string(std::string &str) {
    std::reverse(str.begin(), str.end());
  }

public:
  StringReverser(std::vector<std::string>& lines) : lines(lines) {}

  void manipulate() {
    // Parallel execution of inplace reversal 
    std::for_each(std::execution::par, lines.begin(), lines.end(),
                  reverse_string);
  }

  std::vector<std::string> get_out_strings() const { return lines; };
};


class Main{
  private:
     AbstractIOHandler* ioHandler; // Pointer to AbstractIOHandler
  public:
   Main(AbstractIOHandler* handler) : ioHandler(handler) {} // Constructor

    void run(){
      LOG_INFO("IO Handler initialised.");
      std::vector<std::string> lines{ioHandler->get_original_lines()};
      LOG_INFO("Read " + std::to_string(lines.size()) +
              " lines from the input file.");
      auto string_reverser = StringReverser(lines);
      string_reverser.manipulate();
      LOG_INFO("Reversed the strings.");
      ioHandler->persist_reversed_strings(string_reverser.get_out_strings());
      LOG_INFO("Persisted the data into the file.");
    }
};

int main(int argc, char *argv[])
{
  // Define the input and output file paths
  std::string const in_file_path{"../data/in_file.txt"}, out_file_path{"../data/out_file.txt"};
  IOHandler *handler = new IOHandler(new FDWithContext(in_file_path, std::ios::in), 
    new FDWithContext(out_file_path, std::ios::in | std::ios::out | std::ios::app));
  Main job_runner=Main(handler);
  job_runner.run(); // Run the jobs on the input and output paths. 
  return EXIT_SUCCESS;
}