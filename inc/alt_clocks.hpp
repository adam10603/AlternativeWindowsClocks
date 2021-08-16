#ifndef _ALT_CLOCKS_HPP_
#define _ALT_CLOCKS_HPP_

#include <chrono>
#include <atomic>
#include <Windows.h>

// Clock classes for Windows that implement TrivialClock requirements (compatible with std::chrono)
namespace alt_clocks {

	// Wraps QueryPerformanceCounter(). Resolution is under 1us (typically 100ns). Less performant than fast_monotonic_clock.
	class precision_monotonic_clock {
	public:

		using rep						= std::chrono::nanoseconds::rep;
		using period					= std::ratio<	std::chrono::nanoseconds::period::num,
														std::chrono::nanoseconds::period::den / 100>;
		using duration					= std::chrono::duration<rep, period>;
		using time_point				= std::chrono::time_point<precision_monotonic_clock>;
		static constexpr bool is_steady	= true;

		[[nodiscard]] static auto now() noexcept {
			static_assert(period::num == 1); // Should always pass

			const auto freq	= get_freq();
			const auto ctr	= get_ctr();

			if (freq == period::den) return time_point(duration(ctr));

			const auto whole	= (ctr / freq) * static_cast<rep>(period::den);
			const auto part		= (ctr % freq) * static_cast<rep>(period::den) / freq;

			return time_point(duration(whole + part));
		}

	private:

		static inline rep get_freq() noexcept {
			static std::atomic<rep> freq_cache{ 0 };

			auto freq = freq_cache.load(std::memory_order_relaxed);

			if (freq == 0) {
				auto retrieved_freq = LARGE_INTEGER{};
				QueryPerformanceFrequency(&retrieved_freq);

				freq = static_cast<rep>(retrieved_freq.QuadPart);
				freq_cache.store(freq, std::memory_order_relaxed);
			}

			return freq;
		}

		static inline rep get_ctr() noexcept {
			auto retrieved_ctr = LARGE_INTEGER{};
			QueryPerformanceCounter(&retrieved_ctr);

			return static_cast<rep>(retrieved_ctr.QuadPart);
		}
	};

	// Wraps GetTickCount64(). Typical resolution is 10-16ms. More performant than precision_monotonic_clock.
	class fast_monotonic_clock {
	public:
		using rep						= std::chrono::milliseconds::rep;
		using period					= std::chrono::milliseconds::period;
		using duration					= std::chrono::milliseconds;
		using time_point				= std::chrono::time_point<fast_monotonic_clock>;
		static constexpr bool is_steady	= true;

		static auto now() noexcept {
			return time_point(duration(static_cast<rep>(GetTickCount64())));
		}
	};

}

#endif // !_ALT_CLOCKS_HPP_