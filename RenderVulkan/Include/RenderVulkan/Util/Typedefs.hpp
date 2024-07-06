#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <random>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <any>
#include <exception>
#include <array>
#include <set>
#include <format>
#include <list>
#include <optional>
#include <regex>
#include <typeindex>
#include <filesystem>
#include <typeinfo>
#include <type_traits>
#include <limits>
#include <wrl.h> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#ifdef DLL_MODE
#define DLL_API __declspec(dllimport)
#else
#define DLL_API __declspec(dllexport)
#endif

#undef WriteConsole
#undef CopyMemory
#undef ERROR

namespace RenderVulkan
{
	namespace Util
	{
		typedef signed char schar;
		typedef signed short sshort;
		typedef signed int sint;
		typedef signed long slong;
		typedef signed long long int slongint;

		typedef char Byte;

		typedef unsigned char uchar;
		typedef unsigned short ushort;
		typedef unsigned int uint;
		typedef unsigned long ulong;
		typedef unsigned long long ullong;
		typedef unsigned long long int ullint;

		typedef std::string String;
		typedef std::wstring WString;

		typedef glm::vec<2, int, glm::packed_highp> Vector2i;
		typedef glm::vec<3, int, glm::packed_highp> Vector3i;
		typedef glm::vec<4, int, glm::packed_highp> Vector4i;

		typedef glm::vec<2, float, glm::packed_highp> Vector2f;
		typedef glm::vec<3, float, glm::packed_highp> Vector3f;
		typedef glm::vec<4, float, glm::packed_highp> Vector4f;

		typedef glm::vec<2, double, glm::packed_highp> Vector2d;
		typedef glm::vec<3, double, glm::packed_highp> Vector3d;
		typedef glm::vec<4, double, glm::packed_highp> Vector4d;

		typedef glm::mat<2, 2, float, glm::packed_highp> Matrix2x2f;
		typedef glm::mat<3, 3, float, glm::packed_highp> Matrix3x3f;
		typedef glm::mat<4, 4, float, glm::packed_highp> Matrix4x4f;

		typedef glm::qua<int, glm::packed_highp> Quaternioni;
		typedef glm::qua<float, glm::packed_highp> Quaternionf;	
		typedef glm::qua<double, glm::packed_highp> Quaterniond;

		typedef std::any Any;
		typedef std::exception RuntimeError;

		typedef std::type_info TypeInformation;
		typedef std::type_index TypeIndex;

		typedef std::filesystem::path Path;

		typedef std::chrono::steady_clock SteadyClock;
		typedef std::chrono::high_resolution_clock Clock;
		typedef std::chrono::time_point<Clock> TimePoint;
		typedef std::chrono::high_resolution_clock::time_point HighResolutionTimePoint;
		typedef std::chrono::duration<float> Duration;
		typedef std::chrono::milliseconds Milliseconds;

		typedef std::thread Thread;
		typedef std::mutex Mutex;
		typedef std::condition_variable ConditionVariable;
		typedef std::unique_lock<Mutex> UniqueLock;

		typedef std::atomic<int> AtomicInt;
		typedef std::atomic<bool> AtomicBool;
		typedef std::future<void> Future;

		typedef std::time_t Time;
		typedef std::tm TimeInformation;

		typedef std::filesystem::directory_iterator DirectoryIterator;
		typedef std::filesystem::directory_entry DirectoryEntry;
		typedef std::filesystem::file_status FileStatus;
		typedef std::filesystem::file_type FileType;

		typedef std::random_device RandomDevice;
		typedef std::mt19937 RandomEngine;

		typedef std::ostream OutputStream;
		typedef std::istream InputStream;

		typedef std::stringstream StringStream;
		typedef std::ostringstream OutputStringStream;

		typedef size_t Size;
		typedef char Character;

		typedef std::ofstream OutputFileStream;
		typedef std::ifstream InputFileStream;

		typedef std::streamsize StreamSize;

		typedef std::regex Regex;

		template<typename T>
		using Shared = std::shared_ptr<T>;

		template<typename T>
		using Unique = std::unique_ptr<T>;

		template<typename T>
		using Weak = std::weak_ptr<T>;

		template<typename T>
		using Vector = std::vector<T>;

		template<typename T, typename A>
		using Map = std::map<T, A>;

		template<typename T, typename A>
		using UnorderedMap = std::unordered_map<T, A>;

		template<typename T>
		using Function = std::function<T>;

		template<typename T, size_t S>
		using Array = std::array<T, S>;

		template<typename T>
		using Set = std::set<T>;

		template<class... _Args>
		using FormatString = std::basic_format_string<char, std::type_identity_t<_Args>...>;

		template<typename T>
		using EnableShared = std::enable_shared_from_this<T>;

		template<typename T>
		using List = std::list<T>;

		template<typename T, typename A>
		using Pair = std::pair<T, A>;

		template<typename T>
		using Optional = std::optional<T>;

		template<typename T>
		using LockGuard = std::lock_guard<T>;

		template<typename T>
		using Atomic = std::atomic<T>;

		template<typename T>
		using NumericLimits = std::numeric_limits<T>;

		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	}
}