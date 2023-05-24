#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <utility>
#include <atomic>
#include <filesystem>
#include <mutex>
#include <queue>
#include <span>
#include <shared_mutex>
#include <future>
#include <random>
#include <chrono>
#include <map>

#include <uv.h>

#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/details/thread_pool.h>

#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>